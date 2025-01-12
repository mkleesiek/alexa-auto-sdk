package com.amazon.alexa.auto.lwa;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import com.amazon.aacsconstants.AACSConstants;
import com.amazon.aacsconstants.Action;
import com.amazon.aacsconstants.Topic;
import com.amazon.aacsipc.AACSSender;
import com.amazon.aacsipc.IPCConstants;
import com.amazon.alexa.auto.aacs.common.AACSMessage;
import com.amazon.alexa.auto.aacs.common.AACSMessageBuilder;
import com.amazon.alexa.auto.aacs.common.AACSMessageSender;
import com.amazon.alexa.auto.apis.auth.AuthState;
import com.amazon.alexa.auto.apis.auth.AuthWorkflowData;
import com.amazon.alexa.auto.apis.auth.CodePair;

import org.greenrobot.eventbus.EventBus;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.security.GeneralSecurityException;
import java.util.Optional;

public class CBLReceiver extends BroadcastReceiver {
    private static final String TAG = CBLReceiver.class.getSimpleName();

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent == null || intent.getAction() == null) {
            return;
        }

        AACSMessageBuilder.parseEmbeddedIntent(intent).ifPresent(message -> {
            switch (message.action) {
                case Action.CBL.CBL_STATE_CHANGED:
                    handleCBLStateChanged(message);
                    break;
                case Action.CBL.SET_REFRESH_TOKEN:
                    try {
                        handleSetRefreshToken(context, message);
                    } catch (GeneralSecurityException | IOException e) {
                        Log.e(TAG, "Failed to set refresh token.");
                    }
                    break;
                case Action.CBL.GET_REFRESH_TOKEN:
                    try {
                        handleGetRefreshToken(context, message);
                    } catch (GeneralSecurityException | IOException e) {
                        Log.e(TAG, "Failed to get refresh token.");
                    }
                    break;
                case Action.CBL.CLEAR_REFRESH_TOKEN:
                    try {
                        handleClearRefreshToken(context, message);
                    } catch (GeneralSecurityException | IOException e) {
                        Log.e(TAG, "Failed to clear refresh token.");
                    }
                    break;
                case Action.CBL.SET_USER_PROFILE:
                    try {
                        handleSetUserProfile(context, message);
                    } catch (GeneralSecurityException | IOException e) {
                        Log.e(TAG, "Failed to set user profile.");
                    }
            }
        });
    }

    /**
     * Handles AASB CBL message with CBLStateChanged action.
     * Generates QR code if code/url is provided.
     *
     */
    private void handleCBLStateChanged(AACSMessage message) {
        String state = null;
        String url = "";
        String code = "";

        try {
            JSONObject obj = new JSONObject(message.payload);

            if (obj.has("state") && obj.has("url") && obj.has("code")) {
                url = obj.getString("url");
                code = obj.getString("code");
                state = obj.getString("state");

                if (code.isEmpty() || url.isEmpty()) {
                    Log.w(TAG, "CBL code or URL empty.");
                }
            } else {
                Log.w(TAG, "CBLStateChanged JSON is missing data. json:" + message.payload);
            }

        } catch (JSONException e) {
            Log.e(TAG, "CBLStateChanged JSON cannot be parsed.");
        }

        switch (state) {
            case "CODE_PAIR_RECEIVED":
                Log.d(TAG, "CODE_PAIR_RECEIVED");
                EventBus.getDefault().post(
                        new AuthWorkflowData(AuthState.CBL_Auth_CodePair_Received, new CodePair(url, code), null));
                break;
            case "STARTING":
                Log.d(TAG, "CBLStateChanged: Starting");
                EventBus.getDefault().post(new AuthWorkflowData(AuthState.CBL_Auth_Started, null, null));
                break;
            case "STOPPING":
                Log.d(TAG, "CBLStateChanged: Stopping");
                EventBus.getDefault().post(new AuthWorkflowData(AuthState.CBL_Auth_Not_Started, null, null));
                break;
            default:
                break;
        }
    }

    private void handleSetRefreshToken(Context context, AACSMessage message)
            throws GeneralSecurityException, IOException {
        Log.d(TAG, "handleSetRefreshToken");
        String refreshToken = "";
        try {
            JSONObject obj = new JSONObject(message.payload);

            if (obj.has("refreshToken")) {
                refreshToken = obj.getString("refreshToken");
                if (refreshToken.isEmpty()) {
                    Log.w(TAG, "refreshToken is empty.");
                }
            } else {
                Log.w(TAG, "SetRefreshToken JSON is missing data. json:" + message.payload);
            }

        } catch (JSONException e) {
            Log.e(TAG, "SetRefreshToken JSON cannot be parsed.");
        }

        TokenStore.saveRefreshToken(context, refreshToken);

        EventBus.getDefault().post(new AuthWorkflowData(AuthState.CBL_Auth_Token_Saved, null, null));
    }

    private void handleGetRefreshToken(Context context, AACSMessage message)
            throws GeneralSecurityException, IOException {
        Log.d(TAG, "handleGetRefreshToken");
        Optional<String> refreshToken = TokenStore.getRefreshToken(context);
        JSONObject payloadJson;
        try {
            payloadJson = new JSONObject();
            payloadJson.put("refreshToken", refreshToken.orElse(""));
        } catch (Exception e) {
            Log.e(TAG, e.toString());
            return;
        }

        new AACSMessageSender(new WeakReference<>(context), new AACSSender())
                .sendReplyMessage(message.messageId, Topic.CBL, Action.CBL.GET_REFRESH_TOKEN, payloadJson.toString());
    }

    private void handleClearRefreshToken(Context context, AACSMessage message)
            throws GeneralSecurityException, IOException {
        Log.d(TAG, "handleClearRefreshToken");
        TokenStore.resetRefreshToken(context);
    }

    private void handleSetUserProfile(Context context, AACSMessage message)
            throws GeneralSecurityException, IOException {
        Log.d(TAG, "handleSetUserProfile");
        String userName = "";
        try {
            JSONObject obj = new JSONObject(message.payload);

            if (obj.has("name")) {
                userName = obj.getString("name");
                if (userName.isEmpty()) {
                    Log.w(TAG, "User name is empty.");
                } else {
                    UserIdentityStore.saveUserIdentity(context, userName);
                    EventBus.getDefault().post(
                            new AuthWorkflowData(AuthState.CBL_Auth_User_Identity_Saved, null, null));
                }
            } else {
                Log.w(TAG, "handleSetUserProfile: JSON is missing user name. json:" + message.payload);
            }
        } catch (JSONException e) {
            Log.e(TAG, "handleSetUserProfile: JSON cannot be parsed.");
        }
    }

    public static void sendMessage(Context context, String topic, String action, String message) {
        Intent intent = new Intent();
        intent.setComponent(new ComponentName(
                AACSConstants.getAACSPackageName(new WeakReference<Context>(context)), AACSConstants.AACS_CLASS_NAME));
        intent.setAction(action);
        intent.addCategory(topic);

        Bundle bundle = new Bundle();
        bundle.putString("type", IPCConstants.AacsIpcMessageType.EMBEDDED.getTypeAsString());
        bundle.putString("message", message);

        intent.putExtra("payload", bundle);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            context.startForegroundService(intent);
        } else {
            context.startService(intent);
        }
    }
}
