/* Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not
 * use this file except in compliance with the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef DOMI_SCOPE_GUARD_H__
#define DOMI_SCOPE_GUARD_H__

#include <functional>
#include <iostream>

// 原文宏名称为ON_SCOPE_EXIT，这里使用了一个较短的名字
// 另外，原文使用ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)
// 这样就无法调用Dismiss方法，所以还是改为由调用者传入变量名称
#define DOMI_MAKE_GUARD(var, callback) hiai::ScopeGuard make_guard_##var(callback)
#define DOMI_DISMISS_GUARD(var) make_guard_##var.Dismiss()

/* Usage: */
/* Acquire Resource 1 */
// MAKE_GUARD([&] { /* Release Resource 1 */ })
/* Acquire Resource 2 */
// MAKE_GUARD([&] { /* Release Resource 2 */ })
namespace hiai {
class ScopeGuard {
public:
    // noncopyable
    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard& operator=(ScopeGuard const&) = delete;

    explicit ScopeGuard(std::function<void()> on_exit_scope) : on_exit_scope_(on_exit_scope), dismissed_(false)
    {
    }

    ~ScopeGuard()
    {
        if (!dismissed_) {
            on_exit_scope_();
        }
    }

    void Dismiss()
    {
        dismissed_ = true;
    }

private:
    std::function<void()> on_exit_scope_;
    bool dismissed_;
};
} // namespace hiai

#endif // DOMI_SCOPE_GUARD_H__
