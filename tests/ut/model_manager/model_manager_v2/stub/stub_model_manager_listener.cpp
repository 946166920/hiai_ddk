/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: stub model manager listener
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
