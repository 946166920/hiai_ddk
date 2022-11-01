/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: om wrapper
 *
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
