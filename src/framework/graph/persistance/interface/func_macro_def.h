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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_FUNC_MACRO_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_FUNC_MACRO_DEF_H
#include <string>
#include <map>
// serialize Type

namespace hiai {
enum SerializeType {
    RESERVED = 0,
    PROTOBUF,
};

// define basic data type, such as int/uint
#define DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(type, name) \
    virtual type name() const = 0; \
    virtual void set_##name(const type value) = 0

// define stardard library data type, such as string
#define DEF_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(type, name) \
    virtual const type& name() const = 0; \
    virtual type* mutable_##name() = 0; \
    virtual void set_##name(const type& value) = 0

// define custom data type
#define DEF_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(type, name) \
    virtual const type* name() const = 0; \
    virtual type* mutable_##name() = 0; \
    virtual void set_##name(const type* value) = 0

// define basic list
#define DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(type, name) \
    virtual void clear_##name() = 0; \
    virtual size_t name##_size() const = 0; \
    virtual type name(size_t index) const = 0; \
    virtual void set_##name(size_t index, type value) = 0; \
    virtual void add_##name(type value) = 0

// define standard list
#define DEF_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(type, name) \
    virtual void clear_##name() = 0; \
    virtual size_t name##_size() const = 0; \
    virtual const type& name(size_t index) const = 0; \
    virtual type* mutable_##name(size_t index) = 0; \
    virtual void set_##name(size_t index, const type& value) = 0; \
    virtual void add_##name(const type& value) = 0

// define custom list
#define DEF_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(type, name) \
    virtual void clear_##name() = 0; \
    virtual size_t name##_size() const = 0; \
    virtual type* mutable_##name(size_t index) = 0; \
    virtual const type* name(size_t index) const = 0; \
    virtual type* add_##name() = 0

// define custom map
#define DEF_PERSISTENCE_CUSTOM_MAP_MEMBER_PURE_FUNC(key_type, value_type, name) \
    virtual void clear_##name() = 0; \
    virtual bool has_##name(const key_type& key) const = 0; \
    virtual void del_##name(const key_type& key) = 0; \
    virtual value_type* mutable_##name(const key_type& key) = 0; \
    virtual const value_type* name(const key_type& key) const = 0; \
    virtual std::map<key_type, value_type*>& mutable_##name() = 0; \
    virtual const std::map<key_type, value_type*>& name() const = 0; \
    virtual value_type* add_##name(const key_type& key) = 0

} // namespace hiai

#endif