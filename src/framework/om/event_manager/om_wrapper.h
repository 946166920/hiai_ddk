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

#ifndef OM_WRAPPER_H
#define OM_WRAPPER_H

#include "infra/om/event_manager/ai_om.h"

namespace hiai {
class EventListener {
public:
    virtual int Register();

    virtual ~EventListener();

    virtual void Handle(const AI_OM_EventMsg& event) = 0;

    static void HandleFn(const AI_OM_EventMsg* event, void* userData);

private:
    AI_OM_EventListener* eventListener_ = nullptr;
};
}; // namespace hiai

#endif
