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

#ifndef STUB_LOAD_MODELS_H
#define STUB_LOAD_MODELS_H
#include "compatible/HiAiModelManagerService.h"
#include "compatible/HiAiModelManagerType.h"
#include "model_manager.h"
#include "compatible/AiModelBuilder.h"

using namespace std;
using namespace hiai;

class StubLoadModels {
public:
    void LoadModelsByV1();

    void UnloadModels();

    shared_ptr<AiModelMngerClient> GetClient();

private:
    shared_ptr<AiModelMngerClient> client {nullptr};
};
#endif