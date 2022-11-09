/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: stub model manager listener
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
