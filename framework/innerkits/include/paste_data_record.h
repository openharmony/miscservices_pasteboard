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

#ifndef PASTE_BOARD_RECORD_H
#define PASTE_BOARD_RECORD_H

#include <memory>
#include <string>
#include "parcel.h"
#include "string_ex.h"
#include "uri.h"
#include "want.h"

namespace OHOS {
namespace MiscServices {
namespace {
const std::string MIMETYPE_TEXT_HTML = "text/html";
const std::string MIMETYPE_TEXT_PLAIN = "text/plain";
const std::string MIMETYPE_TEXT_URI = "text/uri";
const std::string MIMETYPE_TEXT_WANT = "text/want";
}

class PasteDataRecord : public Parcelable {
public:
    PasteDataRecord() = default;

    PasteDataRecord(std::string mimeType,
                    std::shared_ptr<std::string> htmlText,
                    std::shared_ptr<OHOS::AAFwk::Want> want,
                    std::shared_ptr<std::string> plainText,
                    std::shared_ptr<OHOS::Uri> uri);

    static std::shared_ptr<PasteDataRecord> NewHtmlRecord(const std::string &htmlText);
    static std::shared_ptr<PasteDataRecord> NewWantRecord(std::shared_ptr<OHOS::AAFwk::Want> want);
    static std::shared_ptr<PasteDataRecord> NewPlaintTextRecord(const std::string &text);
    static std::shared_ptr<PasteDataRecord> NewUriRecord(const OHOS::Uri &uri);

    std::string GetMimeType() const;
    std::shared_ptr<std::string> GetHtmlText() const;
    std::shared_ptr<std::string> GetPlainText() const;
    std::shared_ptr<OHOS::Uri> GetUri() const;
    std::shared_ptr<OHOS::AAFwk::Want> GetWant() const;

    std::string ConvertToText() const;

    virtual bool Marshalling(Parcel &parcel) const override;
    static PasteDataRecord *Unmarshalling(Parcel &parcel);

private:
    bool ReadFromParcel(Parcel &parcel);
    std::string mimeType_;
    std::shared_ptr<std::string> htmlText_;
    std::shared_ptr<OHOS::AAFwk::Want> want_;
    std::shared_ptr<std::string> plainText_;
    std::shared_ptr<OHOS::Uri> uri_;
};
} // MiscServices
} // OHOS
#endif // PASTE_BOARD_RECORD_H