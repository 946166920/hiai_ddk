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

#include "control_client.h"

namespace hiai {
ControlClient& ControlClient::GetInstance()
{
    static ControlClient instance;
    return instance;
}

void ControlClient::Clear()
{
    expectValueMap_.clear();
}

int ControlClient::GetExpectValue(ClientKey key)
{
    if (expectValueMap_.find(key) != expectValueMap_.end()) {
        return expectValueMap_[key];
    }
    return -1;
}

void ControlClient::SetExpectValue(ClientKey key, int expectValue)
{
    expectValueMap_[key] = expectValue;
}
} // namespace hiai