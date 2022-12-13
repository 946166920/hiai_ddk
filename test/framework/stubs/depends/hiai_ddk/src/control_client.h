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

#ifndef CONTROL_CLIENT_H
#define CONTROL_CLIENT_H

#include <map>
#include <string>

namespace hiai {

enum ClientKey {
    CLIENT_PERF_MODE,
    CLIENT_ImageBuffer_create,
    CLIENT_GetHandleInfo_From_BufferHandle,
    CLIENT_HIAI_MemBuffer_createV2,
};
class ControlClient {
public:
    static ControlClient& GetInstance();

    void Clear();

    int GetExpectValue(ClientKey key);

    void SetExpectValue(ClientKey key, int expectValue);

private:
    std::map<ClientKey, int> expectValueMap_;
};
} // namespace hiai
#endif // CONTROL_CLIENT_H