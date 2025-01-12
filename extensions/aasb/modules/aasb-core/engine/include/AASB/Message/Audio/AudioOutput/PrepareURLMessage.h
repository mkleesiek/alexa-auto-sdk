/*
 * Copyright 2017-2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/*********************************************************
**********************************************************
**********************************************************

THIS FILE IS AUTOGENERATED. DO NOT EDIT

**********************************************************
**********************************************************
*********************************************************/

#ifndef AUDIOOUTPUT_PREPAREURLMESSAGE_H
#define AUDIOOUTPUT_PREPAREURLMESSAGE_H

#include <string>
#include <vector>

#include <AACE/Engine/Utils/UUID/UUID.h>
#include <nlohmann/json.hpp>
#include "AASB/Message/Audio/AudioOutput/AudioOutputAudioType.h"

namespace aasb {
namespace message {
namespace audio {
namespace audioOutput {

//Class Definition
struct PrepareURLMessage {
    struct Header {
        struct MessageDescription {
            static const std::string& topic() {
                static std::string topic = "AudioOutput";
                return topic;
            }
            static const std::string& action() {
                static std::string action = "Prepare";
                return action;
            }
        };
        static const std::string& version() {
            static std::string version = "3.3";
            return version;
        }
        static const std::string& messageType() {
            static std::string messageType = "Publish";
            return messageType;
        }
        std::string id = aace::engine::utils::uuid::generateUUID();
        MessageDescription messageDescription;
    };
    struct Payload {
        using AudioOutputAudioType = ::aasb::message::audio::AudioOutputAudioType;

        static const std::string& source() {
            static std::string source = "URL";
            return source;
        }
        std::string channel;
        AudioOutputAudioType audioType;
        std::string token;
        std::string url;
        bool repeating;
    };
    static const std::string& topic() {
        static std::string topic = "AudioOutput";
        return topic;
    }
    static const std::string& action() {
        static std::string action = "Prepare";
        return action;
    }
    static const std::string& version() {
        static std::string version = "3.3";
        return version;
    }
    static const std::string& messageType() {
        static std::string messageType = "Publish";
        return messageType;
    }
    static const std::string& source() {
        static std::string source = "URL";
        return source;
    }
    std::string toString() const;
    Header header;
    Payload payload;
};

//JSON Serialization
inline void to_json(nlohmann::json& j, const PrepareURLMessage::Payload& c) {
    j = nlohmann::json{
        {"source", c.source()},
        {"channel", c.channel},
        {"audioType", c.audioType},
        {"token", c.token},
        {"url", c.url},
        {"repeating", c.repeating},
    };
}
inline void from_json(const nlohmann::json& j, PrepareURLMessage::Payload& c) {
    j.at("channel").get_to(c.channel);
    j.at("audioType").get_to(c.audioType);
    j.at("token").get_to(c.token);
    j.at("url").get_to(c.url);
    j.at("repeating").get_to(c.repeating);
}

inline void to_json(nlohmann::json& j, const PrepareURLMessage::Header::MessageDescription& c) {
    j = nlohmann::json{
        {"topic", c.topic()},
        {"action", c.action()},
    };
}
inline void from_json(const nlohmann::json& j, PrepareURLMessage::Header::MessageDescription& c) {
}

inline void to_json(nlohmann::json& j, const PrepareURLMessage::Header& c) {
    j = nlohmann::json{
        {"version", c.version()},
        {"messageType", c.messageType()},
        {"id", c.id},
        {"messageDescription", c.messageDescription},
    };
}
inline void from_json(const nlohmann::json& j, PrepareURLMessage::Header& c) {
    j.at("id").get_to(c.id);
    j.at("messageDescription").get_to(c.messageDescription);
}

inline void to_json(nlohmann::json& j, const PrepareURLMessage& c) {
    j = nlohmann::json{
        {"header", c.header},
        {"payload", c.payload},
    };
}
inline void from_json(const nlohmann::json& j, PrepareURLMessage& c) {
    j.at("header").get_to(c.header);
    j.at("payload").get_to(c.payload);
}

inline std::string PrepareURLMessage::toString() const {
    nlohmann::json j = *this;
    return j.dump(3);
}

}  // namespace audioOutput
}  // namespace audio
}  // namespace message
}  // namespace aasb

#endif  // AUDIOOUTPUT_PREPAREURLMESSAGE_H
