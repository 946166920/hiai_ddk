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

// inc/framework
#include "framework/graph/core/cgraph/graph_listener.h"
#include "framework/graph/core/cgraph/graph_notifier.h"

namespace ge {
AutoGraphListener::AutoGraphListener(GraphNotifier& notifier, GraphListener& listener)
    : notifier_(notifier), listener_(listener)
{
    notifier_.Register(listener_);
}

AutoGraphListener::~AutoGraphListener()
{
    notifier_.Unregister(listener_);
}
} // namespace ge