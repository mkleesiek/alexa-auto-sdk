/*
 * Copyright 2019-2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/pointer.h>
#include <rapidjson/writer.h>

#include <AACE/Engine/Core/EngineMacros.h>
#include <AACE/Engine/Utils/Metrics/Metrics.h>
#include <AACE/Engine/AddressBook/AddressBookEngineImpl.h>
#include <AACE/AddressBook/AddressBook.h>

namespace aace {
namespace engine {
namespace addressBook {

using namespace aace::engine::utils::metrics;

/// Program Name for Metrics
static const std::string METRIC_PROGRAM_NAME_SUFFIX = "AddressBookEngineImpl";

/// Counter metrics for AddressBook Platform APIs
static const std::string METRIC_ADD_ADDRESS_BOOK = "AddAddressBook";
static const std::string METRIC_REMOVE_ADDRESS_BOOK = "RemoveAddressBook";

// String to identify log entries originating from this file.
static const std::string TAG("aace.engine.addressBook.addressBookEngineImpl");

std::shared_ptr<AddressBookEngineImpl> AddressBookEngineImpl::create(
    std::shared_ptr<aace::addressBook::AddressBook> platformInterface) {
    try {
        auto addressBookEngineImpl =
            std::shared_ptr<AddressBookEngineImpl>(new AddressBookEngineImpl(platformInterface));
        ThrowIfNull(addressBookEngineImpl, "addressBookEngineImplIsNull");

        return addressBookEngineImpl;
    } catch (std::exception& ex) {
        AACE_ERROR(LX(TAG, "create").d("reason", ex.what()));
        return nullptr;
    }
}

AddressBookEngineImpl::AddressBookEngineImpl(std::shared_ptr<aace::addressBook::AddressBook> platformInterface) :
        alexaClientSDK::avsCommon::utils::RequiresShutdown(TAG), m_platformInterface(platformInterface) {
}

void AddressBookEngineImpl::addObserver(std::shared_ptr<AddressBookObserver> observer) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_observers.insert(observer);

    for (auto& pair : m_addressBookEntities) {
        auto addressBookEntity = pair.second;
        observer->addressBookAdded(addressBookEntity);
    }
}

void AddressBookEngineImpl::doShutdown() {
    for (auto observer : m_observers) {
        observer.reset();
    }
    m_addressBookEntities.clear();
    m_platformInterface.reset();
}

void AddressBookEngineImpl::removeObserver(std::shared_ptr<AddressBookObserver> observer) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_observers.erase(observer);
}

bool AddressBookEngineImpl::onAddAddressBook(
    const std::string& addressBookSourceId,
    const std::string& name,
    const AddressBookType type) {
    try {
        emitCounterMetrics(METRIC_PROGRAM_NAME_SUFFIX, "onAddAddressBook", METRIC_ADD_ADDRESS_BOOK, 1);
        ThrowIf(addressBookSourceId.empty(), "addressBookSourceIdInvalid");
        std::lock_guard<std::mutex> guard(m_mutex);

        ThrowIfNot(
            m_addressBookEntities.find(addressBookSourceId) == m_addressBookEntities.end(), "addressBookSourceIdFound");

        auto addressBookEntity = std::make_shared<AddressBookEntity>(addressBookSourceId, name, type);
        m_addressBookEntities[addressBookSourceId] = addressBookEntity;

        for (auto& observer : m_observers) {
            ThrowIfNot(observer->addressBookAdded(addressBookEntity), "addressBookAddedFailed");
        }

        return true;
    } catch (std::exception& ex) {
        AACE_ERROR(LX(TAG, "onAddAddressBook").d("addressBookSourceId", addressBookSourceId).d("reason", ex.what()));
        return false;
    }
}

bool AddressBookEngineImpl::onRemoveAddressBook(const std::string& addressBookSourceId) {
    try {
        emitCounterMetrics(METRIC_PROGRAM_NAME_SUFFIX, "onRemoveAddressBook", METRIC_REMOVE_ADDRESS_BOOK, 1);

        std::lock_guard<std::mutex> guard(m_mutex);

        if (!addressBookSourceId.empty()) {
            auto it = m_addressBookEntities.find(addressBookSourceId);
            ThrowIf(it == m_addressBookEntities.end(), "addressBookSourceIdNotFound");

            auto addressBookEntity = m_addressBookEntities[addressBookSourceId];
            m_addressBookEntities.erase(it);

            for (auto observer : m_observers) {
                ThrowIfNot(observer->addressBookRemoved(addressBookEntity), "addressBookRemovedFailed");
            }
        } else {
            m_addressBookEntities.clear();

            // Remove the uploaded phone book by constructing a dummy address book entity with CONTACT type
            // and notifying all observers. Since AddressBookCloudUploader is one of the observers and allows
            // only one address book of each type, it will ignore the empty client-side source ID and delete
            // the corresponding address book on the cloud by specified address book type.
            auto phoneBookEntity = std::make_shared<AddressBookEntity>("", "", AddressBookType::CONTACT);
            for (auto observer : m_observers) {
                ThrowIfNot(observer->addressBookRemoved(phoneBookEntity), "addressBookRemovedFailed");
            }

            // Do the same for the uploaded navigation books
            auto navigationBookEntity = std::make_shared<AddressBookEntity>("", "", AddressBookType::NAVIGATION);
            for (auto observer : m_observers) {
                ThrowIfNot(observer->addressBookRemoved(navigationBookEntity), "addressBookRemovedFailed");
            }
        }

        return true;
    } catch (std::exception& ex) {
        AACE_ERROR(LX(TAG, "onRemoveAddressBook").d("addressBookSourceId", addressBookSourceId).d("reason", ex.what()));
        return false;
    }
}

bool AddressBookEngineImpl::getEntries(
    const std::string& addressBookSourceId,
    std::weak_ptr<aace::addressBook::AddressBook::IAddressBookEntriesFactory> factory) {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_platformInterface->getEntries(addressBookSourceId, factory);
}

}  // namespace addressBook
}  // namespace engine
}  // namespace aace
