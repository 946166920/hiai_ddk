/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: common fmk type
 *
 */

#ifndef DOMI_COMMON_FMK_TYPES_H__
#define DOMI_COMMON_FMK_TYPES_H__

#include <string>

namespace ge {
// 业务类型: 通用业务，Mini业务， Igs业务， Isp-NN业务
enum ServiceType {
    COMMON_SERVICE = 0, // 通用业务
    DEDICATED_MINI_SERVICE, // Mini业务，保持兼容性
    DEDICATED_IGS_SERVICE, // Igs业务
    DEDICATED_ISPNN_SERVICE, // Isp-NN业务
    SERVICE_TYPE_RESERVED
};
} // namespace ge

namespace hiai {
#ifndef USE_STATIC_LIB
#ifdef HOST_VISIBILITY
#define FMK_FUNC_HOST_VISIBILITY __attribute__((visibility("default")))
#else
#define FMK_FUNC_HOST_VISIBILITY
#endif
#ifdef DEV_VISIBILITY
#define FMK_FUNC_DEV_VISIBILITY __attribute__((visibility("default")))
#else
#define FMK_FUNC_DEV_VISIBILITY
#endif
#else
#define FMK_FUNC_HOST_VISIBILITY
#define FMK_FUNC_DEV_VISIBILITY
#endif

/*
 * @brief 模型数据结构定义，由调用方传入
 */
struct ModelData {
    void* modelData = nullptr; /* 模型二进制数据首地址 */
    uint32_t modelLen = 0; /* 模型二进制数据长度 */
    int32_t priority = 0; /* 模型优先级 */
    std::string key = ""; /* 模型解密密钥的路径，如果为非加密模型，则传入空字符串 */
};

struct TaskGenInfo {
    uint32_t mode = 0; // 模式（是否融合等）
    uint32_t size = 0; // task序列化数据size
    uint8_t* serializedTasks = nullptr; // task序列化数据
    uint64_t dataBase = 0; // task生成时的数据基址
    uint64_t weightBase = 0; // task生成时的权值基址
    bool isOnlineOmg = false; // 是否为在线编译流程
};

enum class MemoryType { UNDEFINED = -1, DDR = 1, ION, SPM, SYSCACHE, SVM, IONISO };

struct MemInfo {
    void* addr;
    uint64_t size;
    MemoryType memType;
};
} // namespace hiai
#endif // FMK_TYPES_H__
