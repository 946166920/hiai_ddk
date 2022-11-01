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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_FUNC_MACRO_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_FUNC_MACRO_DEF_H
#include <vector>
#include <map>
#include <memory>

// #include "/usr1/c00574543/Code/hiai_ddk_q/hiai_ddk/out/src/framework/graph/persistance/proto_impl/proto/ge_ir.pb.h"
#include "proto/ge_ir.pb.h"
#include "graph/graph_api_export.h"

// basic member
static const std::string STR_EMPTY  = "";
#define DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(type, name) \
public: \
    type name() const override; \
    void set_##name(const type value) override

#define IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(clazz, proto, type, name, default_value) \
    type clazz::name() const \
    { \
        return proto != nullptr ? (proto)->name() : default_value; \
    } \
    void clazz::set_##name(const type value) \
    { \
        if (proto != nullptr) { \
            (proto)->set_##name(value); \
        } \
    }

// standard member
#define DEF_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(type, name) \
public: \
    const type& name() const override; \
    type* mutable_##name() override; \
    void set_##name(const type& value) override

#define IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(clazz, proto, type, name) \
    const type& clazz::name() const \
    { \
        return proto != nullptr ? (proto)->name() : STR_EMPTY; \
    } \
    type* clazz::mutable_##name() \
    { \
        return proto != nullptr ? (proto)->mutable_##name() : nullptr; \
    } \
    void clazz::set_##name(const type& value) \
    { \
        if (proto != nullptr) { \
            (proto)->set_##name(value); \
        } \
    }

// custom member
#define DEF_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(type, name) \
public: \
    const type* name() const override; \
    type* mutable_##name() override; \
    void set_##name(const type* value) override; \
\
private: \
    mutable type* name##_ = nullptr; \
    void lazy_##name##_init() const

#define IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(clazz, proto, type, impl_type, name) \
    const type* clazz::name() const \
    { \
        lazy_##name##_init(); \
        return name##_; \
    } \
    type* clazz::mutable_##name() \
    { \
        lazy_##name##_init(); \
        return name##_; \
    } \
    void clazz::set_##name(const type* value) \
    { \
        lazy_##name##_init(); \
        name##_->CopyFrom(value); \
    } \
    void clazz::lazy_##name##_init() const \
    { \
        if (proto != nullptr && name##_ == nullptr) { \
            name##_ = new (std::nothrow) impl_type((proto)->mutable_##name()); \
        } \
    }

#define IMPL_PROTO_CUSTOM_MEMBER_FREE(name) \
    if (name##_ != nullptr) { \
        delete name##_; \
        name##_ = nullptr; \
    }

// basic list member
#define DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(type, name) \
public: \
    void clear_##name() override; \
    size_t name##_size() const override; \
    type name(size_t index) const override; \
    void set_##name(size_t index, type value) override; \
    void add_##name(type value) override

#define IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(clazz, proto, type, name, default_value) \
    void clazz::clear_##name() \
    { \
        if (proto != nullptr) { \
            (proto)->clear_##name(); \
        } \
    } \
    size_t clazz::name##_size() const \
    { \
        return proto != nullptr ? (proto)->name##_size() : 0; \
    } \
    type clazz::name(size_t index) const \
    { \
        if (proto == nullptr || (proto)->name##_size() < 0) { \
            return default_value; \
        } \
        if (index < static_cast<size_t>((proto)->name##_size())) { \
            return (proto)->name(index); \
        } \
        return default_value; \
    } \
    void clazz::set_##name(size_t index, type value) \
    { \
        if (proto == nullptr || (proto)->name##_size() < 0) { \
            return; \
        } \
        if (index < static_cast<size_t>((proto)->name##_size())) { \
            (proto)->set_##name(index, value); \
        } \
    } \
    void clazz::add_##name(type value) \
    { \
        if (proto != nullptr) { \
            (proto)->add_##name(value); \
        } \
    }

// standard list member
#define DEF_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(type, name) \
public: \
    void clear_##name() override; \
    size_t name##_size() const override; \
    const type& name(size_t index) const override; \
    type* mutable_##name(size_t index) override; \
    void set_##name(size_t index, const type& value) override; \
    void add_##name(const type& value) override

#define IMPL_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(clazz, proto, type, name) \
    void clazz::clear_##name() \
    { \
        if (proto != nullptr) { \
            (proto)->clear_##name(); \
        } \
    } \
    size_t clazz::name##_size() const \
    { \
        return proto != nullptr ? (proto)->name##_size() : 0; \
    } \
    const type& clazz::name(size_t index) const \
    { \
        return proto != nullptr ? (proto)->name(index) : STR_EMPTY; \
    } \
    type* clazz::mutable_##name(size_t index) \
    { \
        return proto != nullptr ? (proto)->mutable_##name(index) : nullptr; \
    } \
    void clazz::set_##name(size_t index, const type& value) \
    { \
        if (proto != nullptr) { \
            return (proto)->set_##name(index, value); \
        } \
    } \
    void clazz::add_##name(const type& value) \
    { \
        if (proto != nullptr) { \
            (proto)->add_##name(value); \
        } \
    }

// custom list member
#define DEF_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(itf_type, name) \
public: \
    void clear_##name() override; \
    size_t name##_size() const override; \
    itf_type* mutable_##name(size_t index) override; \
    const itf_type* name(size_t index) const override; \
    itf_type* add_##name() override; \
\
private: \
    mutable std::vector<itf_type*> name##_list_; \
    void lazy_##name##_init() const

#define IMPL_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(clazz, proto, itf_type, impl_type, name) \
    void clazz::clear_##name() \
    { \
        for (auto& e : name##_list_) { \
            delete e; \
        } \
        name##_list_.clear(); \
        if (proto != nullptr) { \
            (proto)->clear_##name(); \
        } \
    } \
    size_t clazz::name##_size() const \
    { \
        return proto != nullptr ? (proto)->name##_size() : 0; \
    } \
    itf_type* clazz::mutable_##name(size_t index) \
    { \
        lazy_##name##_init(); \
        if (index >= name##_list_.size()) { \
            return nullptr; \
        } \
        return name##_list_[index]; \
    } \
    const itf_type* clazz::name(size_t index) const \
    { \
        lazy_##name##_init(); \
        if (index >= name##_list_.size()) { \
            return nullptr; \
        } \
        return name##_list_[index]; \
    } \
    itf_type* clazz::add_##name() \
    { \
        lazy_##name##_init(); \
        if (proto != nullptr) { \
            auto ret = name##_list_.emplace(name##_list_.end(), new (std::nothrow) impl_type((proto)->add_##name())); \
            if (ret != name##_list_.end()) { \
                return *ret; \
            } \
        } \
        return nullptr; \
    } \
    void clazz::lazy_##name##_init() const \
    { \
        if (proto != nullptr && name##_list_.size() == 0) { \
            for (int i = 0; i < (proto)->name##_size(); i++) { \
                name##_list_.push_back(new (std::nothrow) impl_type((proto)->mutable_##name(i))); \
            } \
        } \
    }

#define IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(name) \
    for (auto& it : name##_list_) { \
        delete it; \
    } \
    name##_list_.clear()

// custom map member
#define DEF_PROTO_PERSISTENCE_CUSTOM_MAP_MEMBER_PURE_FUNC(key_type, itf_value_type, name) \
public: \
    void clear_##name() override; \
    bool has_##name(const key_type& key) const override; \
    void del_##name(const key_type& key) override; \
    itf_value_type* mutable_##name(const key_type& key) override; \
    const itf_value_type* name(const key_type& key) const override; \
    std::map<key_type, itf_value_type*>& mutable_##name() override; \
    const std::map<key_type, itf_value_type*>& name() const override; \
    itf_value_type* add_##name(const key_type& key) override; \
\
private: \
    mutable std::map<key_type, itf_value_type*> name##_map_

#define IMPL_PROTO_PERSISTENCE_CUSTOM_MAP_MEMBER_PURE_FUNC( \
    clazz, proto, key_type, itf_value_type, impl_value_type, name) \
    void clazz::clear_##name() \
    { \
        for (auto e : name##_map_) { \
            delete e.second; \
        } \
        name##_map_.clear(); \
        if (proto != nullptr) { \
            (proto)->clear(); \
        } \
    } \
    bool clazz::has_##name(const key_type& key) const \
    { \
        if (proto != nullptr) { \
            return (proto)->find(key) != (proto)->end(); \
        } \
        return false; \
    } \
    void clazz::del_##name(const key_type& key) \
    { \
        auto it = name##_map_.find(key); \
        if (it != name##_map_.end()) { \
            delete it->second; \
            name##_map_.erase(it); \
        } \
        if (proto != nullptr) { \
            auto itProto = (proto)->find(key); \
            if (itProto != (proto)->end()) { \
                (proto)->erase(itProto); \
            } \
        } \
    } \
    itf_value_type* clazz::add_##name(const key_type& key) \
    { \
        if (proto != nullptr) { \
            itf_value_type* add = new (std::nothrow) impl_value_type(&((*proto)[key])); \
            auto ret = name##_map_.emplace(key, add); \
            if (ret.second) { \
                return ret.first->second; \
            } else { \
                delete add; \
            } \
        } \
        return nullptr; \
    } \
    itf_value_type* clazz::mutable_##name(const key_type& key) \
    { \
        const itf_value_type* ret = name(key); \
        if (ret != nullptr) { \
            return const_cast<itf_value_type*>(ret); \
        } \
        return add_##name(key); \
    } \
    const itf_value_type* clazz::name(const key_type& key) const \
    { \
        auto it = name##_map_.find(key); \
        if (it != name##_map_.end()) { \
            return it->second; \
        } \
        if (proto != nullptr) { \
            auto itProto = (proto)->find(key); \
            if (itProto != (proto)->end()) { \
                itf_value_type* add = new (std::nothrow) impl_value_type(&itProto->second); \
                auto ret = name##_map_.emplace(key, add); \
                if (ret.second) { \
                    return ret.first->second; \
                } else { \
                    delete add; \
                } \
            } \
        } \
        return nullptr; \
    } \
    std::map<key_type, itf_value_type*>& clazz::mutable_##name() \
    { \
        return const_cast<std::map<key_type, itf_value_type*>&>(name()); \
    } \
    const std::map<key_type, itf_value_type*>& clazz::name() const \
    { \
        if (proto != nullptr) { \
            for (auto it = (proto)->begin(); it != (proto)->end(); it++) { \
                itf_value_type* add = new (std::nothrow) impl_value_type(&it->second); \
                auto ret = name##_map_.emplace(it->first, add); \
                if (!ret.second) { \
                    delete add; \
                } \
            } \
        } \
        return name##_map_; \
    } \

#define IMPL_PROTO_CUSTOM_MAP_MEMBER_FREE(name) \
    for (auto& it : name##_map_) { \
        delete it.second; \
    } \
    name##_map_.clear()

#endif