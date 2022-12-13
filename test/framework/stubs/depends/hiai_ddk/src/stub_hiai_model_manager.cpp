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

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cutils/native_handle.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <climits>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include "securec.h"
#include "hiai_nd_tensor_buffer.h"
#include "control_client.h"

using namespace std;
using namespace hiai;

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HIAI_DEVPERF_UNSET = 0,
    HIAI_DEVPREF_LOW,
    HIAI_DEVPREF_NORMAL,
    HIAI_DEVPREF_HIGH,
} HIAI_DevPerf;

typedef struct {
    int inputCnt;
    int outputCnt;
    HIAI_NDTensorDesc** inputShape;
    HIAI_NDTensorDesc** outputShape;
} HIAI_ModelNDTensorInfo;

typedef enum {
    HIAI_IO_INPUT = 0,
    HIAI_IO_OUTPUT = 1,
    HIAI_IO_INVALID = 255,
} HIAI_IO_TYPE;

typedef enum { HIAI_MODELTYPE_ONLINE = 0, HIAI_MODELTYPE_OFFLINE } HIAI_ModelType;

typedef enum {
    HIAI_FRAMEWORK_NONE = 0,
    HIAI_FRAMEWORK_TENSORFLOW,
    HIAI_FRAMEWORK_KALDI,
    HIAI_FRAMEWORK_CAFFE,
    HIAI_FRAMEWORK_TENSORFLOW_8BIT,
    HIAI_FRAMEWORK_CAFFE_8BIT,
    HIAI_FRAMEWORK_OFFLINE,
    HIAI_FRAMEWORK_IR,
    HIAI_FRAMEWORK_INVALID,
} HIAI_Framework;

enum NpuCoreAippInputFormat {
    CCE_YUV420SP_U8 = 1,
    CCE_XRGB8888_U8,
    CCE_NC1HWC0DI_FP16,
    CCE_NC1HWC0DI_S8,
    CCE_RGB888_U8,
    CCE_ARGB8888_U8,
    CCE_YUYV_U8,
    CCE_YUV422SP_U8,
    CCE_AYUV444_U8,
    CCE_YUV400_U8,
    CCE_RESERVED
};

enum AiTensorImage_Format {
    AiTensorImage_YUV420SP_U8 = 0,
    AiTensorImage_XRGB8888_U8,
    AiTensorImage_YUV400_U8,
    AiTensorImage_ARGB8888_U8,
    AiTensorImage_YUYV_U8,
    AiTensorImage_YUV422SP_U8,
    AiTensorImage_AYUV444_U8,
    AiTensorImage_RGB888_U8,
    AiTensorImage_BGR888_U8,
    AiTensorImage_YUV444SP_U8,
    AiTensorImage_YVU444SP_U8,
    AiTensorImage_INVALID = 255,
};

typedef struct {
    int number;
    int channel;
    int height;
    int width;
    HIAI_DataType dataType; /* optional */
} HIAI_TensorDescription;

typedef struct {
    int input_cnt;
    int output_cnt;
    int* input_shape;
    int* output_shape;
} HIAI_ModelTensorInfo;

typedef struct {
    const char* modelNetName {nullptr};
    const char* modelNetPath {nullptr};
    bool isModelNetEncrypted {false};
    const char* modelNetKey {nullptr};
    const char* modelNetParamPath {nullptr};
    bool isModelNetParamEncrypted {false};
    const char* modelNetParamKey {nullptr};
    HIAI_ModelType modelType {HIAI_MODELTYPE_ONLINE};
    HIAI_Framework frameworkType {HIAI_FRAMEWORK_NONE};
    HIAI_DevPerf perf {HIAI_DEVPERF_UNSET};
} HIAI_ModelDescription;

struct HIAI_MemBuffer {
    unsigned int size;
    void* data;
};

typedef enum {
    HIAI_PRIORITY_HIGH_PLATFORM1 = 1,
    HIAI_PRIORITY_HIGH_PLATFORM2,
    HIAI_PRIORITY_HIGHEST,
    HIAI_PRIORITY_HIGHER,
    HIAI_PRIORITY_HIGH,
    HIAI_PRIORITY_MIDDLE,
    HIAI_PRIORITY_LOW,
} HIAI_ModelPriority;

struct HIAI_ModelBuffer {
    const char* name;
    const char* path;
    HIAI_DevPerf perf;
    uint32_t size;
    void* impl; /* DON'T MODIFY */
};

struct HIAI_TensorBuffer {
    HIAI_TensorDescription desc;
    int size;
    void* data;
    void* impl; /* DON'T MODIFY */
    const char* name;
};

struct HIAI_NDTensorDesc {
    int32_t* dims;
    size_t dimNum;
    HIAI_DataType dataType;
    HIAI_Format format;
};

struct HIAI_NDTensorBuffer {
    HIAI_NDTensorDesc* desc;
    size_t size;
    void* data;
    void* handle;
    bool owner;
};

struct HIAI_ModelManager {
    void* impl; /* DON'T MODIFY */
    bool sync;
};

namespace hiai {
struct PrivateHandleInfo {
    int fd;
    int size;
    int offset;
    int format;
    int w;
    int h;
    int strideW;
    int strideH;
};
} // namespace hiai

typedef struct HIAI_ModelManagerListener_struct {
    void (*onLoadDone)(void* userdata, int taskStamp) = nullptr;
    void (*onRunDone)(void* userdata, int taskStamp) = nullptr;
    void (*onUnloadDone)(void* userdata, int taskStamp) = nullptr;
    void (*onTimeout)(void* userdata, int taskStamp) = nullptr;
    void (*onError)(void* userdata, int taskStamp, int errCode) = nullptr;
    void (*onServiceDied)(void* userdata) = nullptr;
    void* userdata {nullptr};
} HIAI_ModelManagerListener;

struct HIAI_TensorDescriptionV2 {
    int number;
    int channel;
    int height;
    int width;
    HIAI_DataType dataType;
    int size;
    const char* name;
};

struct HIAI_ModelTensorInfoV2 {
    int inputCnt;
    int outputCnt;
    HIAI_TensorDescriptionV2* inputShape;
    HIAI_TensorDescriptionV2* outputShape;
};

#define HIAI_TENSOR_DESCRIPTION_INIT \
    { \
        0, 0, 0, 0, HIAI_DATATYPE_FLOAT32 \
    }

#define HIAI_TENSOR_DESCRIPTION_V2_INIT \
    { \
        1, 1, 1, 1, HIAI_DATATYPE_FLOAT32, 1, "test" \
    }

#define HIAI_NDTENSOR_DESCRIPTION_INIT \
    { \
        nullptr, 0, HIAI_DATATYPE_FLOAT32 \
    }
// for AIPP once the struct in domi has been updated, this struct must be updated too
struct HIAI_TensorAippBatchPara {
    int8_t cropSwitch; // crop switch
    int8_t scfSwitch; // resize switch
    int8_t paddingSwitch; // 0: unable padding,
    // rotate switch，0: non-ratate,1: ratate 90° clockwise,2: ratate 180° clockwise,3: ratate 270° clockwise
    int8_t rotateSwitch;
    int8_t reserve[4];
    int32_t cropParas[4]; // cropStartPosW, cropStartPosH, cropSizeW, cropSizeH
    int32_t srfParas[4]; // scfInputSizeW, scfInputSizeH, scfOutputSizeW, scfOutputSizeH;
    int32_t paddingParas[4]; // paddingSizeTop, paddingSizeBottom, paddingSizeLeft, paddingSizeRight
    // dtcPixelMeanChn0, dtcPixelMeanChn1, dtcPixelMeanChn2, dtcPixelMeanChn3
    // dtcPixelMinChn0, dtcPixelMinChn1, dtcPixelMinChn2, dtcPixelMinChn3,
    // dtcPixelVarReciChn0, dtcPixelVarReciChn1, dtcPixelVarReciChn2, dtcPixelVarReciChn3
    int16_t dtcParas[12];
    uint16_t paddingValueChn0; // padding value of channle 0
    uint16_t paddingValueChn1; // padding value of channle 1
    uint16_t paddingValueChn2; // padding value of channle 2
    uint16_t paddingValueChn3; // padding value of channle 3
    int8_t reserve1[8]; // 32B assign, for ub copy
};

struct HIAI_TensorAippCommPara {
    uint8_t inputFormat; // input format：YUV420SP_U8/XRGB8888_U8/RGB888_U8
    int8_t cscSwitch; // csc switch
    int8_t rbuvSwapSwitch; // rb/ub swap switch
    int8_t axSwapSwitch; // RGBA->ARGB, YUVA->AYUV swap switch
    int8_t batchNum; // batch parameter number
    int8_t reserve1[3];
    int32_t srcImageSizeW; // source image width
    int32_t srcImageSizeH; // source image height
    // cscMatrixR0C0; cscMatrixR0C1; cscMatrixR0C2; cscMatrixR1C0; cscMatrixR1C1;
    // cscMatrixR1C2; cscMatrixR2C0; cscMatrixR2C1; cscMatrixR2C2;
    int16_t cscMatrix[9];
    int16_t reserve2[3];
    // cscOutputBiasR0,cscOutputBiasR1,cscOutputBiasR2,cscInputBiasR0,cscInputBiasR1,cscInputBiasR2;
    uint8_t cscBiasParas[6];
    uint8_t reserve3[2];
    int8_t reserve4[16];
};

struct HIAI_TensorAippPara {
    uint32_t inputIndex;
    uint32_t inputEdgeIndex;
    void* parasBuff;
    uint32_t size;
    void* impl; /* DON'T MODIFY */
};

static int32_t g_timeStamp = 0;
std::map<HIAI_ModelManager*, HIAI_ModelManagerListener*> g_ModelManagerListenerMap;
static size_t g_dataTypeElementSize[HIAI_DATATYPE_SIZE] = {sizeof(uint8_t), sizeof(float), sizeof(float) / 2,
    sizeof(int32_t), sizeof(int8_t), sizeof(int16_t), sizeof(char), sizeof(int64_t), sizeof(uint32_t), sizeof(double)};

void onLoadDone(HIAI_ModelManager* manager, void* userData, int32_t taskStamp)
{
    map<HIAI_ModelManager*, HIAI_ModelManagerListener*>::iterator it = g_ModelManagerListenerMap.find(manager);
    if (it != g_ModelManagerListenerMap.end()) {
        if (it->second != nullptr) {
            std::cout << "Begin call onLoadDone" << std::endl;
            it->second->onLoadDone(userData, taskStamp);
        }
    }
}

void onRunDone(HIAI_ModelManager* manager, void* userData, int32_t taskStamp)
{
    this_thread::sleep_for(chrono::milliseconds(50));
    map<HIAI_ModelManager*, HIAI_ModelManagerListener*>::iterator it = g_ModelManagerListenerMap.find(manager);
    if (it != g_ModelManagerListenerMap.end()) {
        if (it->second != nullptr) {
            std::cout << "Begin call onRunDone" << std::endl;
            it->second->onRunDone(userData, taskStamp);
        }
    }
}

void onUnloadDone(HIAI_ModelManager* manager, void* userData, int32_t taskStamp)
{
    this_thread::sleep_for(chrono::milliseconds(50));
    map<HIAI_ModelManager*, HIAI_ModelManagerListener*>::iterator it = g_ModelManagerListenerMap.find(manager);
    if (it != g_ModelManagerListenerMap.end()) {
        if (it->second != nullptr) {
            std::cout << "Begin call onUnloadDone" << std::endl;
            it->second->onUnloadDone(userData, taskStamp);
        }
    }
}

void onTimeout(HIAI_ModelManager* manager, void* userData, int32_t taskStamp)
{
    this_thread::sleep_for(chrono::milliseconds(100));
    map<HIAI_ModelManager*, HIAI_ModelManagerListener*>::iterator it = g_ModelManagerListenerMap.find(manager);
    if (it != g_ModelManagerListenerMap.end()) {
        if (it->second != nullptr) {
            std::cout << "Begin call onTimeout" << std::endl;
            it->second->onTimeout(userData, taskStamp);
        }
    }
}

void onError(HIAI_ModelManager* manager, void* userData, int taskStamp, int errCode)
{
    this_thread::sleep_for(chrono::milliseconds(100));
    map<HIAI_ModelManager*, HIAI_ModelManagerListener*>::iterator it = g_ModelManagerListenerMap.find(manager);
    if (it != g_ModelManagerListenerMap.end()) {
        if (it->second != nullptr) {
            std::cout << "Begin call onError" << std::endl;
            it->second->onError(userData, taskStamp, errCode);
        }
    }
}

void onServiceDied(HIAI_ModelManager* manager, void* userData)
{
    this_thread::sleep_for(chrono::milliseconds(100));
    map<HIAI_ModelManager*, HIAI_ModelManagerListener*>::iterator it = g_ModelManagerListenerMap.find(manager);
    if (it != g_ModelManagerListenerMap.end()) {
        if (it->second != nullptr) {
            std::cout << "Begin call onServiceDied" << std::endl;
            it->second->onServiceDied(userData);
        }
    }
}

// for HIAI_ModelBuffer
HIAI_ModelBuffer* HIAI_ModelBuffer_create_from_file(const char* name, const char* path, HIAI_DevPerf perf)
{
    auto buffer = static_cast<HIAI_ModelBuffer*>(malloc(sizeof(HIAI_ModelBuffer)));
    return buffer;
}

HIAI_ModelBuffer* HIAI_ModelBuffer_create_from_buffer(const char* name, void* modelBuf, int size, HIAI_DevPerf perf)
{
    auto buffer = static_cast<HIAI_ModelBuffer*>(malloc(sizeof(HIAI_ModelBuffer)));
    buffer->perf = perf;
    return buffer;
}

void HIAI_ModelBuffer_destroy(HIAI_ModelBuffer* buffer)
{
    if (buffer != nullptr) {
        free(buffer);
        buffer = nullptr;
    }
}

const char* HIAI_ModelBuffer_getName(HIAI_ModelBuffer* b)
{
    return b ? b->name : nullptr;
}

const char* HIAI_ModelBuffer_getPath(HIAI_ModelBuffer* b)
{
    return b ? b->path : nullptr;
}

HIAI_DevPerf HIAI_ModelBuffer_getPerf(HIAI_ModelBuffer* b)
{
    return b ? b->perf : HIAI_DEVPERF_UNSET;
}

int HIAI_ModelBuffer_getSize(HIAI_ModelBuffer* b)
{
    return b ? static_cast<int>(b->size) : 0;
}

HIAI_TensorBuffer* HIAI_TensorBuffer_createFromTensorDesc(HIAI_TensorDescription* tensor)
{
    if (tensor == nullptr) {
        return nullptr;
    }
    uint32_t size = tensor->number * tensor->channel * tensor->height * tensor->width * sizeof(float);
    if (size == 0) {
        return nullptr;
    }
    auto* buffer = static_cast<HIAI_TensorBuffer*>(malloc(sizeof(HIAI_TensorBuffer)));
    if (buffer == nullptr) {
        return nullptr;
    }
    buffer->desc = *tensor; // copy all fields
    buffer->data = malloc(size);
    buffer->size = size;
    buffer->impl = nullptr;
    buffer->name = "";
    return buffer;
}

size_t HIAI_NDTensorDesc_GetTotalDimNum(const HIAI_NDTensorDesc* tensorDesc)
{
    size_t totalSize = 1;
    for (size_t i = 0; i < tensorDesc->dimNum; i++) {
        int32_t dimValue = tensorDesc->dims[i];
        totalSize *= dimValue;
    }

    return totalSize;
}

size_t HIAI_NDTensorDesc_GetDimNum_stub(const HIAI_NDTensorDesc* tensorDesc)
{
    if (tensorDesc == nullptr) {
        return 0;
    }
    return tensorDesc->dimNum;
}

int32_t HIAI_NDTensorDesc_GetDim(const HIAI_NDTensorDesc* tensorDesc, size_t index)
{
    if (tensorDesc == nullptr) {
        return -1;
    }

    if (index >= tensorDesc->dimNum) {
        return -1;
    }

    return tensorDesc->dims[index];
}

HIAI_DataType HIAI_NDTensorDesc_GetDataType(const HIAI_NDTensorDesc* tensorDesc)
{
    if (tensorDesc == nullptr) {
        return (HIAI_DataType)0;
    }
    return tensorDesc->dataType;
}

HIAI_Format HIAI_NDTensorDesc_GetFormat(const HIAI_NDTensorDesc* tensorDesc)
{
    if (tensorDesc == nullptr) {
        return (HIAI_Format)0;
    }
    return tensorDesc->format;
}

void* GetChannelAndScaleFactorByFormat(HIAI_ImageFormat format, int32_t& channel, float& factor)
{
    static const std::map<HIAI_ImageFormat, float> sizeScaleMap = {
        {HIAI_YUV420SP_U8, 1.5},
        {HIAI_XRGB8888_U8, 4},
        {HIAI_YUV400_U8, 1},
        {HIAI_ARGB8888_U8, 4},
        {HIAI_YUYV_U8, 2},
        {HIAI_YUV422SP_U8, 2},
        {HIAI_AYUV444_U8, 4},
        {HIAI_RGB888_U8, 3},
    };
    static const std::map<HIAI_ImageFormat, int32_t> cMap = {
        {HIAI_YUV420SP_U8, 3},
        {HIAI_XRGB8888_U8, 4},
        {HIAI_YUV400_U8, 1},
        {HIAI_ARGB8888_U8, 4},
        {HIAI_YUYV_U8, 3},
        {HIAI_YUV422SP_U8, 3},
        {HIAI_AYUV444_U8, 4},
        {HIAI_RGB888_U8, 3},
    };
    auto sizeScale = sizeScaleMap.find(format);
    if (sizeScale == sizeScaleMap.end()) {
        return nullptr;
    }
    auto c = cMap.find(format);
    if (c == cMap.end()) {
        return nullptr;
    }
    channel = c->second;
    factor = sizeScale->second;

    return 0;
}

int32_t GetChannelIndex(const HIAI_NDTensorDesc* desc)
{
    int32_t cIndex = -1;
    HIAI_Format format = HIAI_NDTensorDesc_GetFormat(desc);
    if (format == HIAI_FORMAT_NCHW) {
        cIndex = 1;
    } else if (format == HIAI_FORMAT_NHWC) {
        cIndex = 3;
    }
    return cIndex;
}

HIAI_NDTensorDesc* HIAI_NDTensorDesc_Create(
    const int32_t* dims, size_t dimNum, HIAI_DataType dataType, HIAI_Format format)
{
    std::cout << __func__ << std::endl;
    HIAI_NDTensorDesc* desc = static_cast<HIAI_NDTensorDesc*>(malloc(sizeof(HIAI_NDTensorDesc)));
    size_t dimsSize = dimNum * sizeof(int32_t);
    desc->dims = (int32_t*)malloc(dimsSize);
    (void)memcpy_s(desc->dims, dimsSize, dims, dimsSize);
    desc->dimNum = dimNum;
    desc->dataType = dataType;
    desc->format = format;
    return desc;
}

HIAI_NDTensorDesc* HIAI_NDTensorDesc_Clone(const HIAI_NDTensorDesc* tensorDesc)
{
    std::cout << __func__ << std::endl;
    if (tensorDesc == nullptr) {
        return nullptr;
    }

    return HIAI_NDTensorDesc_Create(tensorDesc->dims, tensorDesc->dimNum, tensorDesc->dataType, tensorDesc->format);
}

void HIAI_NDTensorDesc_Destroy(HIAI_NDTensorDesc** tensorDesc)
{
    std::cout << __func__ << std::endl;
    if (tensorDesc == nullptr || *tensorDesc == nullptr) {
        return;
    }
    if ((*tensorDesc)->dims != nullptr) {
        free((*tensorDesc)->dims);
    }
    free((*tensorDesc));
    (*tensorDesc) = nullptr;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNDTensorDesc(const HIAI_NDTensorDesc* desc)
{
    std::cout << __func__ << std::endl;
    if (desc == nullptr) {
        return nullptr;
    }
    size_t size = HIAI_NDTensorDesc_GetTotalDimNum(desc) * g_dataTypeElementSize[HIAI_NDTensorDesc_GetDataType(desc)];
    if (size == 0) {
        return nullptr;
    }
    // 复制一份desc
    HIAI_NDTensorDesc* d = HIAI_NDTensorDesc_Clone(desc);

    // 打桩创建rom侧buffer
    HIAI_NDTensorBuffer* buffer = static_cast<HIAI_NDTensorBuffer*>(malloc(sizeof(HIAI_NDTensorBuffer)));
    (void)memset_s(buffer, sizeof(HIAI_NDTensorBuffer), 0, sizeof(HIAI_NDTensorBuffer));

    buffer->desc = d;
    buffer->size = size;
    buffer->data = malloc(size);
    buffer->handle = malloc(1);

    return buffer;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromSize(const HIAI_NDTensorDesc* desc, size_t size)
{
    std::cout << __func__ << std::endl;
    if (size == 0) {
        return nullptr;
    }
    // 复制一份desc
    HIAI_NDTensorDesc* d = HIAI_NDTensorDesc_Clone(desc);

    // 打桩创建rom侧buffer
    HIAI_NDTensorBuffer* buffer = static_cast<HIAI_NDTensorBuffer*>(malloc(sizeof(HIAI_NDTensorBuffer)));
    (void)memset_s(buffer, sizeof(HIAI_NDTensorBuffer), 0, sizeof(HIAI_NDTensorBuffer));
    buffer->desc = d;
    buffer->size = size;
    buffer->data = malloc(size);
    buffer->handle = malloc(1);

    return buffer;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromFormat(const HIAI_NDTensorDesc* desc, HIAI_ImageFormat format)
{
    std::cout << __func__ << std::endl;
    int32_t channel = 0;
    float scaleFactor = 0;

    GetChannelAndScaleFactorByFormat(format, channel, scaleFactor);
    int32_t cIndex = GetChannelIndex(desc);

    int32_t dimC = HIAI_NDTensorDesc_GetDim(desc, cIndex);
    size_t totalDimNum = HIAI_NDTensorDesc_GetTotalDimNum(desc);

    size_t totalBytes = totalDimNum / dimC;

std::cout << "ysj format = " << format << std::endl;
    std::cout << "ysj totalDimNum = " << totalDimNum << std::endl;
    std::cout << "ysj scaleFactor = " << scaleFactor << std::endl;
    totalBytes = static_cast<size_t>(totalBytes * scaleFactor);

    size_t newDimNum = HIAI_NDTensorDesc_GetDimNum_stub(desc);
    int32_t* newDims = (int32_t*)malloc(sizeof(int32_t) * newDimNum);
    for (int i = 0; i < newDimNum; i++) {
        if (i == cIndex) {
            newDims[i] = channel; // 设置C通道的dim值
            continue;
        }
        newDims[i] = HIAI_NDTensorDesc_GetDim(desc, i);
    }
    HIAI_NDTensorDesc* newDesc = HIAI_NDTensorDesc_Create(
        newDims, newDimNum, HIAI_NDTensorDesc_GetDataType(desc), HIAI_NDTensorDesc_GetFormat(desc));
    free(newDims);

    // 打桩创建rom侧buffer
    HIAI_NDTensorBuffer* buffer = static_cast<HIAI_NDTensorBuffer*>(malloc(sizeof(HIAI_NDTensorBuffer)));
    (void)memset_s(buffer, sizeof(HIAI_NDTensorBuffer), 0, sizeof(HIAI_NDTensorBuffer));
    buffer->desc = newDesc;
    buffer->size = totalBytes;
    buffer->data = malloc(totalBytes);
    buffer->handle = malloc(1);

     std::cout << "ysj totalBytes = " << totalBytes << std::endl;

    return buffer;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNativeHandle(
    const HIAI_NDTensorDesc* desc, const HIAI_NativeHandle* handle)
{
    std::cout << __func__ << std::endl;
    if (desc == nullptr) {
        return nullptr;
    }
    // 复制一份desc
    HIAI_NDTensorDesc* d = HIAI_NDTensorDesc_Clone(desc);

    // 打桩创建rom侧buffer
    HIAI_NDTensorBuffer* buffer = static_cast<HIAI_NDTensorBuffer*>(malloc(sizeof(HIAI_NDTensorBuffer)));
    (void)memset_s(buffer, sizeof(HIAI_NDTensorBuffer), 0, sizeof(HIAI_NDTensorBuffer));

    size_t totalDimNum = HIAI_NDTensorDesc_GetTotalDimNum(desc);
    size_t size = totalDimNum * g_dataTypeElementSize[desc->dataType];

    buffer->desc = d;
    buffer->size = size;
    buffer->data = malloc(size);
    buffer->handle = malloc(1);

    return buffer;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBufferNoCopy(const HIAI_NDTensorDesc* desc, size_t size, const void* data)
{
    HIAI_NDTensorBuffer* ndBuffer = (HIAI_NDTensorBuffer*)malloc(sizeof(HIAI_NDTensorBuffer));
    if (ndBuffer == nullptr) {
        return nullptr;
    }
    (void)memset_s(ndBuffer, sizeof(HIAI_NDTensorBuffer), 0, sizeof(HIAI_NDTensorBuffer));

    ndBuffer->desc = HIAI_NDTensorDesc_Clone(desc);
    if (ndBuffer->desc == nullptr) {
        free(ndBuffer);
        return nullptr;
    }

    ndBuffer->size = size;
    ndBuffer->data = (void*)data;
    ndBuffer->handle = nullptr;
    ndBuffer->owner = false;
    return ndBuffer;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateNoCopy(const HIAI_NDTensorDesc* desc, const void* data, size_t dataSize)
{
    if (desc == nullptr) {
        return nullptr;
    }

    if (data == nullptr) {
        return nullptr;
    }

    if (dataSize == 0) {
        return nullptr;
    }

    return HIAI_NDTensorBufferNoCopy(desc, dataSize, data);
}

int32_t HIAI_NDTensorBuffer_GetFd(const HIAI_NDTensorBuffer* tensorBuffer)
{
    std::cout << __func__ << std::endl;
    if (tensorBuffer == nullptr || tensorBuffer->handle == nullptr) {
        return -1;
    }
    return 1;
}

int32_t HIAI_NDTensorBuffer_GetOriginFd(const HIAI_NDTensorBuffer* tensorBuffer)
{
    std::cout << __func__ << std::endl;
    if (tensorBuffer == nullptr || tensorBuffer->handle == nullptr) {
        std::cout << "nullptr\n" << std::endl;
        return -1;
    }
    return 1;
}

HIAI_NDTensorDesc* HIAI_NDTensorBuffer_GetNDTensorDesc(const HIAI_NDTensorBuffer* tensorBuffer)
{
    std::cout << __func__ << std::endl;
    if (tensorBuffer == nullptr) {
        return nullptr;
    }
    return tensorBuffer->desc;
}

void* HIAI_NDTensorBuffer_GetHandle(const HIAI_NDTensorBuffer* tensorBuffer)
{
    std::cout << __func__ << std::endl;
    if (tensorBuffer == nullptr) {
        return nullptr;
    }
    return tensorBuffer->handle;
}

size_t HIAI_NDTensorBuffer_GetSize(const HIAI_NDTensorBuffer* tensorBuffer)
{
    std::cout << __func__ << std::endl;
    if (tensorBuffer == nullptr) {
        std::cout << "ysj HIAI_NDTensorBuffer_GetSize 0000000000\n" << std::endl;
        return 0;
    }
    return tensorBuffer->size;
}

void* HIAI_NDTensorBuffer_GetData(const HIAI_NDTensorBuffer* ndBuffer)
{
    std::cout << __func__ << std::endl;
    if (ndBuffer == nullptr) {
        return nullptr;
    }
    return ndBuffer->data;
}

void HIAI_NDTensorBuffer_Destroy(HIAI_NDTensorBuffer** ndBuffer)
{
    std::cout << __func__ << " " << (void*)(*ndBuffer) << std::endl;
    if (ndBuffer == nullptr || *ndBuffer == nullptr) {
        return;
    }
    // 释放ndbuffer desc
    HIAI_NDTensorDesc* desc = (*ndBuffer)->desc;
    HIAI_NDTensorDesc_Destroy(&desc);

    if ((*ndBuffer)->data != nullptr) {
        free((*ndBuffer)->data);
    }

    void* handle = HIAI_NDTensorBuffer_GetHandle(*ndBuffer);
    if (handle != nullptr) {
        free(handle);
    }
    // 释放ndbuffer
    free(*ndBuffer);
    *ndBuffer = nullptr;
}

// for HIAI_TensorBuffer
HIAI_TensorBuffer* HIAI_TensorBuffer_create(int n, int c, int h, int w)
{
    std::cout << __func__ << std::endl;
    HIAI_TensorDescription t = HIAI_TENSOR_DESCRIPTION_INIT;
    t.number = n;
    t.channel = c;
    t.height = h;
    t.width = w;
    return HIAI_TensorBuffer_createFromTensorDesc(&t);
}

HIAI_TensorBuffer* HIAI_TensorBuffer_CreateWithDataType(int n, int c, int h, int w, HIAI_DataType dataType)
{
    std::cout << __func__ << std::endl;
    auto buffer = static_cast<HIAI_TensorBuffer*>(malloc(sizeof(HIAI_TensorBuffer)));
    (void)memset_s(buffer, sizeof(HIAI_TensorBuffer), 0, sizeof(HIAI_TensorBuffer));
    return buffer;
}

HIAI_TensorBuffer* HIAI_ImageBuffer_create(int n, int h, int w, HIAI_ImageFormat imageFormat)
{
    std::cout << __func__ << std::endl;

    int value = ControlClient::GetInstance().GetExpectValue(ClientKey::CLIENT_ImageBuffer_create);
    if (value != -1) {
        return nullptr;
    }

    static const std::map<HIAI_ImageFormat, float> sizeScaleMap = {
        {HIAI_YUV420SP_U8, 1.5},
        {HIAI_XRGB8888_U8, 4},
        {HIAI_YUV400_U8, 1},
        {HIAI_ARGB8888_U8, 4},
        {HIAI_YUYV_U8, 2},
        {HIAI_YUV422SP_U8, 2},
        {HIAI_AYUV444_U8, 4},
        {HIAI_RGB888_U8, 3},
    };

    static const std::map<HIAI_ImageFormat, int> cMap = {
        {HIAI_YUV420SP_U8, 3},
        {HIAI_XRGB8888_U8, 4},
        {HIAI_YUV400_U8, 1},
        {HIAI_ARGB8888_U8, 4},
        {HIAI_YUYV_U8, 3},
        {HIAI_YUV422SP_U8, 3},
        {HIAI_AYUV444_U8, 4},
        {HIAI_RGB888_U8, 3},
    };
    auto sizeScale = sizeScaleMap.find(imageFormat);
    if (sizeScale == sizeScaleMap.end()) {
        printf("unsupport imageFormat: %d \n", imageFormat);
        return nullptr;
    }

    int32_t totalBytes = n * h;
    totalBytes *= w;
    totalBytes *= sizeScale->second;

    auto c = cMap.find(imageFormat);
    if (c == cMap.end()) {
        printf("unsupport imageFormat: %d \n", imageFormat);
        return nullptr;
    }

    HIAI_TensorBuffer* buffer = static_cast<HIAI_TensorBuffer*>(malloc(sizeof(HIAI_TensorBuffer)));
    buffer->size = totalBytes;
    buffer->data = malloc(totalBytes);
    buffer->impl = nullptr;
    return buffer;
}

HIAI_TensorBuffer* HIAI_TensorBuffer_create_v2(int n, int c, int h, int w, int size)
{
    std::cout << __func__ << std::endl;
    HIAI_TensorDescription t = HIAI_TENSOR_DESCRIPTION_INIT;
    t.number = n;
    t.channel = c;
    t.height = h;
    t.width = w;

    auto buffer = static_cast<HIAI_TensorBuffer*>(malloc(sizeof(HIAI_TensorBuffer)));

    buffer->desc = t;
    buffer->size = size;
    buffer->data = malloc(size);
    buffer->impl = nullptr;

    return buffer;
}

namespace hiai {

struct NativeHandle {
    int fd;
    int size;
    int offset;
};

HIAI_TensorBuffer* HIAI_TensorBuffer_createTensorFromNativeHandle(
    HIAI_TensorDescription* tensor, hiai::NativeHandle* nativeHandle)
{
    std::cout << __func__ << std::endl;
    auto buffer = static_cast<HIAI_TensorBuffer*>(malloc(sizeof(HIAI_TensorBuffer)));
    buffer->size = 1024;
    buffer->data = malloc(1024);
    buffer->impl = nullptr;
    return buffer;
}

PrivateHandleInfo* HIAI_GetHandleInfo_From_BufferHandle(buffer_handle_t handle)
{
    std::cout << __func__ << std::endl;

    int value = ControlClient::GetInstance().GetExpectValue(ClientKey::CLIENT_GetHandleInfo_From_BufferHandle);
    if (value != -1) {
        return nullptr;
    }
    const int privateFormat = 16;
    auto info = static_cast<PrivateHandleInfo*>(malloc(sizeof(PrivateHandleInfo)));
    (void)memset_s(info, sizeof(PrivateHandleInfo), 0, sizeof(PrivateHandleInfo));
    info->fd = 0;
    info->size = 1024;
    info->offset = 0;
    info->format = privateFormat;
    info->w = 100;
    info->h = 100;
    info->strideW = 10;
    info->strideH = 10;
    return info;
}

void HIAI_DestroyPrivateHandle(PrivateHandleInfo* handle)
{
    std::cout << __func__ << std::endl;
    if (handle) {
        free(handle);
        handle = nullptr;
    }
}

}; // namespace hiai

HIAI_TensorBuffer* HIAI_TensorBuffer_createFromTensorDescWithSize(HIAI_TensorDescription* tensor, int size)
{
    std::cout << __func__ << std::endl;
    return nullptr;
}

int HIAI_TensorBuffer_getFdAndSize(
    HIAI_TensorBuffer* buffer[], int count, std::vector<int>& fdList, std::vector<int>& sizeList)
{
    std::cout << __func__ << std::endl;
    for (int i = 0; i < count; i++) {
        fdList.push_back(1);
        sizeList.push_back(1);
    }
    return 0;
}

int HIAI_TensorBuffer_getOriginFd(HIAI_TensorBuffer* buffer[], int count, std::vector<int>& fdList)
{
    std::cout << __func__ << std::endl;
    for (int i = 0; i < count; i++) {
        fdList.push_back(1);
    }
    return 0;
}

void HIAI_TensorBuffer_destroy(HIAI_TensorBuffer* buffer)
{
    std::cout << __func__ << std::endl;
    if (buffer == nullptr) {
        return;
    }
    if (buffer->data != nullptr) {
        free(buffer->data);
    }

    if (buffer->impl != nullptr) {
        free(buffer->impl);
    }

    if (buffer != nullptr) {
        free(buffer);
        buffer = nullptr;
    }
}

HIAI_TensorDescription HIAI_TensorBuffer_getTensorDesc(HIAI_TensorBuffer* b)
{
    std::cout << __func__ << std::endl;
    HIAI_TensorDescription init = HIAI_TENSOR_DESCRIPTION_INIT;
    return b ? b->desc : init;
}

void* HIAI_TensorBuffer_getRawBuffer(HIAI_TensorBuffer* b)
{
    std::cout << __func__ << std::endl;
    return b ? b->data : nullptr;
}

int HIAI_TensorAipp_getRawBufferSize(HIAI_TensorAippPara* tensorAipp)
{
    std::cout << __func__ << std::endl;
    return tensorAipp ? static_cast<int>(tensorAipp->size) : -1;
}

int HIAI_TensorBuffer_getBufferSize(HIAI_TensorBuffer* b)
{
    std::cout << __func__ << std::endl;
    return b ? b->size : 0;
}

HIAI_ModelTensorInfo* HIAI_ModelManager_getModelTensorInfo(HIAI_ModelManager* manager, const char* modelName)
{
    std::cout << __func__ << std::endl;
    const size_t inputCnt = 1;
    const size_t outputCnt = 1;
    const size_t dimension = 4; // NCHW
    size_t mallocSize = sizeof(HIAI_ModelTensorInfo) + (dimension * sizeof(int) * (inputCnt + outputCnt));
    auto info = static_cast<HIAI_ModelTensorInfo*>(malloc(mallocSize));
    info->input_cnt = inputCnt;
    info->output_cnt = outputCnt;
    info->input_shape = reinterpret_cast<int*>(reinterpret_cast<char*>(info) + sizeof(HIAI_ModelTensorInfo));
    info->output_shape = info->input_shape + (dimension * inputCnt);

    for (uint32_t i = 0, pos = 0; i < inputCnt; ++i) {
        info->input_shape[pos++] = 1;
        info->input_shape[pos++] = 3;
        info->input_shape[pos++] = 64;
        info->input_shape[pos++] = 64;
    }

    for (uint32_t i = 0, pos = 0; i < outputCnt; ++i) {
        info->output_shape[pos++] = 1;
        info->output_shape[pos++] = 32;
        info->output_shape[pos++] = 1;
        info->output_shape[pos++] = 1;
    }
    return info;
}

void HIAI_ModelManager_releaseModelTensorInfo(HIAI_ModelTensorInfo* modelTensor)
{
    std::cout << __func__ << std::endl;
    if (modelTensor != nullptr) {
        free(modelTensor);
        modelTensor = nullptr;
    }
}

HIAI_ModelNDTensorInfo* HIAI_ModelManager_GetModelNDTensorInfo(HIAI_ModelManager* manager, const char* modelName)
{
    std::cout << __func__ << std::endl;
    if (manager == nullptr) {
        return nullptr;
    }
    if (modelName == nullptr) {
        return nullptr;
    }

    HIAI_ModelNDTensorInfo* info = (HIAI_ModelNDTensorInfo*)(malloc(sizeof(HIAI_ModelNDTensorInfo)));
    if (info == nullptr) {
        return nullptr;
    }
    info->inputCnt = 1;
    info->outputCnt = 1;
    info->inputShape = (HIAI_NDTensorDesc**)(malloc(sizeof(HIAI_NDTensorDesc*) * (info->inputCnt)));
    if (info->inputShape == nullptr) {
        free(info);
        return nullptr;
    }
    info->outputShape = (HIAI_NDTensorDesc**)(malloc(sizeof(HIAI_NDTensorDesc*) * (info->outputCnt)));
    if (info->outputShape == nullptr) {
        free(info->inputShape);
        free(info);
        return nullptr;
    }
    vector<int32_t> dims = {1, 3, 64, 64};

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dims.size(), (HIAI_DataType)0, (HIAI_Format)0);
    info->inputShape[0] = desc; // sizeof(HIAI_ModelNDTensorInfo)

    vector<int32_t> dimsOut = {1, 32, 1, 1};
    HIAI_NDTensorDesc* descOut =
        HIAI_NDTensorDesc_Create(dimsOut.data(), dimsOut.size(), (HIAI_DataType)0, (HIAI_Format)0);
    info->outputShape[0] = descOut; // sizeof(HIAI_ModelNDTensorInfo)

    std::cout << "success" << std::endl;
    return info;
}

void HIAI_ModelManager_ReleaseModelNDTensorInfo(HIAI_ModelNDTensorInfo* modelTensor)
{
    std::cout << __func__ << std::endl;
    if (modelTensor == nullptr) {
        return;
    }
    if (modelTensor->inputShape != nullptr) {
        for (int i = 0; i < modelTensor->inputCnt; ++i) {
            HIAI_NDTensorDesc_Destroy(&modelTensor->inputShape[i]);
        }
        free(modelTensor->inputShape);
    }
    if (modelTensor->outputShape != nullptr) {
        for (int i = 0; i < modelTensor->outputCnt; ++i) {
            HIAI_NDTensorDesc_Destroy(&modelTensor->outputShape[i]);
        }
        free(modelTensor->outputShape);
    }
    free(modelTensor);
    modelTensor = nullptr;

    return;
}

HIAI_ModelTensorInfoV2* HIAI_ModelManager_getModelTensorInfoV2(HIAI_ModelManager* manager, const char* modelName)
{
    std::cout << __func__ << std::endl;
    const size_t inputCnt = 1;
    const size_t outputCnt = 1;
    const size_t dimension = 4; // NCHW

    auto info = (HIAI_ModelTensorInfoV2*)malloc(sizeof(HIAI_ModelTensorInfoV2));
    HIAI_TensorDescriptionV2* input_shape = (HIAI_TensorDescriptionV2*)malloc(sizeof(HIAI_TensorDescriptionV2));
    HIAI_TensorDescriptionV2* output_shape = (HIAI_TensorDescriptionV2*)malloc(sizeof(HIAI_TensorDescriptionV2));
    info->inputCnt = inputCnt;
    info->outputCnt = outputCnt;
    info->inputShape = input_shape;
    info->outputShape = output_shape;

    info->inputShape->number = 1;
    info->inputShape->channel = 3;
    info->inputShape->height = 64;
    info->inputShape->width = 64;
    info->inputShape->dataType = HIAI_DataType::HIAI_DATATYPE_FLOAT32;

    info->outputShape->number = 1;
    info->outputShape->channel = 32;
    info->outputShape->height = 1;
    info->outputShape->width = 1;
    info->outputShape->dataType = HIAI_DataType::HIAI_DATATYPE_FLOAT32;

    return info;
}

void HIAI_ModelManager_releaseModelTensorInfoV2(HIAI_ModelTensorInfoV2* modelTensor)
{
    std::cout << __func__ << std::endl;
    if (modelTensor != nullptr) {
        if (modelTensor->inputShape) {
            free(modelTensor->inputShape);
            modelTensor->inputShape = nullptr;
        }

        if (modelTensor->outputShape) {
            free(modelTensor->outputShape);
            modelTensor->outputShape = nullptr;
        }
        free(modelTensor);
        modelTensor = nullptr;
    }
    return;
}

HIAI_TensorBuffer* HIAI_TensorBuffer_createFromTensorDescV2(HIAI_TensorDescriptionV2* tensor)
{
    std::cout << __func__ << std::endl;
    if (tensor == nullptr) {
        return nullptr;
    }
    uint32_t size = tensor->number * tensor->channel * tensor->height * tensor->width * sizeof(float);
    uint32_t malloc_size = sizeof(HIAI_TensorBuffer) + size;
    auto* buffer = static_cast<HIAI_TensorBuffer*>(malloc(sizeof(HIAI_TensorBuffer) + size));
    if (buffer == nullptr) {
        return nullptr;
    }

    HIAI_TensorDescription td;
    td.number = static_cast<uint32_t>(tensor->number);
    td.channel = static_cast<uint32_t>(tensor->channel);
    td.height = static_cast<uint32_t>(tensor->height);
    td.width = static_cast<uint32_t>(tensor->width);
    td.dataType = HIAI_DataType::HIAI_DATATYPE_FLOAT32;
    buffer->desc = td; // copy all fields
    buffer->data = buffer;
    buffer->size = size;
    buffer->impl = nullptr;
    buffer->name = "";
    return buffer;
}

int HIAI_ModelTensorInfoV2_getIOCount(const HIAI_ModelTensorInfoV2* tensorInfo, HIAI_IO_TYPE type)
{
    std::cout << __func__ << std::endl;
    return type == HIAI_IO_INPUT ? tensorInfo->inputCnt : tensorInfo->outputCnt;
}

HIAI_TensorDescriptionV2* HIAI_ModelTensorInfoV2_getTensorDescription(
    const HIAI_ModelTensorInfoV2* tensorInfo, HIAI_IO_TYPE type, int index)
{
    std::cout << __func__ << std::endl;
    HIAI_TensorDescriptionV2* descPointer = nullptr;
    if (tensorInfo != nullptr)
        descPointer = (type == HIAI_IO_INPUT ? tensorInfo->inputShape : tensorInfo->outputShape);
    return descPointer;
}

int HIAI_TensorDescriptionV2_getDimensions(const HIAI_TensorDescriptionV2* tensorDesc, int* n, int* c, int* h, int* w)
{
    std::cout << __func__ << std::endl;
    return 0;
}

const char* HIAI_TensorDescriptionV2_getName(const HIAI_TensorDescriptionV2* tensorDesc)
{
    std::cout << __func__ << std::endl;
    return "";
}

#ifdef DYNAMIC_BATCH
int HIAI_TensorBuffer_setDynamicBatch(HIAI_TensorBuffer* input[], int nInput, int inputIndex, int batchNumber)
{
    std::cout << __func__ << std::endl;
    return 0;
}
#endif

void* HIAI_TensorAipp_getRawBuffer(HIAI_TensorAippPara* tensorAipp)
{
    std::cout << __func__ << std::endl;
    return tensorAipp ? tensorAipp->parasBuff : nullptr;
}

int HIAI_TensorAipp_getInputIndex(HIAI_TensorAippPara* tensorAipp)
{
    std::cout << __func__ << std::endl;
    return tensorAipp ? static_cast<int>(tensorAipp->inputIndex) : -1;
}

void HIAI_TensorAipp_setInputIndex(HIAI_TensorAippPara* tensorAipp, unsigned int inputIndex)
{
    std::cout << __func__ << std::endl;
    if (tensorAipp != nullptr) {
        tensorAipp->inputIndex = inputIndex;
    }
}

int HIAI_TensorAipp_getInputAippIndex(HIAI_TensorAippPara* tensorAipp)
{
    std::cout << __func__ << std::endl;
    return tensorAipp ? static_cast<int>(tensorAipp->inputEdgeIndex) : -1;
}

void HIAI_TensorAipp_setInputAippIndex(HIAI_TensorAippPara* tensorAipp, unsigned int inputAippIndex)
{
    std::cout << __func__ << std::endl;
    if (tensorAipp != nullptr) {
        tensorAipp->inputEdgeIndex = inputAippIndex;
    }
}

int HIAI_ModelManger_getTensorAippInfo(HIAI_ModelManager* manager, const char* modelName, unsigned int index,
    unsigned int* aippCount, unsigned int* batchCount)
{
    std::cout << __func__ << std::endl;
    if (index > 0) {
        return 1;
    }
    *aippCount = 1;
    *batchCount = 1;
    return 0;
}

HIAI_TensorAippPara* HIAI_TensorAipp_create(unsigned int batchCount)
{
    std::cout << __func__ << std::endl;
    const uint32_t size = sizeof(HIAI_TensorAippCommPara) + sizeof(HIAI_TensorAippBatchPara) * batchCount;
    auto aippPara = static_cast<HIAI_TensorAippPara*>(malloc(sizeof(HIAI_TensorAippPara)));
    if (aippPara == nullptr) {
        return nullptr;
    }
    auto buffer = static_cast<HIAI_TensorAippCommPara*>(malloc(size));
    aippPara->inputIndex = 0;
    aippPara->inputEdgeIndex = 0;
    aippPara->parasBuff = buffer;
    aippPara->size = size;
    aippPara->impl = nullptr;
    return aippPara;
}

void HIAI_TensorAipp_destroy(HIAI_TensorAippPara* aippParas)
{
    std::cout << __func__ << std::endl;
    if (aippParas != nullptr) {
        if (aippParas->parasBuff) {
            free(aippParas->parasBuff);
            aippParas->parasBuff = nullptr;
        }
        free(aippParas);
        aippParas = nullptr;
    }
}

int HIAI_ModelManger_getTensorAippPara(HIAI_ModelManager* manager, const char* modelName, unsigned int index,
    HIAI_TensorAippPara* aippParas[], unsigned int aippCount, unsigned int batchCount)
{
    std::cout << __func__ << std::endl;
    for (uint32_t i = 0; i < aippCount; ++i) {
        if (aippParas[i] != nullptr) {
            HIAI_TensorAipp_destroy(aippParas[i]);
            aippParas[i] = nullptr;
        }
        aippParas[i] = HIAI_TensorAipp_create(batchCount);
    }
    return 0;
}

// for HIAI_ModelManager
HIAI_ModelManager* HIAI_ModelManager_create(HIAI_ModelManagerListener* listener)
{
    std::cout << __func__ << std::endl;
    auto manager = static_cast<HIAI_ModelManager*>(malloc(sizeof(HIAI_ModelManager)));
    if (listener != nullptr) {
        manager->sync = false;
        g_ModelManagerListenerMap[manager] = listener;
    } else {
        manager->sync = true;
    }
    return manager;
}

#ifdef SUPPORT_MEMORY_ALLOCATOR
HIAI_ModelManager* HIAI_ModelManager_createWithMem(
    HIAI_ModelManagerListener* listener, HIAI_ModelManagerMemAllocator* memAllocator)
{
    auto manager = static_cast<HIAI_ModelManager*>(malloc(sizeof(HIAI_ModelManager)));
    return manager;
}
#endif

void HIAI_ModelManager_destroy(HIAI_ModelManager* manager)
{
    std::cout << __func__ << std::endl;
    map<HIAI_ModelManager*, HIAI_ModelManagerListener*>::iterator it = g_ModelManagerListenerMap.find(manager);
    if (it != g_ModelManagerListenerMap.end()) {
        g_ModelManagerListenerMap.erase(it);
    }

    if (manager != nullptr) {
        free(manager);
        manager = nullptr;
    }
}

int HIAI_ModelManager_loadFromModelBuffers(HIAI_ModelManager* manager, HIAI_ModelBuffer* bufferArray[], int nBuffers)
{
    std::cout << __func__ << std::endl;

    int value = ControlClient::GetInstance().GetExpectValue(CLIENT_PERF_MODE);
    if (value != -1 && nBuffers > 0) {
        if (bufferArray[0]->perf != value) {
            return -1;
        }
    }

    if (manager->sync) {
        return 0;
    }

    void* userdata = nullptr;
    std::thread load_thread(onLoadDone, manager, userdata, ++g_timeStamp);
    load_thread.detach();
    this_thread::sleep_for(chrono::milliseconds(200));
    return g_timeStamp;
}

int HIAI_ModelManager_loadFromModelDescriptions(
    HIAI_ModelManager* manager, HIAI_ModelDescription descsArray[], int nDescs)
{
    std::cout << __func__ << std::endl;
    return 0;
}

static int Run(HIAI_ModelManager* manager)
{
    if (manager->sync) {
        return 0;
    }
    void* userdata = nullptr;
    std::thread run_thread(onRunDone, manager, userdata, g_timeStamp);
    run_thread.detach();
    return g_timeStamp;
}

int HIAI_ModelManager_runModel(HIAI_ModelManager* manager, HIAI_TensorBuffer* input[], int nInput,
    HIAI_TensorBuffer* output[], int nOutput, int ulTimeout, const char* modelName)
{
    std::cout << __func__ << std::endl;
    return Run(manager);
}

#define ON_ERROR_VALUE -100
#define ON_SERVICE_DIED_VLAUE -200
#define ON_TIMEOUT_VALUE -300

int HIAI_ModelManager_runModel_v3(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[], int inputNum,
    HIAI_NDTensorBuffer* output[], int outputNum, int ulTimeout, const char* modelName)
{
    std::cout << __func__ << " modelName" << modelName << std::endl;
    if (manager->sync) {
        return 0;
    }

    int ret = 0;
    int stamp = 0;
    void* userdata = nullptr;
    if (ulTimeout == ON_SERVICE_DIED_VLAUE) {
        std::thread run_thread(onServiceDied, manager, userdata);
        run_thread.detach();
    } else if (ulTimeout == ON_ERROR_VALUE) {
        int errorCode = 1;
        std::thread run_thread(onError, manager, userdata, stamp, errorCode);
        run_thread.detach();
    } else if (ulTimeout == ON_TIMEOUT_VALUE) {
        std::thread run_thread(onTimeout, manager, userdata, stamp);
        run_thread.detach();
    } else {
        std::thread run_thread(onRunDone, manager, userdata, g_timeStamp);
        run_thread.detach();
        ret = g_timeStamp;
    }
    return ret;
}

int HIAI_ModelManager_runAippModel(HIAI_ModelManager* manager, HIAI_TensorBuffer* input[], int nInput,
    HIAI_TensorAippPara* aippPara[], int nAipp, HIAI_TensorBuffer* output[], int nOutput, int ulTimeout,
    const char* modelName)
{
    std::cout << __func__ << std::endl;
    return Run(manager);
}

int HIAI_ModelManager_runAippModel_v3(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[], int nInput,
    HIAI_TensorAippPara* aippPara[], int nAipp, HIAI_NDTensorBuffer* output[], int nOutput, int ulTimeout,
    const char* modelName)
{
    std::cout << __func__ << std::endl;
    return Run(manager);
}

int HIAI_ModelManager_setInputsAndOutputs(HIAI_ModelManager* manager, const char* modelName, HIAI_TensorBuffer* input[],
    int nInput, HIAI_TensorBuffer* output[], int nOutput)
{
    std::cout << __func__ << std::endl;
    return 0;
}

int HIAI_ModelManager_startCompute(HIAI_ModelManager* manager, const char* modelName)
{
    std::cout << __func__ << std::endl;
    return 0;
}

int HIAI_ModelManager_unloadModel(HIAI_ModelManager* manager)
{
    std::cout << __func__ << std::endl;
    if (manager->sync) {
        return 0;
    }
    void* userdata = nullptr;
    std::thread unload_thread(onUnloadDone, manager, userdata, g_timeStamp);
    unload_thread.detach();
    return g_timeStamp;
}

char* HIAI_GetVersion_Config()
{
    return nullptr;
}

char* HIAI_GetVersion_Vendor()
{
    return nullptr;
}

char* HIAI_GetVersion()
{
    static char buf[] = "100.500.000.000";
    char* version = getenv("ROM_HIAI_VERSION");
    if (version == nullptr) {
        return buf;
    } else {
        printf("ROM_HIAI_VERSION = %s\n", version);
        return version;
    }
}

char* HIAI_GetApiVersion()
{
    return nullptr;
}

HIAI_MemBuffer* HIAI_MemBuffer_create_from_file(const char* path)
{
    std::cout << __func__ << std::endl;
    if (path == nullptr) {
        return nullptr;
    }
    // open model file
    char pathTemp[PATH_MAX + 1] = {0x00};
    if (strlen(path) > PATH_MAX || realpath(path, pathTemp) == nullptr) {
        return nullptr;
    }

    FILE* fp = fopen(pathTemp, "r");
    if (fp == nullptr) {
        return nullptr;
    }

    // get model file length
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return nullptr;
    }
    long fileLength = ftell(fp);
    if (fseek(fp, 0, SEEK_SET)) {
        fclose(fp);
        return nullptr;
    }
    if (fileLength <= 0) {
        fclose(fp);
        return nullptr;
    }

    // alloc model buffer
    void* data = malloc(static_cast<unsigned int>(fileLength));
    if (data == nullptr) {
        fclose(fp);
        return nullptr;
    }

    // read file into memory
    uint32_t readSize = static_cast<uint32_t>(fread(data, 1, static_cast<unsigned int>(fileLength), fp));
    // check if read success
    if (static_cast<long>(readSize) != fileLength) {
        free(data);
        data = nullptr;
        fclose(fp);
        return nullptr;
    }

    // close model file
    fclose(fp);

    // create an MemBuffer
    HIAI_MemBuffer* membuf = reinterpret_cast<HIAI_MemBuffer*>(malloc(sizeof(HIAI_MemBuffer)));
    if (membuf == nullptr) {
        free(data);
        data = nullptr;
        return nullptr;
    }

    // set size && data
    membuf->size = static_cast<uint32_t>(readSize);
    membuf->data = data;
    return membuf;
}

HIAI_MemBuffer* HIAI_MemBuffer_create_from_buffer(void* buffer, const uint32_t size)
{
    std::cout << __func__ << std::endl;
    if (buffer == nullptr || size == 0) {
        return nullptr;
    }
    auto membuf = static_cast<HIAI_MemBuffer*>(malloc(sizeof(HIAI_MemBuffer)));
    membuf->size = static_cast<uint32_t>(size);
    membuf->data = buffer;
    return membuf;
}

HIAI_MemBuffer* HIAI_ReadBinaryProto_from_file(const char* path)
{
    if (path == nullptr || strlen(path) == 0) {
        return nullptr;
    }
    auto membuf = static_cast<HIAI_MemBuffer*>(malloc(sizeof(HIAI_MemBuffer)));
    return membuf;
}

HIAI_MemBuffer* HIAI_ReadBinaryProto_from_buffer(void* data, const unsigned int size)
{
    if (data == nullptr) {
        return nullptr;
    }
    auto membuf = static_cast<HIAI_MemBuffer*>(malloc(sizeof(HIAI_MemBuffer)));
    return membuf;
}

HIAI_MemBuffer* HIAI_MemBuffer_createV2(HIAI_Framework frameworkType, HIAI_MemBuffer* inputModelBuffers[],
    unsigned int inputModelBuffersNum, unsigned int inputModelSize)
{
    int value = ControlClient::GetInstance().GetExpectValue(ClientKey::CLIENT_HIAI_MemBuffer_createV2);
    if (value != -1) {
        return nullptr;
    }
    if (inputModelSize == 0) {
        inputModelSize = 200 * 1024 * 1024;
    }
    void* data = malloc(inputModelSize);
    if (data == nullptr) {
        return nullptr;
    }

    HIAI_MemBuffer* membuf = reinterpret_cast<HIAI_MemBuffer*>(malloc(sizeof(HIAI_MemBuffer)));
    if (membuf == nullptr) {
        printf("HIAI_MemBuffer_create_from_size ERROR: create HIAI_MemBuffer fail\n");
        free(data);
        data = nullptr;
        return nullptr;
    }

    membuf->size = inputModelSize;
    membuf->data = data;
    return membuf;
}

HIAI_MemBuffer* HIAI_MemBuffer_create(
    HIAI_Framework frameworkType, HIAI_MemBuffer* inputModelBuffers[], const unsigned int inputModelBuffersNum)
{
    auto membuf = static_cast<HIAI_MemBuffer*>(malloc(sizeof(HIAI_MemBuffer)));
    size_t size = 200 * 1024 * 1024;
    void* data = malloc(size);
    if (data == nullptr) {
        return nullptr;
    }
    membuf->size = size;
    membuf->data = data;
    return membuf;
}

void HIAI_MemBuffer_destroy(HIAI_MemBuffer* membuf)
{
    free(membuf->data);
    membuf->data = nullptr;
    free(membuf);
    return;
}

bool HIAI_MemBuffer_export_file(HIAI_MemBuffer* membuf, const unsigned int buildSize, const char* buildPath)
{
    if (membuf == nullptr) {
        return false;
    }

    if (buildSize > membuf->size) {
        return false;
    }

    if (strlen(buildPath) > PATH_MAX) {
        return false;
    }

    int fd = open(buildPath, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        return false;
    }

    uint32_t writeSize = static_cast<uint32_t>(write(fd, membuf->data, buildSize));
    if (writeSize != buildSize) {
        close(fd);
        return false;
    }

    close(fd);
    return true;
}

int HIAI_ModelManager_buildModel(HIAI_ModelManager* manager, HIAI_Framework frameworkType,
    HIAI_MemBuffer* inputModelBuffers[], const unsigned int inputModelBuffersNum, HIAI_MemBuffer* outputModelBuffer,
    unsigned int* outModelSize)
{
    std::cout << __func__ << std::endl;
    *outModelSize = 2264;
    return 0;
}

bool HIAI_CheckModelCompatibility_from_file(HIAI_ModelManager* manager, const char* modelPath)
{
    return true;
}

bool HIAI_CheckModelCompatibility_from_buffer(HIAI_ModelManager* manager, void* buffer, const uint32_t size)
{
    return true;
}

static int CheckModelManagerValid(HIAI_ModelManager* manager)
{
    return 0;
}

int HIAI_ModelManager_setModelPriority(HIAI_ModelManager* manager, const char* modelName, HIAI_ModelPriority priority)
{
    return 0;
}

void HIAI_ModelManager_cancelCompute(HIAI_ModelManager* manager, const char* modelName)
{
}

int CreateAshmemRegionFd(const char* name, size_t size)
{
    return 0;
}

int HIAI_ModelManager_buildModel_v2(HIAI_ModelManager* manager, HIAI_Framework frameworkType,
    HIAI_MemBuffer* inputModelBuffers[], const unsigned int inputModelBuffersNum, HIAI_MemBuffer* outputModelBuffer,
    unsigned int* outModelSize, bool useOriginFormat)
{
    std::cout << __func__ << std::endl;
    FILE* fp = fopen("bin/llt/framework/domi/modelmanager/tf_softmax_hcs_cpucl.om", "r+");
    if (fp == nullptr) {
        printf("HIAI_ModelManager_buildModel_v2 ERROR: open the model file failed.\n");
        return -1;
    }
    long fileLength = 0;
    int result = fseek(fp, 0, SEEK_END);
    fileLength = ftell(fp);
    result = fseek(fp, 0, SEEK_SET);

    fread(outputModelBuffer->data, 1, fileLength, fp);

    *outModelSize = fileLength;
    return 0;
}

#ifdef __cplusplus
}
#endif
