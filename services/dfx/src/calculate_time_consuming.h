/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef TIME_CONSUMING_H
#define TIME_CONSUMING_H

#include <ctime>
#include <sys/time.h>
#include "singleton.h"

namespace OHOS {
namespace MiscServices {
class CalculateTimeConsuming : public Singleton<CalculateTimeConsuming> {
public:
     CalculateTimeConsuming(const size_t calPasteboardData, const int calPasteboardState);
     ~CalculateTimeConsuming();
private:
    static uint64_t GetCurrentTimeMicros();
    int CalculateTime(uint64_t time);
    int CalculateData(size_t calPasteboardData);
    
    int pasteboardData_;
    int pasteboardState_;
    uint64_t lastTime_{ 0 };
};
} // namespace MiscServices
} // namespace OHOS
#endif // TIME_CONSUMING_H
