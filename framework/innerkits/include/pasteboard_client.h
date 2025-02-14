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

#ifndef PASTE_BOARD_CLIENT_H
#define PASTE_BOARD_CLIENT_H

#include <functional>
#include <singleton.h>
#include "paste_data_record.h"
#include "paste_data.h"
#include "i_pasteboard_service.h"
#include "pasteboard_observer.h"
#include "want.h"

namespace OHOS {
namespace MiscServices {
class PasteboardSaDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit PasteboardSaDeathRecipient();
    ~PasteboardSaDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;
private:
    DISALLOW_COPY_AND_MOVE(PasteboardSaDeathRecipient);
};

class PasteboardClient : public DelayedSingleton<PasteboardClient>  {
    DECLARE_DELAYED_SINGLETON(PasteboardClient);
public:
    DISALLOW_COPY_AND_MOVE(PasteboardClient);

    /**
     * CreateHtmlTextRecord
     * @descrition Create Html Text Record.
     * @param std::string text.
     * @return PasteDataRecord.
     */
    std::shared_ptr<PasteDataRecord> CreateHtmlTextRecord(const std::string &text);

    /**
     * CreatePlainTextRecord
     * @descrition Create Plaint Text Record.
     * @param std::string text.
     * @return PasteDataRecord.
     */
    std::shared_ptr<PasteDataRecord> CreatePlainTextRecord(const std::string &text);

    /**
     * CreateUriRecord
     * @descrition Create Uri Text Record.
     * @param OHOS::Uri uri.
     * @return PasteDataRecord.
     */
    std::shared_ptr<PasteDataRecord> CreateUriRecord(const OHOS::Uri &uri);

    /**
     * CreateWantRecord
     * @descrition Create Plaint Want Record.
     * @param OHOS::AAFwk::Want want.
     * @return PasteDataRecord.
     */
    std::shared_ptr<PasteDataRecord> CreateWantRecord(std::shared_ptr<OHOS::AAFwk::Want> want);

    /**
     * CreateHtmlData
     * @descrition Create Html Paste Data.
     * @param std::string text  .
     * @return PasteData.
     */
    std::shared_ptr<PasteData> CreateHtmlData(const std::string &htmlText);

    /**
     * CreatePlainTextData
     * @descritionCreate Plain Text Paste Data.
     * @param std::string text .
     * @return PasteData.
     */
    std::shared_ptr<PasteData> CreatePlainTextData(const std::string &text);

    /**
     * CreateUriData
     * @descrition Create Uri Paste Data.
     * @param OHOS::Uri uri .
     * @return PasteData.
     */
    std::shared_ptr<PasteData> CreateUriData(const OHOS::Uri &uri);

    /**
     * CreateWantData
     * @descrition Create Want Paste Data.
     * @param OHOS::AAFwk::Want want .
     * @return PasteData.
     */
    std::shared_ptr<PasteData> CreateWantData(std::shared_ptr<OHOS::AAFwk::Want> want);
    /**
     * GetPasteData
     * @descrition
     * @return bool.
     */
    bool GetPasteData(PasteData& pasteData);

    /**
     * HasPasteData
     * @descrition
     * @return bool true on success, false on failure.
     */
    bool HasPasteData();

    /**
     * Clear
     * @descrition Clear Current pasteboard data
     * @return bool true on success, false on failure.
     */
    void Clear();

    /**
     * SetPasteData
     * @descrition
     * @param pasteData .
     * @return void.
     */
    void SetPasteData(PasteData& pasteData);

    /**
     * AddPasteboardChangedObserver
     * @descrition
     * @param observer pasteboard change callback.
     * @return void.
     */
    void AddPasteboardChangedObserver(std::shared_ptr<PasteboardObserver> callback);

    /**
     * RemovePasteboardChangedObserver
     * @descrition
     * @param observer pasteboard change callback.
     * @return void.
     */
    void RemovePasteboardChangedObserver(std::shared_ptr<PasteboardObserver> callback);

    void OnRemoteSaDied(const wptr<IRemoteObject> &object);
private:
    void ConnectService();

    static sptr<IPasteboardService> pasteboardServiceProxy_;
    static std::mutex instanceLock_;

    sptr<IRemoteObject::DeathRecipient> deathRecipient_ {nullptr};
};
} // MiscServices
} // OHOS
#endif // PASTE_BOARD_CLIENT_H