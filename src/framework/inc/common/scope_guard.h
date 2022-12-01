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
