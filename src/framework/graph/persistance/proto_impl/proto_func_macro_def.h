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

#include "proto/ge_ir.pb.h"

// basic member
#define DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(type, name) \
public: \
    type name() const override; \
    void set_##name(const type value) override

#define IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(clazz, proto, type, name, default_value) \
    type clazz::name() const \
    { \
        return proto.name(); \
    } \
    void clazz::set_##name(const type value) \
    { \
        proto.set_##name(value); \
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
        return proto.name(); \
    } \
    type* clazz::mutable_##name() \
    { \
        return proto.mutable_##name(); \
    } \
    void clazz::set_##name(const type& value) \
    { \
        proto.set_##name(value); \
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
        if (name##_ == nullptr) { \
            name##_ = new (std::nothrow) impl_type(*proto.mutable_##name()); \
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
        proto.clear_##name(); \
    } \
    size_t clazz::name##_size() const \
    { \
        return proto.name##_size(); \
    } \
    type clazz::name(size_t index) const \
    { \
        if (index < name##_size()) { \
            return proto.name(index); \
        } \
        return default_value; \
    } \
    void clazz::set_##name(size_t index, type value) \
    { \
        proto.set_##name(index, value); \
    } \
    void clazz::add_##name(type value) \
    { \
        proto.add_##name(value); \
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
        proto.clear_##name(); \
    } \
    size_t clazz::name##_size() const \
    { \
        return proto.name##_size(); \
    } \
    const type& clazz::name(size_t index) const \
    { \
        return proto.name(index); \
    } \
    type* clazz::mutable_##name(size_t index) \
    { \
        return proto.mutable_##name(index); \
    } \
    void clazz::set_##name(size_t index, const type& value) \
    { \
        return proto.set_##name(index, value); \
    } \
    void clazz::add_##name(const type& value) \
    { \
        proto.add_##name(value); \
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
        proto.clear_##name(); \
    } \
    size_t clazz::name##_size() const \
    { \
        return proto.name##_size(); \
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
        auto add = new (std::nothrow) impl_type(*proto.add_##name()); \
        if (add != nullptr) { \
            name##_list_.emplace_back(add); \
        } \
        return add; \
    } \
    void clazz::lazy_##name##_init() const \
    { \
        if (name##_list_.size() == 0) { \
            for (int i = 0; i < proto.name##_size(); i++) { \
                auto add = new (std::nothrow) impl_type(*proto.mutable_##name(i)); \
                if (add != nullptr) { \
                    name##_list_.emplace_back(add); \
                } \
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
public: \
    mutable std::map<key_type, itf_value_type*> name##_map_

#define IMPL_PROTO_PERSISTENCE_CUSTOM_MAP_MEMBER_PURE_FUNC( \
    clazz, proto, key_type, itf_value_type, impl_value_type, name) \
    void clazz::clear_##name() \
    { \
        for (auto e : name##_map_) { \
            delete e.second; \
        } \
        name##_map_.clear(); \
        proto.clear(); \
    } \
    bool clazz::has_##name(const key_type& key) const \
    { \
        return proto.find(key) != proto.end(); \
    } \
    void clazz::del_##name(const key_type& key) \
    { \
        auto it = name##_map_.find(key); \
        if (it != name##_map_.end()) { \
            delete it->second; \
            name##_map_.erase(it); \
        } \
        auto iter = proto.find(key); \
        if (iter != proto.end()) { \
            proto.erase(iter); \
        } \
    } \
    itf_value_type* clazz::add_##name(const key_type& key) \
    { \
        auto add = new (std::nothrow) impl_value_type(proto[key]); \
        if (add != nullptr) { \
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
        auto it = name##_map_.find(key); \
        if (it != name##_map_.end()) { \
            return it->second; \
        } \
        auto iter = proto.find(key); \
        if (iter != proto.end()) { \
            auto add = new (std::nothrow) impl_value_type(iter->second); \
            if (add != nullptr) { \
                auto ret = name##_map_.emplace(key, add); \
                if (ret.second) { \
                    return ret.first->second; \
                } else { \
                    delete add; \
                } \
            } \
        } \
        return add_##name(key); \
    } \
    const itf_value_type* clazz::name(const key_type& key) const \
    { \
        auto it = name##_map_.find(key); \
        if (it != name##_map_.end()) { \
            return it->second; \
        } \
        auto iter = proto.find(key); \
        if (iter != proto.end()) { \
            auto add = new (std::nothrow) impl_value_type(iter->second); \
            if (add != nullptr) { \
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
        for (auto it = proto.begin(); it != proto.end(); it++) { \
            auto add = new (std::nothrow) impl_value_type(it->second); \
            if (add != nullptr) { \
                auto ret = name##_map_.emplace(it->first, add); \
                if (!ret.second) { \
                    delete add; \
                } \
            } \
        } \
        return name##_map_; \
    } \
    const std::map<key_type, itf_value_type*>& clazz::name() const \
    { \
        for (auto it = proto.begin(); it != proto.end(); it++) { \
            auto add = new (std::nothrow) impl_value_type(it->second); \
            if (add != nullptr) { \
                auto ret = name##_map_.emplace(it->first, add); \
                if (!ret.second) { \
                    delete add; \
                } \
            } \
        } \
        return name##_map_; \
    }

#define IMPL_PROTO_CUSTOM_MAP_MEMBER_FREE(name) \
    for (auto& it : name##_map_) { \
        delete it.second; \
    } \
    name##_map_.clear()

#endif