/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: om warraper
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
