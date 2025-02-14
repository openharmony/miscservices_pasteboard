/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include "string_ex.h"
#include "system_ability_definition.h"
#include "pasteboard_observer.h"
#include "pasteboard_common.h"
#include "pasteboard_client.h"

namespace OHOS {
namespace MiscServices {
sptr<IPasteboardService> PasteboardClient::pasteboardServiceProxy_;
std::mutex PasteboardClient::instanceLock_;

PasteboardClient::PasteboardClient() {};
PasteboardClient::~PasteboardClient()
{
    if (pasteboardServiceProxy_ != nullptr) {
        auto remoteObject = pasteboardServiceProxy_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
    }
}

std::shared_ptr<PasteDataRecord> PasteboardClient::CreateHtmlTextRecord(const std::string &htmlText)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "New text record: %{public}s", htmlText.c_str());
    return PasteDataRecord::NewHtmlRecord(htmlText);
}

std::shared_ptr<PasteDataRecord> PasteboardClient::CreateWantRecord(std::shared_ptr<OHOS::AAFwk::Want> want)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "New want record");
    return PasteDataRecord::NewWantRecord(std::move(want));
}

std::shared_ptr<PasteDataRecord> PasteboardClient::CreatePlainTextRecord(const std::string &text)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "New text record: %{public}s", text.c_str());
    return PasteDataRecord::NewPlaintTextRecord(text);
}

std::shared_ptr<PasteDataRecord> PasteboardClient::CreateUriRecord(const OHOS::Uri &uri)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "New uri record");
    return PasteDataRecord::NewUriRecord(uri);
}

std::shared_ptr<PasteData> PasteboardClient::CreateHtmlData(const std::string &htmlText)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "New htmlText data: %{public}s", htmlText.c_str());
    auto pasteData = std::make_shared<PasteData>();
    pasteData->AddHtmlRecord(htmlText);
    return pasteData;
}

std::shared_ptr<PasteData> PasteboardClient::CreateWantData(std::shared_ptr<OHOS::AAFwk::Want> want)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "New want data");
    auto pasteData = std::make_shared<PasteData>();
    pasteData->AddWantRecord(std::move(want));
    return pasteData;
}

std::shared_ptr<PasteData> PasteboardClient::CreatePlainTextData(const std::string &text)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "New plain data: %{public}s", text.c_str());
    auto pasteData = std::make_shared<PasteData>();
    pasteData->AddTextRecord(text);
    return pasteData;
}

std::shared_ptr<PasteData> PasteboardClient::CreateUriData(const OHOS::Uri &uri)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "New uri data");
    auto pasteData = std::make_shared<PasteData>();
    pasteData->AddUriRecord(uri);
    return pasteData;
}

void PasteboardClient::Clear()
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "start.");
    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGW(PASTEBOARD_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }

    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "GetPasteData quit.");
        return;
    }
    pasteboardServiceProxy_->Clear();
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "end.");
    return;
}

bool PasteboardClient::GetPasteData(PasteData& pasteData)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "start.");
    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGW(PASTEBOARD_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }

    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "GetPasteData quit.");
        return false;
    }
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "end.");
    return pasteboardServiceProxy_->GetPasteData(pasteData);
}

bool PasteboardClient::HasPasteData()
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "start.");
    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGW(PASTEBOARD_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }

    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "HasPasteData quit ");
        return false;
    }
    auto result = pasteboardServiceProxy_->HasPasteData();
    return result;
}

void PasteboardClient::SetPasteData(PasteData& pasteData)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "start.");
    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGW(PASTEBOARD_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }

    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "SetPasteData quit.");
        return;
    }
    pasteboardServiceProxy_->SetPasteData(pasteData);
}

void PasteboardClient::AddPasteboardChangedObserver(std::shared_ptr<PasteboardObserver> callback)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "start.");
    if (callback == nullptr) {
        PASTEBOARD_HILOGW(PASTEBOARD_MODULE_CLIENT, "input nullptr.");
        return;
    }
    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGW(PASTEBOARD_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }

    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "AddPasteboardChangedObserver quit.");
        return;
    }

    auto remoteObject = callback->AsObject();
    sptr<IPasteboardChangedObserver> observerPtr = iface_cast<IPasteboardChangedObserver>(remoteObject);
    pasteboardServiceProxy_->AddPasteboardChangedObserver(observerPtr);
    return;
}

void PasteboardClient::RemovePasteboardChangedObserver(std::shared_ptr<PasteboardObserver> callback)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "start.");
    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGW(PASTEBOARD_MODULE_CLIENT, "Redo ConnectService");
        ConnectService();
    }

    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "quit.");
        return;
    }
    if (callback == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "remove all.");
        pasteboardServiceProxy_->RemoveAllChangedObserver();
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "end.");
        return;
    }
    auto remoteObject = callback->AsObject();
    sptr<IPasteboardChangedObserver> observerPtr = iface_cast<IPasteboardChangedObserver>(remoteObject);
    pasteboardServiceProxy_->RemovePasteboardChangedObserver(observerPtr);
    PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "end.");
    return;
}

void PasteboardClient::ConnectService()
{
    std::lock_guard<std::mutex> lock(instanceLock_);
    if (pasteboardServiceProxy_ != nullptr) {
        return ;
    }
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "start.");
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "Getting SystemAbilityManager failed.");
        pasteboardServiceProxy_ = nullptr;
        return;
    }
    sptr<IRemoteObject> remoteObject = sam->CheckSystemAbility(PASTEBOARD_SERVICE_ID);
    if (remoteObject == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "GetSystemAbility failed!");
        return;
    }

    deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new PasteboardSaDeathRecipient());
    if (deathRecipient_ == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "Getting deathRecipient_ failed.");
        return;
    }
    if ((remoteObject->IsProxyObject()) && (!remoteObject->AddDeathRecipient(deathRecipient_))) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "Add death recipient to paste service failed.");
        return;
    }

    pasteboardServiceProxy_ = iface_cast<IPasteboardService>(remoteObject);
    if (pasteboardServiceProxy_ == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "Get PasteboardServiceProxy from SA failed.");
        return ;
    }
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_CLIENT, "Getting PasteboardServiceProxy succeeded.");
}

void PasteboardClient::OnRemoteSaDied(const wptr<IRemoteObject> &remote)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "start.");
    ConnectService();
}

PasteboardSaDeathRecipient::PasteboardSaDeathRecipient()
{
}

void PasteboardSaDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    PASTEBOARD_HILOGE(PASTEBOARD_MODULE_CLIENT, "PasteboardSaDeathRecipient on remote systemAbility died.");
    PasteboardClient::GetInstance()->OnRemoteSaDied(object);
}
} // MiscServices
} // OHOS