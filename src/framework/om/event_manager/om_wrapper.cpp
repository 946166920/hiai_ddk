/**
 * Copyright 2019-2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "om_wrapper.h"

namespace hiai {
int EventListener::Register()
{
    eventListener_ = AI_OM_EventListener_Create(HandleFn, this);
    if (eventListener_ == nullptr) {
        return -1;
    }
    if (AI_OM_RegisterListener(eventListener_) != 0) {
        return -1;
    }
    return 0;
}

EventListener::~EventListener()
{
    if (eventListener_) {
        AI_OM_EventListener_Destroy(eventListener_);
    }
}

void EventListener::HandleFn(const AI_OM_EventMsg* event, void* userData)
{
    auto that = reinterpret_cast<EventListener*>(userData);
    if (that != nullptr && event != nullptr) {
        that->Handle(*event);
    }
}
}; // namespace hiai
