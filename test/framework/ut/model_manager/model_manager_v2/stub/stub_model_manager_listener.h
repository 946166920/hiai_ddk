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

#ifndef STUB_MODEL_MANAGER_LISTENER
#define STUB_MODEL_MANAGER_LISTENER

#include "model_manager/model_manager.h"
#include "model/built_model_aipp.h"
#include "tensor/image_config_tensor_util.h"
#include "tensor/image_tensor_buffer.h"

using namespace hiai;

class StubModelManagerListener : public IModelManagerListener {
public:
    void OnRunDone(const Context& context, Status result, std::vector<std::shared_ptr<INDTensorBuffer>>& outputs);
    void OnServiceDied();
    bool IsListenerInvoked();
private:
    bool isInvoked_ {false};
};

#endif
