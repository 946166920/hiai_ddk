/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: static storage
 */

#ifndef HIAI_FRAMEWORK_INFRA_MEM_STATIC_STORAGE_H
#define HIAI_FRAMEWORK_INFRA_MEM_STATIC_STORAGE_H

namespace hiai {
template <typename T>
struct StaticStorage {
    void* Alloc() noexcept
    {
        return static_cast<void*>(&u);
    }

    void Free() noexcept
    {
        Pointer()->~T();
    }

    const T* operator->() const noexcept
    {
        return Pointer();
    }

    T* operator->() noexcept
    {
        return Pointer();
    }

    const T& operator*() const noexcept
    {
        return Ref();
    }

    T& operator*() noexcept
    {
        return Ref();
    }

    const T* Pointer() const noexcept
    {
        return static_cast<const T*>(static_cast<const void*>(&u));
    }

    T* Pointer() noexcept
    {
        return static_cast<T*>(static_cast<void*>(&u));
    }

    const T& Ref() const noexcept
    {
        return *Pointer();
    }

    T& Ref() noexcept
    {
        return *Pointer();
    }

private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type u;
};
}

#endif // HIAI_FRAMEWORK_INFRA_MEM_STATIC_STORAGE_H