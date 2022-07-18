/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * miscservices under the License is miscservices on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MISCSERVICES_PASTEBOARD_HI_VIEW_ADAPTER_H
#define MISCSERVICES_PASTEBOARD_HI_VIEW_ADAPTER_H

#include <sys/time.h>

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "dfx_code_constant.h"
#include "dfx_types.h"
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
    TIME_LEVEL_ONE = 72,
    TIME_LEVEL_TWO = 73,
    TIME_LEVEL_THREE = 74,
    TIME_LEVEL_FOUR = 75,
    TIME_LEVEL_FIVE = 76,
    TIME_LEVEL_SIX = 77,
    TIME_LEVEL_SEVEN = 78,
    TIME_LEVEL_EIGHT = 79,
    TIME_LEVEL_NINE = 80,
    TIME_LEVEL_TEN = 81,
    TIME_LEVEL_ELEVEN = 82,
};

class HiViewAdapter {
public:
    ~HiViewAdapter();
    static void ReportInitializationFault(int dfxCode, const InitializationFaultMsg &msg);
    static void ReportTimeConsumingStatistic(int dfxCode, const TimeConsumingStat &stat);
    static void ReportPasteboardBehaviour(int dfxCode, const PasteboardBehaviourMsg &msg);
    static void StartTimerThread();
    static std::map<int, int> InitDataMap();
    static std::map<int, int> InitTimeMap();

private:
    static void InvokePasteBoardBehaviour();
    static void InitializeTimeConsuming(int initFlag);
    static void CopyTimeConsumingCount(const TimeConsumingStat &stat, int dataLevel, int timeLevel);
    static void PasteTimeConsumingCount(const TimeConsumingStat &stat, int dataLevel, int timeLevel);
    static void CopyTimeConsuming(const TimeConsumingStat &stat, int level);
    static void PasteTimeConsuming(const TimeConsumingStat &stat, int level);
    static const char *GetDataLevel(int dataLevel);
    static void InvokeTimeConsuming();
    static void ReportBehaviour(std::map<std::string, int> &behaviour, const char *statePasteboard);

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
    
    static const inline int ONE_DAY_IN_HOURS  = 24;
    static const inline int EXEC_HOUR_TIME = 23;
    static const inline int EXEC_MIN_TIME = 60;
    static const inline int ONE_MINUTE_IN_SECONDS  = 60;
    static const inline int ONE_HOUR_IN_SECONDS = 1 * 60 * 60; // 1 hour
};
}  // namespace MiscServices
}  // namespace OHOS
#endif // MISCSERVICES_PASTEBOARD_HI_VIEW_ADAPTER_H
