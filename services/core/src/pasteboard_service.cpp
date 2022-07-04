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
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "os_account_manager.h"
#include "pasteboard_common.h"
#include "pasteboard_service.h"
#include "pasteboard_trace.h"

namespace OHOS {
namespace MiscServices {
namespace {
const std::int32_t INIT_INTERVAL = 10000L;
const std::string PASTEBOARD_SERVICE_NAME = "PasteboardService";
const std::int32_t ERROR_USERID = -1;
const bool G_REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<PasteboardService>::GetInstance().get());
const std::string FAIL_TO_GET_TIME_STAMP = "FAIL_TO_GET_TIME_STAMP";
}

std::vector<std::shared_ptr<std::string>> PasteboardService::dataHistory_;
std::shared_ptr<Command> PasteboardService::copyHistory;
std::shared_ptr<Command> PasteboardService::copyData;

PasteboardService::PasteboardService()
    : SystemAbility(PASTEBOARD_SERVICE_ID, true),
      state_(ServiceRunningState::STATE_NOT_START)
{
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "PasteboardService Start.");
}

PasteboardService::~PasteboardService() {}

int32_t PasteboardService::Init()
{
    if (!Publish(DelayedSingleton<PasteboardService>::GetInstance().get())) {
        PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "OnStart register to system ability manager failed.");
        return ERR_INVALID_OPTION;
    }
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "Init Success.");
    state_ = ServiceRunningState::STATE_RUNNING;
    return ERR_OK;
}

void PasteboardService::OnStart()
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "PasteboardService OnStart.");
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_SERVICE, "PasteboardService is already running.");
        return;
    }
    InitServiceHandler();
    if (Init() != ERR_OK) {
        auto callback = [=]() { Init(); };
        serviceHandler_->PostTask(callback, INIT_INTERVAL);
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_SERVICE, "Init failed. Try again 10s later.");
        return;
    }

    copyHistory = std::make_shared<Command>(std::vector<std::string>{ "--copy-history" },
        "Dump access history last ten times.",
        [this](const std::vector<std::string> &input, std::string &output) -> bool {
            output = DumpHistory();
            return true;
        });

    copyData = std::make_shared<Command>(std::vector<std::string>{ "-data" }, "Show copy data details.",
        [this](const std::vector<std::string> &input, std::string &output) -> bool {
            output = DunmpData();
            return true;
        });

    PasteboardDumpHelper::GetInstance().RegisterCommand(copyHistory);
    PasteboardDumpHelper::GetInstance().RegisterCommand(copyData);
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "Start PasteboardService success.");
    return;
}

void PasteboardService::OnStop()
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "OnStop Started.");
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        return;
    }
    serviceHandler_ = nullptr;
    state_ = ServiceRunningState::STATE_NOT_START;
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "OnStop End.");
}

void PasteboardService::InitServiceHandler()
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "InitServiceHandler started.");
    if (serviceHandler_ != nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_SERVICE, "Already init.");
        return;
    }
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(PASTEBOARD_SERVICE_NAME);
    serviceHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "InitServiceHandler Succeeded.");
}

void PasteboardService::InitStorage()
{
    if (pasteboardStorage_ == nullptr) {
        PASTEBOARD_HILOGI(PASTEBOARD_MODULE_SERVICE, "Init storage handler.");
    }

    if (pasteboardStorage_ == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_SERVICE, "Init storage handler failed.");
        return;
    }
}

void PasteboardService::Clear()
{
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "start.");
    auto userId = GetUserId();
    if (userId == ERROR_USERID) {
        return;
    }
    std::lock_guard<std::mutex> lock(clipMutex_);
    auto it = clips_.find(userId);
    if (it != clips_.end()) {
        clips_.erase(it);
        NotifyObservers();
    }
}

bool PasteboardService::GetPasteData(PasteData& data)
{
    PasteboardTrace tracer("PasteboardService, GetPasteData");
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "start.");
    auto userId = GetUserId();

    int32_t uid = IPCSkeleton::GetCallingUid();
    std::string time = GetTime();
    SetPasteboardHistory(uid, "Get", time);

    if (userId == ERROR_USERID) {
        return false;
    }
    std::lock_guard<std::mutex> lock(clipMutex_);
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "Clips length %{public}d.", static_cast<uint32_t>(clips_.size()));
    auto it = clips_.find(userId);
    if (it != clips_.end()) {
        PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "find end.");
        data = *(it->second);
        return true;
    } else {
        PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "not found end.");
        return false;
    }
}

bool PasteboardService::HasPasteData()
{
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "start.");
    auto userId = GetUserId();
    if (userId == ERROR_USERID) {
        return false;
    }
    std::lock_guard<std::mutex> lock(clipMutex_);
    return clips_.find(userId) != clips_.end();
}

void PasteboardService::SetPasteData(PasteData& pasteData)
{
    PasteboardTrace tracer("PasteboardService, SetPasteData");
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "start.");
    auto userId = GetUserId();

    int32_t uid = IPCSkeleton::GetCallingUid();
    uIdForLastCopy_ = uid;
    std::string time = GetTime();
    timeForLastCopy_ = time;
    SetPasteboardHistory(uid, "Set", time);

    if (userId == ERROR_USERID) {
        return;
    }
    std::lock_guard<std::mutex> lock(clipMutex_);
    auto it = clips_.find(userId);
    if (it != clips_.end()) {
        clips_.erase(it);
    }
    clips_.insert(std::make_pair(userId, std::make_shared<PasteData>(pasteData)));
    NotifyObservers();
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "Clips length %{public}d.", static_cast<uint32_t>(clips_.size()));
}

int32_t PasteboardService::GetUserId()
{
    int32_t userId = ERROR_USERID;
    int32_t uid = IPCSkeleton::GetCallingUid();
    auto result = AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(uid, userId);
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE,
        "Get UserId, uid = %{public}d, userId = %{public}d, result = %{public}d.", uid, userId, result);
    return userId;
}

void PasteboardService::AddPasteboardChangedObserver(const sptr<IPasteboardChangedObserver>& observer)
{
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "start.");
    if (observer == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_SERVICE, "nullptr.");
        return;
    }
    std::lock_guard<std::mutex> lock(observerMutex_);
    auto userId = GetUserId();
    if (userId == ERROR_USERID) {
        return;
    }
    auto it = observerMap_.find(userId);
    std::shared_ptr<std::set<const sptr<IPasteboardChangedObserver>, classcomp>> observers;
    if (it != observerMap_.end()) {
        observers = it->second;
    } else {
        observers = std::make_shared<std::set<const sptr<IPasteboardChangedObserver>, classcomp>>();
        observerMap_.insert(std::make_pair(userId, observers));
    }
    observers->insert(observer);
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE,
        " observer = %{public}p, observers->size = %{public}d,",
        observer.GetRefPtr(),
        static_cast<unsigned int>(observerMap_.size()));
}
void PasteboardService::RemovePasteboardChangedObserver(const sptr<IPasteboardChangedObserver>& observer)
{
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "start.");
    if (observer == nullptr) {
        return;
    }
    auto userId = GetUserId();
    if (userId == ERROR_USERID) {
        return;
    }
    std::lock_guard<std::mutex> lock(observerMutex_);
    auto it = observerMap_.find(userId);
    if (it == observerMap_.end()) {
        return;
    }
    auto observers = it->second;
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "observers->size: %{public}d.",
        static_cast<unsigned int>(observers->size()));
    auto eraseNum = observers->erase(observer);
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE,
        " callback = %{public}p, listeners.size = %{public}d,"
        " eraseNum = %{public}zu",
        observer.GetRefPtr(),
        static_cast<unsigned int>(observers->size()),
        eraseNum);
}

void PasteboardService::RemoveAllChangedObserver()
{
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "start.");
    auto userId = GetUserId();
    if (userId == ERROR_USERID) {
        return;
    }
    std::lock_guard<std::mutex> lock(observerMutex_);
    auto it = observerMap_.find(userId);
    if (it == observerMap_.end()) {
        return;
    }
    observerMap_.erase(userId);
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "end.");
}

void PasteboardService::NotifyObservers()
{
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "start.");
    std::lock_guard<std::mutex> lock(observerMutex_);
    for (auto &observers : observerMap_) {
        PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "notify uid : %{public}d.", observers.first);
        for (const auto &observer: *(observers.second)) {
            observer->OnPasteboardChanged();
        }
    }
}

bool PasteboardService::SetPasteboardHistory(int32_t uId, std::string state, std::string timeStamp)
{
    constexpr const size_t DATA_HISTORY_SIZE = 10;
    std::string bundleName;
    if (GetBundleNameByUid(uId, bundleName)) {
        PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "get bundleName success!");
    } else {
        bundleName = "com.pasteboard.default";
    }

    std::string bundleNameState = timeStamp + "  " + bundleName + "    " + state;
    std::shared_ptr<std::string> pBundleNameState = std::make_shared<std::string>(bundleNameState);
    if (dataHistory_.size() == DATA_HISTORY_SIZE) {
        dataHistory_.erase(dataHistory_.begin());
    }
    dataHistory_.push_back(pBundleNameState);
    return true;
}

int PasteboardService::Dump(int fd, const std::vector<std::u16string> &args)
{
    int uid = static_cast<int>(IPCSkeleton::GetCallingUid());
    const int maxUid = 10000;
    if (uid > maxUid) {
        return 0;
    }

    std::vector<std::string> argsStr;
    for (auto item : args) {
        argsStr.emplace_back(Str16ToStr8(item));
    }

    if (PasteboardDumpHelper::GetInstance().Dump(fd, argsStr)) {
        return 0;
    }
    return 0;
}

bool PasteboardService::GetBundleNameByUid(int32_t uid, std::string &bundleName)
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);

    sptr<AppExecFwk::IBundleMgr> iBundleMgr = OHOS::iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (iBundleMgr == nullptr) {
        PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, " permission check failed, cannot get IBundleMgr.");
        return false;
    }
    return iBundleMgr->GetBundleNameForUid(uid, bundleName);
}

std::string PasteboardService::GetTime()
{
    constexpr int USEC_TO_MSEC = 1000;
    time_t time_seconds = time(0);
    if (time_seconds == -1) {
        return FAIL_TO_GET_TIME_STAMP;
    }
    struct tm now_time;
    localtime_r(&time_seconds, &now_time);

    struct timeval tv = { 0, 0 };
    gettimeofday(&tv, nullptr);

    std::string targetTime = std::to_string(now_time.tm_year + 1900) + "-"
                             + std::to_string(now_time.tm_mon + 1) + "-"
                             + std::to_string(now_time.tm_mday) + " "
                             + std::to_string(now_time.tm_hour) + ":"
                             + std::to_string(now_time.tm_min) + ":"
                             + std::to_string(now_time.tm_sec) + "."
                             + std::to_string(tv.tv_usec / USEC_TO_MSEC);
    return targetTime;
}

std::string PasteboardService::DumpHistory() const
{
    std::string result;
    if (!dataHistory_.empty()) {
    result.append("Access history last ten times: ").append("\n");
    for (auto iter = dataHistory_.rbegin(); iter != dataHistory_.rend(); ++iter) {
        result.append("          ")
            .append(**iter)
            .append("\n");
        }
    } else {
        result.append("Access history fail! dataHistory_ no data.").append("\n");
    }
    return result;
}

std::string PasteboardService::DunmpData()
{
    std::string result;
    std::vector<std::string> mimeTypes;
    std::string bundleName;
    if (!clips_.empty()) {
        size_t recordCounts = clips_.rbegin()->second->GetRecordCount();
        mimeTypes = clips_.rbegin()->second->GetMimeTypes();
        if (GetBundleNameByUid(uIdForLastCopy_, bundleName)) {
            PASTEBOARD_HILOGD(PASTEBOARD_MODULE_SERVICE, "get bundleName success!");
            } else {
                bundleName = "com.pasteboard.default";
            }
            result.append("|Owner       :  ")
             .append(bundleName)
             .append("\n")
             .append("|Timestamp   :  ")
             .append(timeForLastCopy_)
             .append("\n")
             .append("|Share Option: ")
             .append(" CrossDevice").append("\n")
             .append("|Record Count:  ")
             .append(std::to_string(recordCounts)).append("\n")
             .append("|Mime types  :  {");
            if (!mimeTypes.empty()) {
                for (size_t i = 0; i < mimeTypes.size(); ++i) {
                    result.append(mimeTypes[i]).append(",");
                }
            }
            result.append("}");
    } else {
        result.append("No copy data.").append("\n");
    }
    return result;
}
} // MiscServices
} // OHOS