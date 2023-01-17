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

#ifndef GE_GRAH_PERSISTENCE_PROTO_WRAPPER_DEF_H
#define GE_GRAH_PERSISTENCE_PROTO_WRAPPER_DEF_H

namespace hiai {
template <typename T> class ProtoWrapper {
protected:
    inline T& GetProto()
    {
        return proto_;
    }

protected:
    T proto_;
};
} // namespace hiai
#endif