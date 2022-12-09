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

#include <iostream>
#include "model_manager/model_manager.h"
#include "stub_model_manager_listener.h"

using namespace hiai;

void StubModelManagerListener::OnRunDone(const Context &context, Status result,
    std::vector<std::shared_ptr<INDTensorBuffer>> &outputs)
{
    std::cout << "run in Stub OnRunDone success." << std::endl;
    isInvoked_ = true;
}

void StubModelManagerListener::OnServiceDied()
{
    std::cout << "run in Stub OnServiceDied success." << std::endl;
    isInvoked_ = true;
}

bool StubModelManagerListener::IsListenerInvoked()
{
    return isInvoked_;
}
