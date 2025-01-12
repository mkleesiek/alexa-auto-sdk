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

#ifndef AUTHORIZATION_STARTAUTHORIZATIONMESSAGE_H
#define AUTHORIZATION_STARTAUTHORIZATIONMESSAGE_H

#include <string>
#include <vector>

#include <AACE/Engine/Utils/UUID/UUID.h>
#include <nlohmann/json.hpp>

namespace aasb {
namespace message {
namespace authorization {
namespace authorization {

//Class Definition
struct StartAuthorizationMessage {
    struct Header {
        struct MessageDescription {
            static const std::string& topic() {
                static std::string topic = "Authorization";
                return topic;
            }
            static const std::string& action() {
                static std::string action = "StartAuthorization";
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
        std::string service;
        std::string data;
    };
    static const std::string& topic() {
        static std::string topic = "Authorization";
        return topic;
    }
    static const std::string& action() {
        static std::string action = "StartAuthorization";
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
    std::string toString() const;
    Header header;
    Payload payload;
};

//JSON Serialization
inline void to_json(nlohmann::json& j, const StartAuthorizationMessage::Payload& c) {
    j = nlohmann::json{
        {"service", c.service},
        {"data", c.data},
    };
}
inline void from_json(const nlohmann::json& j, StartAuthorizationMessage::Payload& c) {
    j.at("service").get_to(c.service);
    j.at("data").get_to(c.data);
}

inline void to_json(nlohmann::json& j, const StartAuthorizationMessage::Header::MessageDescription& c) {
    j = nlohmann::json{
        {"topic", c.topic()},
        {"action", c.action()},
    };
}
inline void from_json(const nlohmann::json& j, StartAuthorizationMessage::Header::MessageDescription& c) {
}

inline void to_json(nlohmann::json& j, const StartAuthorizationMessage::Header& c) {
    j = nlohmann::json{
        {"version", c.version()},
        {"messageType", c.messageType()},
        {"id", c.id},
        {"messageDescription", c.messageDescription},
    };
}
inline void from_json(const nlohmann::json& j, StartAuthorizationMessage::Header& c) {
    j.at("id").get_to(c.id);
    j.at("messageDescription").get_to(c.messageDescription);
}

inline void to_json(nlohmann::json& j, const StartAuthorizationMessage& c) {
    j = nlohmann::json{
        {"header", c.header},
        {"payload", c.payload},
    };
}
inline void from_json(const nlohmann::json& j, StartAuthorizationMessage& c) {
    j.at("header").get_to(c.header);
    j.at("payload").get_to(c.payload);
}

inline std::string StartAuthorizationMessage::toString() const {
    nlohmann::json j = *this;
    return j.dump(3);
}

}  // namespace authorization
}  // namespace authorization
}  // namespace message
}  // namespace aasb

#endif  // AUTHORIZATION_STARTAUTHORIZATIONMESSAGE_H
