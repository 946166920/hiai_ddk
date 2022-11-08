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

#ifndef DOMI_COMMON_TYPES_H_
#define DOMI_COMMON_TYPES_H_
#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <array>
#include <memory>
#include <algorithm>
#include <climits>
#include "framework/common/fmk_types.h"
#if (OS_TYPE == 0) //lint !e553
#include <linux/limits.h>
#endif
#include "op_types.h"

#if !defined(__ANDROID__) && !defined(ANDROID)
#define DOMI_DYNAMIC_CAST static_cast
#define DOMI_DYNAMIC_POINTER_CAST std::static_pointer_cast
#else
#define DOMI_DYNAMIC_CAST static_cast
#define DOMI_DYNAMIC_POINTER_CAST std::static_pointer_cast
#endif

#if defined(HOST_VISIBILITY) || defined(DEV_VISIBILITY)
#define GE_API_VISIBILITY __attribute__((visibility("default")))
#else
#ifdef _MSC_VER
#ifdef LIBAI_FMK_OP_DEF_EXPORTS
#define GE_API_VISIBILITY __declspec(dllexport)
#else
#define GE_API_VISIBILITY __declspec(dllimport)
#endif
#else
#define GE_API_VISIBILITY
#endif
#endif

namespace hiai {
/*
 * @brief 模型名字长度
 */
const int MODEL_NAME_LENGTH = 32;

/*
 * @brief 用户自定义信息长度
 */
const int USER_DEFINE_INFO_LENGTH = 32;

/*
 * @brief 模型文件签名长度
 */
const int MODEL_FILE_CHECKSUM_LENGTH = 64;

/*
 * @brief 模型文件头保留字段长度
 */
const int MODEL_FILE_RESERVED_LENGTH = 68;

// 当前模型partition数目
const int WEIGHT_MERGED_IR_MODEL_PARTITION_SIZE = 2;

#define SIZE_OF_MODEL_PARTITION_TABLE(table) (sizeof(ModelPartitionTable) + sizeof(ModelPartitionMemInfo) * (table).num)


GE_API_VISIBILITY extern const char* const MODEL_ATTR_TASKS;
GE_API_VISIBILITY extern const char* const MODEL_ATTR_TASK_GEN_BASE_ADDR;
GE_API_VISIBILITY extern const char* const MODEL_ATTR_TASK_GEN_WEIGHT_ADDR;
GE_API_VISIBILITY extern const char* const MODEL_ATTR_FUSION_MODEL_DEF;

GE_API_VISIBILITY extern const int MODEL_MAX_SIZE; // Max size of 2 GB minus 1 byte.
GE_API_VISIBILITY extern const uint64_t FILE_HEADER_MAX_SIZE; // Max size of 3 GB.

#if !defined(__ANDROID__) && !defined(ANDROID)
GE_API_VISIBILITY extern const uint64_t ALLOC_MEMORY_MAX_SIZE; // Max size of 8 GB.
#else
GE_API_VISIBILITY extern const uint64_t ALLOC_MEMORY_MAX_SIZE; // Max size of 512M.
#endif


/*
 * @brief 模型文件加密类型
 */
enum ModelEncryptType {
    UNENCRYPTED, // 不加密
    ENCRYPTED // 加密
};

/*
 * @brief 校验签名
 */
enum ModelCheckType {
    CHECK, // 校验签名
    UNCHECK // 不校验
};

/*
 * @brief 模型文件魔数
 */
GE_API_VISIBILITY const uint32_t MODEL_FILE_MAGIC_NUM = 0x444F4D49; /* 魔数DOMI */

/*
 * @brief 模型头长度
 */
GE_API_VISIBILITY extern const uint32_t MODEL_FILE_HEAD_LEN;

/**
 * @ingroup domi_omg
 * @brief INPUT节点类型
 */
GE_API_VISIBILITY extern const char* const INPUT_TYPE;

/*
 * @brief 平台版本长度
 */
const int PLATFORM_VERSION_LEN = 20;

GE_API_VISIBILITY bool IsOpTypeInMainGraph(const std::string& type);

/*
 * @brief 模型文件文件头定义
 */
struct ModelFileHeader {
    uint32_t magic = MODEL_FILE_MAGIC_NUM; /* 魔数DOMI */
    uint32_t headsize = MODEL_FILE_HEAD_LEN; /* 模型头长度，固定256 */
    uint32_t version = MODEL_VERSION; /* *< 版本1.0 */
    uint8_t checksum[MODEL_FILE_CHECKSUM_LENGTH] = {0}; /* 签名 */
    uint32_t length = 0; /* 密文长度, 非加密模型为明文长度 */
    uint8_t isEncrypt = ModelEncryptType::UNENCRYPTED; /* 是否加密 0:不加密，1:加密 */
    uint8_t isChecksum = ModelCheckType::CHECK; /* *< 是否检查checksum */
    uint8_t modeltype = 0; /* 0：IR模型 1：OM标准模型 2: OM Tiny模型 3:异构分段编译模型 */
    uint8_t genmode = 0; /*  0：离线生成 1：在线生成 */
    uint8_t name[MODEL_NAME_LENGTH] = {0}; /* 模型名字32长度 */
    uint32_t ops = 0; /* 算力，单位Kops */
    uint8_t userdefineinfo[USER_DEFINE_INFO_LENGTH] = {0}; /* 用户自定义信息 32长度 */
    uint32_t omIrVersion = 0;
    uint8_t platformVersion[PLATFORM_VERSION_LEN] = {0};
    uint8_t platformType = {0};
    uint32_t customDataLen = 0;
    uint32_t multiShapeGears = 0;
    uint8_t reserved[MODEL_FILE_RESERVED_LENGTH] = {0}; /* 保留字段 68 长度 */
};

enum ModelType {
    IR_GRAPH_MODEL = 0, /* 老模型 标准OM模型（异构前） */
    OM_STANDARD_MODEL, /* 之前没使用地方 */
    IGS_MODEL, /* 异构前的TINY模型 */
    STANDARD_IR_GRAPH_MODEL, /* 异构之后的标准IR模型 */
    HCS_PARTITION_MODEL, /* 异构之后的分段D模型 */
    ISPNN_MODEL, /* ISPNN模型 */
    SPECIAL_3RD_MODEL,
    IR_API_GRAPH_MODEL, /* IR API构造经序列化后的模型, 无模型头 */
    UTINY_MODEL, /* Pha模型，枚举值修改后，需要修改sensorhub侧代码 */
    MULTI_SHAPE_MODEL
};

// 当前模型partition数目
enum ModelPartitionType {
    MODEL_DEF = 0,
    WEIGHTS_DATA,
    TASK_INFO,
    TASK_GRAPH,
    SIGNATURE_INFO, // 签名字段,用来做模型中的部分数据做完整性校验
    MODEL_CONFIG,
    AIPP_CUSTOM_INFO,
    ENCRYPT_INFO
};

struct ModelPartitionMemInfo {
    ModelPartitionType type;
    uint32_t memOffset;
    uint32_t memSize;
};

struct ModelPartitionTable {
    uint32_t num;
    ModelPartitionMemInfo partition[0];
};

#pragma pack(1) /* 单字节对齐 */
/*
 * @ingroup domi_ome
 * @brief DUMP file struct
 */
struct FileHeader {
    int32_t Version; /* 版本号 */
    int32_t OutputOffset; /* output偏移地址  */
    char Reserved[24] = {0}; /* 保留 24 */
};

struct BasicInfo {
    struct FileHeader header; /* 文件头 */
    int32_t streamId; /* stread id */
    uint64_t startTime; /* 开始时间 */
    uint64_t endTime; /* 结束时间 */
    uint32_t inputSize; /* 输入内存大小 */
    uint32_t outputSize; /* 输出内存大小 */
    uint32_t weightSize; /* 权值内存大小 */
    uint32_t workspaceSize; /* workspace */
    uint32_t totalSize; /* 内存总大小  */
};
#pragma pack() /* * 取消单字节对齐 * */
}; // namespace hiai
#endif // DOMI_OMG_COMMON_TYPES_H_
