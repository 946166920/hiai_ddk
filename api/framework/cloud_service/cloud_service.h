/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: model manager client implementation
 *
 */

#ifndef CLOUD_SERVICE_NEW_H
#define CLOUD_SERVICE_NEW_H
#include <atomic>

#ifdef HIAI_CLOUD_SERVICE_API_VISIABLE
#ifdef _MSC_VER
#define HIAI_CLOUD_SERVICE_API_EXPORT __declspec(dllexport)
#else
#define HIAI_CLOUD_SERVICE_API_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define HIAI_CLOUD_SERVICE_API_EXPORT
#endif

namespace hiai {
class HIAI_CLOUD_SERVICE_API_EXPORT CloudService {
public:
    static bool Enable(void* env, void* context);
    static const char* GetComputeCapabilityVersion();

private:
    CloudService() = default;
    ~CloudService() = default;
    static std::atomic<bool> isSoLoaded_;
};
} // end namespace hiai
#endif
