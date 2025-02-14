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

#include "pasteboard_service_stub.h"

#include "errors.h"
#include "paste_data.h"
#include "pasteboard_common.h"
#include "pasteboard_hilog_wreapper.h"

namespace OHOS {
namespace MiscServices {
PasteboardServiceStub::PasteboardServiceStub()
{
    memberFuncMap_[static_cast<uint32_t>(GET_PASTE_DATA)] = &PasteboardServiceStub::OnGetPasteData;
    memberFuncMap_[static_cast<uint32_t>(HAS_PASTE_DATA)] = &PasteboardServiceStub::OnHasPasteData;
    memberFuncMap_[static_cast<uint32_t>(SET_PASTE_DATA)] = &PasteboardServiceStub::OnSetPasteData;
    memberFuncMap_[static_cast<uint32_t>(CLEAR_ALL)] = &PasteboardServiceStub::OnClear;
    memberFuncMap_[static_cast<uint32_t>(ADD_OBSERVER)] =
        &PasteboardServiceStub::OnAddPasteboardChangedObserver;
    memberFuncMap_[static_cast<uint32_t>(DELETE_OBSERVER)] =
        &PasteboardServiceStub::OnRemovePasteboardChangedObserver;
    memberFuncMap_[static_cast<uint32_t>(DELETE_ALL_OBSERVER)] =
        &PasteboardServiceStub::OnRemoveAllChangedObserver;
}

int32_t PasteboardServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "start##code = %{public}u", code);
    std::u16string myDescripter = PasteboardServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_SERVICE, "end##descriptor checked fail");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    pid_t p = IPCSkeleton::GetCallingPid();
    pid_t p1 = IPCSkeleton::GetCallingUid();
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE,
        "CallingPid = %{public}d, CallingUid = %{public}d, code = %{public}u",
        p, p1, code);
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    int ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "end##ret = %{public}d", ret);
    return ret;
}
int32_t PasteboardServiceStub::OnClear(MessageParcel &data, MessageParcel &reply)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "start.");
    Clear();
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "end.");
    return ERR_OK;
}

int32_t PasteboardServiceStub::OnGetPasteData(MessageParcel &data, MessageParcel &reply)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, " start.");
    PasteData pasteData {};
    auto hasPasteData = GetPasteData(pasteData);
    if (!hasPasteData) {
        PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, " end.");
        return ERR_INVALID_VALUE;
    }
    if (!reply.WriteParcelable(&pasteData)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_SERVICE, "Failed to write parcelable pasteData");
        return ERR_INVALID_VALUE;
    }
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, " end.");
    return ERR_OK;
}
int32_t PasteboardServiceStub::OnHasPasteData(MessageParcel &data, MessageParcel &reply)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, " start.");
    auto result = HasPasteData();
    reply.WriteBool(result);
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, " end.");
    return ERR_OK;
}

int32_t PasteboardServiceStub::OnSetPasteData(MessageParcel &data, MessageParcel &reply)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, " start.");
    std::unique_ptr<PasteData> pasteData(data.ReadParcelable<PasteData>());
    if (!pasteData) {
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, " start.");
        return ERR_INVALID_VALUE;
    }

    SetPasteData(*pasteData);
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, " end.");
    return ERR_OK;
}
int32_t PasteboardServiceStub::OnAddPasteboardChangedObserver(MessageParcel &data, MessageParcel &reply)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "start.");
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    if (obj == nullptr) {
        PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "obj nullptr");
        return ERR_INVALID_VALUE;
    }
    sptr<IPasteboardChangedObserver> callback = iface_cast<IPasteboardChangedObserver>(obj);
    if (callback == nullptr) {
        PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "callback nullptr");
        return ERR_INVALID_VALUE;
    }
    AddPasteboardChangedObserver(callback);
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "end.");
    return ERR_OK;
}
int32_t PasteboardServiceStub::OnRemovePasteboardChangedObserver(MessageParcel &data, MessageParcel &reply)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "start.");
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    if (obj == nullptr) {
        PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "obj nullptr");
        return ERR_INVALID_VALUE;
    }
    sptr<IPasteboardChangedObserver> callback = iface_cast<IPasteboardChangedObserver>(obj);
    if (callback == nullptr) {
        PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "callback nullptr");
        return ERR_INVALID_VALUE;
    }
    RemovePasteboardChangedObserver(callback);
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "end.");
    return ERR_OK;
}

int32_t PasteboardServiceStub::OnRemoveAllChangedObserver(MessageParcel &data, MessageParcel &reply)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "start.");
    RemoveAllChangedObserver();
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "end.");
    return ERR_OK;
}

PasteboardServiceStub::~PasteboardServiceStub()
{
    memberFuncMap_.clear();
}
} // namespace MiscServices
} // namespace OHOS