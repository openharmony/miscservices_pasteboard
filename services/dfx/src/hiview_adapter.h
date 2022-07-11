/*
 * Copyright (C) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef MISCSERVICES_PASTEBOARD_HI_VIEW_ADAPTER_H
#define MISCSERVICES_PASTEBOARD_HI_VIEW_ADAPTER_H

#include <map>
#include <mutex>
#include <vector>
#include <string>
#include <sys/time.h>
#include "dfx_types.h"
#include "hisysevent.h"
#include "dfx_code_constant.h"
#include "hisysevent.h"

namespace OHOS {
namespace MiscServices {
enum class DataConsumingLevel {
    DATA_LEVEL_ONE = 0,
    DATA_LEVEL_TWO = 1,
    DATA_LEVEL_THREE = 2,
    DATA_LEVEL_FOUR = 3,
    DATA_LEVEL_FIVE = 4,
    DATA_LEVEL_SIX = 5,
    DATA_LEVEL_SEVEN = 6,
};
enum class TimeConsumingLevel {
    TIME_LEVEL_ONE = 9,
    TIME_LEVEL_TWO = 10,
    TIME_LEVEL_THREE = 11,
    TIME_LEVEL_FOUR = 12,
    TIME_LEVEL_FIVE = 13,
    TIME_LEVEL_SIX = 14,
    TIME_LEVEL_SEVEN = 15,
    TIME_LEVEL_EIGHT = 16,
    TIME_LEVEL_NINE = 17,
    TIME_LEVEL_TEN = 18,
    TIME_LEVEL_ELEVEN = 19,
};

class HiViewAdapter {
public:
    ~HiViewAdapter();
    static void ReportInitializationFault(int dfxCode, const InitializationFaultMsg &msg);
    static void ReportTimeConsumingStatistic(const TimeConsumingStat &stat);
    static void ReportPasteboardBehaviour(const PasteboardBehaviourMsg &msg);
    static void StartTimerThread();
    static std::map<int, int> InitDataMap();
    static std::map<int, int> InitTimeMap();

private:
    static void InvokePasteBoardBehaviour();
    static void InitializeTimeConsuming(int initFlag);
    static void CopyTimeConsumingCount(int dataLevel, int timeLevel);
    static void PasteTimeConsumingCount(int dataLevel, int timeLevel);
    static void CopyTimeConsuming(const TimeConsumingStat &stat, int level);
    static void PasteTimeConsuming(const TimeConsumingStat &stat, int level);
    static const char *GetDataLevel(int dataLevel);
    static void InvokeTimeConsuming();
    static void ReportBehaviour(std::map<std::string, int> &behaviour, const char *statePasteboard);
    static void ReportStatisticEvent(std::vector<std::map<int, int>> &timeConsumingStat, std::string pasteboardState);

    static std::mutex timeConsumingMutex_;
    static std::vector<std::map<int, int>> copyTimeConsumingStat_;
    static std::vector<std::map<int, int>> pasteTimeConsumingStat_;

    static std::mutex behaviourMutex_;
    static std::map<std::string, int> copyPasteboardBehaviour_;
    static std::map<std::string, int> pastePasteboardBehaviour_;
    
    static std::map<int, int> dataMap_;
    static std::map<int, int> timeMap_;

    static std::string CoverEventID(int dfxCode);
private:
    static std::mutex runMutex_;
    static bool running_;
    
    static inline constexpr int ONE_DAY_IN_HOURS  = 24;
    static inline constexpr int EXEC_HOUR_TIME = 23;
    static inline constexpr int EXEC_MIN_TIME = 60;
    static inline constexpr int ONE_MINUTE_IN_SECONDS  = 60;
    static inline constexpr int ONE_HOUR_IN_SECONDS = 1 * 60 * 60; // 1 hour
};
}  // namespace MiscServices
}  // namespace OHOS
#endif // MISCSERVICES_PASTEBOARD_HI_VIEW_ADAPTER_H
