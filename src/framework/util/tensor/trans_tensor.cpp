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

#include "framework/util/tensor/trans_tensor.h"

#include <list>
#include <cmath>
#include <string>
#include <vector>
#include "securec.h"

#include "infra/base/assertion.h"
#include "infra/math/fp16_t.h"

#include "framework/common/types.h"
#include "framework/infra/log/log.h"
#include "framework/graph/utils/tensor_utils.h"

#include "common/math/math_util.h"

#if defined(ARM_NEON_32)
#include <arm_neon.h>
#endif

using namespace std;
using namespace hiai;
/*
 * @ingroup dnn
 * @brief check whether expr is true, if not print log then return errorcode
 */
#ifndef CHECK
#define CHECK(expr, errorCode, errInfo, ...) \
    do { \
        if (!(expr)) { \
            FMK_LOGD(errInfo, ##__VA_ARGS__); \
            return errorCode; \
        } \
    } while (0);
#endif

/*
 * @ingroup dnn
 * @brief check whether expr is true, if not then return errorcode
 */
#ifndef CHECK_ONLY_RET
#define CHECK_ONLY_RET(expr, errorCode) \
    do { \
        if (!(expr)) { \
            return errorCode; \
        } \
    } while (0);
#endif

/*
 * @ingroup dnn
 * @brief check whether obj is null, if null then print log and return fail
 */
#ifndef CHECK_NULL_WITH_RET
#define CHECK_NULL_WITH_RET(obj, retValue) \
    do { \
        if (obj == nullptr) { \
            FMK_LOGD(#obj " is nullptr!"); \
            return retValue; \
        } \
    } while (0)
#endif

#ifndef CEIL
#define CEIL(N, n) (((N) + (n) - 1) / (n))
#endif

#ifdef ARM_NEON
#define TransTensorHalfToFloat16_neon(inPtr, outPtr) \
    do { \
        __asm__ __volatile( \
            "mov x2, %[inPtr]\n" \
            "mov x3, %[outPtr]\n" \
            "mov x4, #17\n" \
            "lsl x4, x4, #1\n" \
\
            "ld1 {v9.4h}, [x2], #8\n" \
            "fcvtl v10.4s, v9.4h\n" \
            "st1 {v10.4s}, [x3], #16\n" \
            "ld1 {v9.4h}, [x2], #8\n" \
            "fcvtl v10.4s, v9.4h\n" \
            "st1 {v10.4s}, [x3], #16\n" \
            "ld1 {v9.4h}, [x2], #8\n" \
            "fcvtl v10.4s, v9.4h\n" \
            "st1 {v10.4s}, [x3], #16\n" \
            "ld1 {v9.4h}, [x2], #8\n" \
            "fcvtl v10.4s, v9.4h\n" \
            "st1 {v10.4s}, [x3], #16\n" \
            : [outPtr] "+r"(outPtr) \
            : [inPtr] "r"(inPtr) \
            : "x2", "x3", "x4", "x5", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v18", "s17", "cc"); \
    } while (0)

#define TransTensorHalfToFloat3_neon(inPtr, outPtr) \
    do { \
        __asm__ __volatile("mov x2, %[inPtr]\n" \
                           "mov x3, %[outPtr]\n" \
                           "mov x4, #1\n" \
                           "lsl x4, x4, #2\n" \
\
                           "ld1 {v9.h}[0], [x2], #2\n" \
                           "mov h17, v9.h[0]\n" \
                           "fcvt s18, h17\n" \
                           "st1 {v18.s}[0], [x3], x4\n" \
                           "ld1 {v9.h}[0], [x2], #2\n" \
                           "mov h17, v9.h[0]\n" \
                           "fcvt s18, h17\n" \
                           "st1 {v18.s}[0], [x3], x4\n" \
                           "ld1 {v9.h}[0], [x2], #2\n" \
                           "mov h17, v9.h[0]\n" \
                           "fcvt s18, h17\n" \
                           "st1 {v18.s}[0], [x3], x4\n" \
                           : [outPtr] "+r"(outPtr) \
                           : [inPtr] "r"(inPtr) \
                           : "x2", "x3", "x4", "x5", "v9", "v18", "s17", "cc"); \
    } while (0)

#define TransTensorFp32ToFp16_C3_neon(inPtr, outPtr) \
    do { \
        __asm__ __volatile("mov x2, %[inPtr]\n" \
                           "mov x3, %[outPtr]\n" \
                           "mov x4, #1\n" \
                           "lsl x4, x4, #1\n" \
\
                           "ld1 {v9.s}[0], [x2], #4\n" \
                           "mov s17, v9.s[0]\n" \
                           "fcvt h18, s17\n" \
                           "st1 {v18.h}[0], [x3], x4\n" \
                           "ld1 {v9.s}[0], [x2], #4\n" \
                           "mov s17, v9.s[0]\n" \
                           "fcvt h18, s17\n" \
                           "st1 {v18.h}[0], [x3], x4\n" \
                           "ld1 {v9.s}[0], [x2], #4\n" \
                           "mov s17, v9.s[0]\n" \
                           "fcvt h18, s17\n" \
                           "st1 {v18.h}[0], [x3], x4\n" \
                           : [outPtr] "+r"(outPtr) \
                           : [inPtr] "r"(inPtr) \
                           : "x2", "x3", "x4", "x5", "v9", "v13", "v14", "v18", "s17", "cc"); \
    } while (0)

namespace {
inline void TransTensorFp32ToFp16_C8_neon(uint64_t inPtr, uint64_t outPtr)
{
    __asm__ __volatile("mov x2, %[inPtr]\n"
                       "mov x3, %[outPtr]\n"

                       "ld1 {v9.4s, v10.4s}, [x2], #32\n"
                       "fcvtn v11.4h, v9.4s\n"
                       "fcvtn2 v11.8h, v10.4s\n"
                       "st1 {v11.8h}, [x3], #16\n"
                       : [outPtr] "+r"(outPtr)
                       : [inPtr] "r"(inPtr)
                       : "x2", "x3", "v9", "v10", "v11", "cc");
}
}
#endif

namespace ge {
/*
 * @ingroup dnn
 * @brief check whether uint64 multiplication can result in overflow
 * @param [in] a  multiplicator
 * @param [in] b  multiplicator
 * @return uint32_t
 */
inline uint32_t CheckUint64AddOverflow(uint64_t a, uint64_t b)
{
    if (a == 0 || b == 0) {
        return SUCCESS;
    }

    if (a > (UINT64_MAX - b)) {
        return FAILED;
    }

    return SUCCESS;
}

inline uint32_t Uint64_addCheck(uint64_t a, uint64_t b)
{
    if (CheckUint64AddOverflow(a, b) != SUCCESS) {
        FMK_LOGD("Unsigned Integer64 %lu and %lu addition can result in overflow!", (a), (b));
        return FAILED;
    }
    return SUCCESS;
}

/*
 * @ingroup dnn
 * @brief max element number of tensor
 */
const int MAX_TENSOR_ELEMENT_COUNT = 2000000000;

/*
 * @ingroup dnn
 * @brief align size which input and output data actually ocupy
 */
const int DATA_MEMORY_ALIGN_SIZE = 32;

/*
 * @ingroup dnn
 * @brief dimcnt of 4d tensor = 4
 */
const int CC_4D_FORMAT_DIMCNT = 4;

/*
 * @ingroup dnn
 * @brief max number of dimensions when use NC1HWC0 format
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
const int CC_REALDIM_MAX = 4;
#pragma GCC diagnostic pop

/*
 * @ingroup fmk
 * @kernel ub address align size(bit)
 */
const int CC_INT8_C0_SIZE = 32;

/*
 * @ingroup dnn
 * @brief check whether int32 multiplication of input array can result in overflow
 * @param [in] array the array to execute multiplication
 * @param [in] num the number of array elements
 * @param [in|out] result point to the result of multiplication
 * @return uint32_t
 */
uint32_t CheckInt32ArrayMulOverflow(int32_t array[], int32_t num, int32_t* result)
{
    if (result == nullptr) {
        FMK_LOGD("result is nullptr !!!");
        return FAILED;
    }

    int32_t product = array[0];
    for (int32_t i = 1; i < num; i++) {
        if (CheckInt32MulOverflow(product, array[i]) != SUCCESS) {
            return FAILED;
        }
        product *= array[i];
    }
    *result = product;
    return SUCCESS;
}

static Status DescNd2NHWC(ccTensor_t& tensorDesc)
{
    int32_t dimTemp[CC_4D_FORMAT_DIMCNT] = {0};
    int32_t strideTemp[CC_4D_FORMAT_DIMCNT] = {0};
    CHECK(tensorDesc.dimCnt > 0, FAILED, "dimCnt must bigger than zero!!!");
    CHECK(tensorDesc.dimCnt <= CC_4D_FORMAT_DIMCNT, FAILED, "dimCnt must smaller than four!!!");
    const int32_t startTemp = CC_4D_FORMAT_DIMCNT - tensorDesc.dimCnt;
    for (int32_t i = 0; i < tensorDesc.dimCnt; i++) {
        dimTemp[i] = tensorDesc.dim[i];
        strideTemp[i] = tensorDesc.stride[i];
    }
    for (int32_t i = 0; i < CC_4D_FORMAT_DIMCNT; i++) {
        tensorDesc.dim[i] = 1;
        tensorDesc.stride[i] = 0;
    }
    for (int32_t i = startTemp; i < CC_4D_FORMAT_DIMCNT; i++) {
        tensorDesc.dim[i] = dimTemp[i - startTemp];
        tensorDesc.stride[i] = strideTemp[i - startTemp];
    }
    tensorDesc.format = FORMAT_NHWC;
    tensorDesc.dimCnt = CC_4D_FORMAT_DIMCNT;
    return SUCCESS;
}

static bool IsHashTableTensorFormat(const Format& format)
{
    if (format == FORMAT_HASHTABLE_LOOKUP_LOOKUPS || format == FORMAT_HASHTABLE_LOOKUP_KEYS ||
        format == FORMAT_HASHTABLE_LOOKUP_VALUE || format == FORMAT_HASHTABLE_LOOKUP_OUTPUT ||
        format == FORMAT_HASHTABLE_LOOKUP_HITS) {
        return true;
    }
    return false;
}

static Status GetTensorNdDescriptor(const ccTensor_t& tensorDesc, uint32_t arrLength, DataType_t& dataType,
    int32_t& dimCnt, int32_t dimA[], int32_t strideA[]);

static Status GetTensor4dDescriptor(const ccTensor_t& tensorDesc, DataType_t& dataType, int32_t& n, int32_t& c,
    int32_t& h, int32_t& w, int32_t& nStride, int32_t& cStride, int32_t& hStride, int32_t& wStride);

static Status Check5DTensorOverFlow(const ccTensor_t& tensorDesc)
{
    if (tensorDesc.format == FORMAT_NCDHW || tensorDesc.format == FORMAT_NDHWC) {
        int32_t productNum = 0;
        int32_t dims[5] = {
            tensorDesc.dim[0], tensorDesc.dim[1], tensorDesc.dim[2], tensorDesc.dim[3], tensorDesc.dim[4]};
        // check overflow
        if (CheckInt32ArrayMulOverflow(dims, 5, &productNum) != SUCCESS) {
            FMK_LOGE("5D Tesnor dims multiplication can result in overflow!");
            return FAILED;
        }
        if (productNum > MAX_TENSOR_ELEMENT_COUNT) {
            FMK_LOGE("5D Tensor element count is too large!");
            return FAILED;
        }
    } else {
        FMK_LOGE("unkown format.");
        return FAILED;
    }
    return SUCCESS;
}

static Status CheckTensorOverFlow(const ccTensor_t& tensorDesc)
{
    DataType_t dataType = CC_DATA_RESERVED;
    int32_t dimCnt = 0;
    int32_t dim[CC_DIM_MAX] = {0, 0, 0, 0, 0, 0, 0, 0};
    int32_t stride[CC_DIM_MAX] = {0, 0, 0, 0, 0, 0, 0, 0};
    int32_t n = -1;
    int32_t c = -1;
    int32_t h = -1;
    int32_t w = -1;
    int32_t nStride = -1;
    int32_t cStride = -1;
    int32_t hStride = -1;
    int32_t wStride = -1;

    if (tensorDesc.format == FORMAT_ND || IsHashTableTensorFormat(tensorDesc.format)) {
        CHECK_ONLY_RET(GetTensorNdDescriptor(tensorDesc, CC_DIM_MAX, dataType, dimCnt, dim, stride) == SUCCESS, FAILED);
    } else if ((tensorDesc.format == FORMAT_NCDHW) || (tensorDesc.format == FORMAT_NDHWC)) {
        FMK_LOGI("now is 5D format %d.", static_cast<int32_t>(tensorDesc.format));
    } else {
        CHECK_ONLY_RET(
            GetTensor4dDescriptor(tensorDesc, dataType, n, c, h, w, nStride, cStride, hStride, wStride) == SUCCESS,
            FAILED);
    }
    switch (tensorDesc.format) {
        case FORMAT_ND:
        case FORMAT_HASHTABLE_LOOKUP_LOOKUPS:
        case FORMAT_HASHTABLE_LOOKUP_KEYS:
        case FORMAT_HASHTABLE_LOOKUP_VALUE:
        case FORMAT_HASHTABLE_LOOKUP_OUTPUT:
        case FORMAT_HASHTABLE_LOOKUP_HITS: {
            int32_t productNum = 0;
            // check overflow
            CHECK((CheckInt32ArrayMulOverflow(dim, dimCnt, &productNum) == SUCCESS), FAILED,
                "Tesnor dims multiplication can result in overflow!");
            CHECK((productNum <= MAX_TENSOR_ELEMENT_COUNT), FAILED, "Tensor element count is too large!");
            break;
        }
        case FORMAT_NCHW:
        case FORMAT_NHWC: {
            int32_t productNum = 0, dims[4] = {n, c, h, w};
            // check overflow
            CHECK((CheckInt32ArrayMulOverflow(dims, 4, &productNum) == SUCCESS), FAILED,
                "Tesnor dims multiplication can result in overflow!");
            CHECK((productNum <= MAX_TENSOR_ELEMENT_COUNT), FAILED, "Tensor element count is too large!");
            break;
        }
        case FORMAT_NCDHW:
        case FORMAT_NDHWC: {
            if (Check5DTensorOverFlow(tensorDesc) != SUCCESS) {
                return FAILED;
            };
            break;
        }
        case FORMAT_NC1HWC0:
        case FORMAT_C1HWNC0: {
            int32_t c0 = ((dataType == CC_DATA_BOOL) || (dataType == CC_DATA_INT8) || (dataType == CC_DATA_UINT8) ||
                             (dataType == CC_DATA_QUINT8) || (dataType == CC_DATA_DUAL_SUB_UINT8) ||
                             (dataType == CC_DATA_DUAL_SUB_INT8)) ?
                CC_INT8_C0_SIZE :
                CC_CUBE_SIZE;
            int32_t c1 = static_cast<int32_t>(std::ceil(c * 1.0 / c0));
            int32_t productNum = 0, dims[5] = {n, c1, h, w, c0};
            // check overflow
            CHECK((CheckInt32ArrayMulOverflow(dims, 5, &productNum) == SUCCESS), FAILED,
                "Tesnor dims multiplication can result in overflow!");
            CHECK((productNum <= MAX_TENSOR_ELEMENT_COUNT), FAILED, "Tensor element count is too large!");
            break;
        }
        default:
            FMK_LOGD("s format %d is not supported!", tensorDesc.format);
            return FAILED;
    }
    return SUCCESS;
}

#ifdef ARM_NEON
#define TransTensorNHWCToHC1HWC0_C3_neon(inPtr, outPtr) \
    do { \
        __asm__ __volatile("mov x2, %[inPtr]\n" \
                           "mov x3, %[outPtr]\n" \
                           "mov x4, #1\n" \
                           "lsl x4, x4, #1\n" \
\
                           "movi   v13.8h, #0x0\n" \
                           "movi   v14.8h, #0x0\n" \
                           "st1 {v13.8h, v14.8h}, [x3]\n" \
\
                           "ld1 {v9.s}[0], [x2], #4\n" \
                           "mov s17, v9.s[0]\n" \
                           "fcvt h18, s17\n" \
                           "st1 {v18.h}[0], [x3], x4\n" \
                           "ld1 {v9.s}[0], [x2], #4\n" \
                           "mov s17, v9.s[0]\n" \
                           "fcvt h18, s17\n" \
                           "st1 {v18.h}[0], [x3], x4\n" \
                           "ld1 {v9.s}[0], [x2], #4\n" \
                           "mov s17, v9.s[0]\n" \
                           "fcvt h18, s17\n" \
                           "st1 {v18.h}[0], [x3], x4\n" \
                           : [outPtr] "+r"(outPtr) \
                           : [inPtr] "r"(inPtr) \
                           : "x2", "x3", "x4", "x5", "v9", "v13", "v14", "v18", "s17", "cc"); \
    } while (0)

#define TransTensorUint8ToUint8_neon(inPtr, outPtr) \
    do { \
        __asm__ __volatile("mov x2, %[inPtr]\n" \
                           "mov x3, %[outPtr]\n" \
\
                           "ld1 {v9.8h, v10.8h}, [x2], #32\n" \
                           "st1 {v9.8h, v10.8h}, [x3], #32\n" \
                           : [outPtr] "+r"(outPtr) \
                           : [inPtr] "r"(inPtr) \
                           : "x2", "x3", "v9", "v10", "cc"); \
    } while (0)

inline void TransTensor4DTo5DUint8_C3_neon(uint64_t inPtr, uint64_t outPtr)
{
    __asm__ __volatile("mov x2, %[inPtr]\n"
                       "mov x3, %[outPtr]\n"
                       "mov x5, %[outPtr]\n"
                       "mov x4, #1\n"

                       "movi   v13.8h, #0x0\n"
                       "movi   v14.8h, #0x0\n"
                       "st1 {v13.8h, v14.8h}, [x5]\n"

                       "ld1 {v9.b}[0], [x2], x4\n"
                       "st1 {v9.b}[0], [x3], x4\n"
                       "ld1 {v9.b}[0], [x2], x4\n"
                       "st1 {v9.b}[0], [x3], x4\n"
                       "ld1 {v9.b}[0], [x2], x4\n"
                       "st1 {v9.b}[0], [x3], x4\n"
                       : [outPtr] "+r"(outPtr)
                       : [inPtr] "r"(inPtr)
                       : "x2", "x3", "x4", "x5", "v9", "v13", "v14", "v18", "s17", "cc");
}

#define TransTensor5DTo4DUint8_C3_neon(inPtr, outPtr) \
    do { \
        __asm__ __volatile("mov x2, %[inPtr]\n" \
                           "mov x3, %[outPtr]\n" \
                           "mov x4, #1\n" \
\
                           "ld1 {v9.b}[0], [x2], x4\n" \
                           "st1 {v9.b}[0], [x3], x4\n" \
                           "ld1 {v9.b}[0], [x2], x4\n" \
                           "st1 {v9.b}[0], [x3], x4\n" \
                           "ld1 {v9.b}[0], [x2], x4\n" \
                           "st1 {v9.b}[0], [x3], x4\n" \
                           : [outPtr] "+r"(outPtr) \
                           : [inPtr] "r"(inPtr) \
                           : "x2", "x3", "x4", "x5", "v9", "v13", "v14", "v18", "s17", "cc"); \
    } while (0);

static Status TransTensorNHWCToNC1HWC0_C3_neon(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y)
{
    (void)(yDesc);
    uint32_t n = static_cast<uint32_t>(xDesc.dim[0]);
    uint32_t h = static_cast<uint32_t>(xDesc.dim[1]);
    uint32_t w = static_cast<uint32_t>(xDesc.dim[2]);

    uint32_t srcStride = 3 * sizeof(float);
    uint32_t dstStride = CC_CUBE_SIZE * sizeof(uint16_t);
    uint64_t outPtr = reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(y));
    uint64_t inPtr = reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(x));
    FMK_UINT32_MULCHECK(n, h);
    FMK_UINT32_MULCHECK(n * h, w);
    FMK_UINT32_MULCHECK(n * h * w, dstStride);
    CHECK_ONLY_RET((memset_s(y,
        static_cast<uint64_t>(static_cast<uint64_t>(n) * static_cast<uint64_t>(h) *
        static_cast<uint64_t>(w) * CC_CUBE_SIZE * sizeof(uint16_t)) == SUCCESS, 0,
        static_cast<uint64_t>(static_cast<uint64_t>(n) * static_cast<uint64_t>(h) *
        static_cast<uint64_t>(w) * CC_CUBE_SIZE * sizeof(uint16_t)))),
        FAILED);

    for (uint32_t i = 0; i < n * h * w; i++) {
        TransTensorNHWCToHC1HWC0_C3_neon(inPtr, outPtr);
        Uint64_addCheck(inPtr, static_cast<uint64_t>(srcStride));
        Uint64_addCheck(outPtr, static_cast<uint64_t>(dstStride));
        inPtr = inPtr + srcStride;
        outPtr = outPtr + dstStride;
    }
    return SUCCESS;
}

static Status TransHalfToFloat(uint32_t c1Align, uint32_t num, uint64_t& inPtr, uint64_t& outPtr)
{
    fp16_t fp;
    for (uint32_t k = 0; k < num; k++) {
        for (uint32_t i = 0; i < c1Align; i++) {
            fp = *(reinterpret_cast<fp16_t*>(static_cast<uintptr_t>(inPtr)));
            *(reinterpret_cast<float*>(static_cast<uintptr_t>(outPtr))) = fp.toFloat();
            Uint64_addCheck(inPtr, sizeof(uint16_t));
            Uint64_addCheck(outPtr, sizeof(float));
            inPtr = inPtr + sizeof(uint16_t);
            outPtr = outPtr + sizeof(float);
        }
        Uint64_addCheck(
            inPtr, static_cast<uint64_t>(sizeof(uint16_t)) * (CC_CUBE_SIZE - static_cast<uint64_t>(c1Align)));
        inPtr = inPtr + static_cast<uint64_t>(sizeof(uint16_t)) * (CC_CUBE_SIZE - static_cast<uint64_t>(c1Align));
    }
    return SUCCESS;
}

static Status TransTensorNC1HWC0ToNHWC_neon(
    const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, const void* y)
{
    uint32_t n = static_cast<uint32_t>(xDesc.dim[0]);
    uint32_t c = static_cast<uint32_t>(xDesc.dim[1]);
    uint32_t h = static_cast<uint32_t>(xDesc.dim[2]);
    uint32_t w = static_cast<uint32_t>(xDesc.dim[3]);
    uint32_t cy = static_cast<uint32_t>(yDesc.dim[3]);
    uint32_t hy = static_cast<uint32_t>(yDesc.dim[1]);
    uint32_t wy = static_cast<uint32_t>(yDesc.dim[2]);
    uint32_t c0 = CC_CUBE_SIZE;
    uint32_t c1 = static_cast<uint32_t>(CEIL(c, c0));
    uint32_t c1Align = cy % 16;

    uint32_t srcStride = static_cast<uint32_t>(CC_CUBE_SIZE) * sizeof(uint16_t);
    uint32_t srcNStride = static_cast<uint32_t>(CC_CUBE_SIZE) * c1 * h * w * sizeof(uint16_t);
    uint32_t dstStride = static_cast<uint32_t>(cy) * sizeof(float);
    FMK_UINT32_MULCHECK(cy * hy * wy,
        static_cast<uint32_t>(sizeof(float))); /* only check (cy * hy * wy)*sizeof(float),n*h*c*w is checked befor */
    uint32_t dstNStride = static_cast<uint32_t>(cy) * hy * wy * sizeof(float);
    uint32_t dstC1Stride = static_cast<uint32_t>(CC_CUBE_SIZE) * sizeof(float);
    uint64_t outPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(y));
    uint64_t inPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(x));
    fp16_t fp;

    for (uint32_t x1 = 0; x1 < n; x1++) {
        inPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(x)) +
            static_cast<uint64_t>(srcNStride) * static_cast<uint64_t>(x1);
        for (uint32_t x2 = 0; x2 < c1 - 1; x2++) {
            outPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(y)) +
                static_cast<uint64_t>(dstNStride) * static_cast<uint64_t>(x1) +
                static_cast<uint64_t>(dstC1Stride) * static_cast<uint64_t>(x2);
            for (uint32_t i = 0; i < h * w; i++) {
                TransTensorHalfToFloat16_neon(inPtr, outPtr);
                Uint64_addCheck(inPtr, static_cast<uint64_t>(srcStride));
                Uint64_addCheck(outPtr, static_cast<uint64_t>(dstStride));
                inPtr = inPtr + srcStride;
                outPtr = outPtr + dstStride;
            }
        }
        outPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(y)) +
            static_cast<uint64_t>(dstNStride) * static_cast<uint64_t>(x1) +
            static_cast<uint64_t>(dstC1Stride) * (static_cast<uint64_t>(c1) - 1);
        if (c1Align == 0) {
            for (uint32_t j = 0; j < h * w; j++) {
                TransTensorHalfToFloat16_neon(inPtr, outPtr);
                Uint64_addCheck(inPtr, static_cast<uint64_t>(srcStride));
                Uint64_addCheck(outPtr, static_cast<uint64_t>(dstStride));
                inPtr = inPtr + srcStride;
                outPtr = outPtr + dstStride;
            }
        } else if (c1Align == 3) {
            uint32_t align = c1Align;
            for (uint32_t i = 0; i < h * w; i++) {
                TransTensorHalfToFloat3_neon(inPtr, outPtr);
                Uint64_addCheck(inPtr, static_cast<uint64_t>(srcStride));
                Uint64_addCheck(outPtr, static_cast<uint64_t>(align * sizeof(float)));
                inPtr = inPtr + srcStride;
                outPtr = outPtr + align * sizeof(float);
            }
        } else if (TransHalfToFloat(c1Align, h * w, inPtr, outPtr) != SUCCESS) {
            FMK_LOGE("TransHalfToFloat failed!");
            return FAILED;
        }
    }
    return SUCCESS;
}

static Status TransTensorNC1HWC0ToNHWCUint8_neon(
    const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, const void* y)
{
    uint32_t n = static_cast<uint32_t>(xDesc.dim[0]);
    uint32_t c = static_cast<uint32_t>(xDesc.dim[1]);
    uint32_t h = static_cast<uint32_t>(xDesc.dim[2]);
    uint32_t w = static_cast<uint32_t>(xDesc.dim[3]);
    uint32_t cy = static_cast<uint32_t>(yDesc.dim[3]);
    uint32_t hy = static_cast<uint32_t>(yDesc.dim[1]);
    uint32_t wy = static_cast<uint32_t>(yDesc.dim[2]);
    uint32_t c0 = CC_CUBE_SIZE * 2;
    uint32_t c1 = static_cast<uint32_t>(CEIL(c, c0));
    uint32_t c1Align = cy % 32;

    uint32_t srcStride = static_cast<uint32_t>(CC_CUBE_SIZE) * 2 * sizeof(uint8_t);
    uint32_t srcNStride = static_cast<uint32_t>(CC_CUBE_SIZE) * 2 * c1 * h * w * sizeof(uint8_t);
    uint32_t dstStride = static_cast<uint32_t>(cy) * sizeof(uint8_t);
    uint32_t dstNStride = static_cast<uint32_t>(cy) * hy * wy * sizeof(uint8_t);
    uint32_t dstC1Stride = static_cast<uint32_t>(CC_CUBE_SIZE) * 2 * sizeof(uint8_t);
    uint64_t outPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(y));
    uint64_t inPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(x));

    for (uint32_t x1 = 0; x1 < n; x1++) {
        inPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(x)) +
            static_cast<uint64_t>(srcNStride) * static_cast<uint64_t>(x1);
        for (uint32_t x2 = 0; x2 < c1 - 1; x2++) {
            outPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(y)) +
                static_cast<uint64_t>(dstNStride) * static_cast<uint64_t>(x1) +
                static_cast<uint64_t>(dstC1Stride) * static_cast<uint64_t>(x2);
            for (uint32_t i = 0; i < h * w; i++) {
                TransTensorUint8ToUint8_neon(inPtr, outPtr);
                Uint64_addCheck(inPtr, static_cast<uint64_t>(srcStride));
                Uint64_addCheck(outPtr, static_cast<uint64_t>(dstStride));
                inPtr = inPtr + srcStride;
                outPtr = outPtr + dstStride;
            }
        }
        outPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(y)) +
            static_cast<uint64_t>(dstNStride) * static_cast<uint64_t>(x1) +
            static_cast<uint64_t>(dstC1Stride) * (static_cast<uint64_t>(c1) - 1);
        if (c1Align == 0) {
            for (uint32_t j = 0; j < h * w; j++) {
                TransTensorUint8ToUint8_neon(inPtr, outPtr);
                Uint64_addCheck(inPtr, static_cast<uint64_t>(srcStride));
                Uint64_addCheck(outPtr, static_cast<uint64_t>(dstStride));
                inPtr = inPtr + srcStride;
                outPtr = outPtr + dstStride;
            }
        } else if (c1Align == 3) {
            uint32_t align = c1Align;
            for (uint32_t i = 0; i < h * w; i++) {
                TransTensor5DTo4DUint8_C3_neon(inPtr, outPtr);
                Uint64_addCheck(inPtr, static_cast<uint64_t>(srcStride));
                Uint64_addCheck(outPtr, align * sizeof(uint8_t));
                inPtr = inPtr + srcStride;
                outPtr = outPtr + align * sizeof(uint8_t);
            }
        } else {
            for (uint32_t j = 0; j < h * w; j++) {
                for (uint32_t i = 0; i < c1Align; i++) {
                    if (inPtr != 0 && outPtr != 0) {
                        uint8_t fp = *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(inPtr));
                        *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(outPtr)) = fp;
                    }
                    Uint64_addCheck(inPtr, sizeof(uint8_t));
                    Uint64_addCheck(outPtr, sizeof(uint8_t));
                    inPtr = inPtr + sizeof(uint8_t);
                    outPtr = outPtr + sizeof(uint8_t);
                }
                uint32_t align = 2;
                Uint64_addCheck(inPtr, sizeof(uint8_t) * (CC_CUBE_SIZE * align - c1Align));
                inPtr = inPtr + sizeof(uint8_t) * (CC_CUBE_SIZE * align - c1Align);
            }
        }
    }
    return SUCCESS;
}

static Status TransTensorNHWCToNC1HWC0Uint8_C3_neon(
    const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, const void* y)
{
    (void)(yDesc);
    uint32_t n = static_cast<uint32_t>(xDesc.dim[0]);
    uint32_t h = static_cast<uint32_t>(xDesc.dim[1]);
    uint32_t w = static_cast<uint32_t>(xDesc.dim[2]);
    uint32_t c0 = CC_CUBE_SIZE * 2;
    uint32_t srcStride = 3 * static_cast<uint32_t>(sizeof(uint8_t));
    uint32_t dstStride = c0 * static_cast<uint32_t>(sizeof(uint8_t));
    uint64_t outPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(y));
    uint64_t inPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(x));

    for (uint32_t i = 0; i < n * h * w; i++) {
        TransTensor4DTo5DUint8_C3_neon(inPtr, outPtr);

        Uint64_addCheck(inPtr, static_cast<uint64_t>(srcStride));
        Uint64_addCheck(outPtr, static_cast<uint64_t>(dstStride));
        inPtr = inPtr + srcStride;
        outPtr = outPtr + dstStride;
    }
    return SUCCESS;
}

#endif

/*
 * @ingroup dnn
 * @brief get the size of data type in device
 * @param [in] dataType   data type in device
 * @param [in|out] size   size of data type
 * @return Status
 */
static Status GetDataTypeSize(DataType_t dataType, uint32_t& size)
{
    struct TYPE_SIZE {
        DataType_t ccType;
        uint8_t size;
    };
    using TypeSize_t = struct TYPE_SIZE;

    TypeSize_t TypeSize[CC_DATA_RESERVED] = {
        {CC_DATA_FLOAT, sizeof(float)},
        {CC_DATA_HALF_UINT16_PROPOSAL, sizeof(fp16_t)},
        {CC_DATA_HALF, sizeof(fp16_t)},
        {CC_DATA_INT8, sizeof(int8_t)},
        {CC_DATA_INT32, sizeof(int32_t)},
        {CC_DATA_UINT8, sizeof(uint8_t)},
        {CC_DATA_UINT32, sizeof(uint32_t)},
        {CC_DATA_INT16, sizeof(int16_t)},
        {CC_DATA_UINT16, sizeof(uint16_t)},
        {CC_DATA_INT64, sizeof(int64_t)},
        {CC_DATA_UINT64, sizeof(uint64_t)},
        {CC_DATA_DOUBLE, sizeof(double)},
        {CC_DATA_BOOL, sizeof(uint8_t)},
        {CC_DATA_DUAL, sizeof(fp16_t) + sizeof(int8_t)},
        {CC_DATA_DUAL_SUB_UINT8, sizeof(int8_t)},
        {CC_DATA_DUAL_SUB_INT8, sizeof(int8_t)},
        {CC_DATA_QINT8, sizeof(int8_t)},
        {CC_DATA_QUINT8, sizeof(uint8_t)},
        {CC_DATA_QINT16, sizeof(int16_t)},
        {CC_DATA_QUINT16, sizeof(uint16_t)},
        {CC_DATA_QINT32, sizeof(int32_t)},
        {CC_DATA_2BITS, sizeof(int8_t)},
    };
    int i = 0;
    size = 0;
    for (i = 0; i < CC_DATA_RESERVED; i++) {
        if (TypeSize[i].ccType == dataType) {
            size = TypeSize[i].size;
            break;
        }
    }
    if (dataType == CC_DATA_2BITS) {
        FMK_LOGD("dataType is CC_DATA_2BITS: dataSize is sizeof(int8_t)!");
    }
    if ((i >= CC_DATA_RESERVED) || (size == 0)) {
        FMK_LOGD("dataType: %d not support!", dataType);
        return FAILED;
    }
    return SUCCESS;
};

static Status SetNCHWTensorDimAndCalcCount(
    ccTensor_t& tensorDesc, std::vector<int32_t>& dims, int32_t& elementCnt)
{
    int32_t productNCHW = 0;
    int32_t operandsNCHW[4] = {dims[NCHW_DIM_N], dims[NCHW_DIM_C], dims[NCHW_DIM_H], dims[NCHW_DIM_W]};
    tensorDesc.dim[0] = dims[NCHW_DIM_N];
    tensorDesc.dim[1] = dims[NCHW_DIM_C];
    tensorDesc.dim[2] = dims[NCHW_DIM_H];
    tensorDesc.dim[3] = dims[NCHW_DIM_W];
    // check overflow
    if (CheckInt32ArrayMulOverflow(operandsNCHW, 4, &productNCHW) != SUCCESS) {
        FMK_LOGD("Integer multiplication can result in overflow!");
        return FAILED;
    }
    if (productNCHW > MAX_TENSOR_ELEMENT_COUNT) {
        FMK_LOGD("The tensor element count %d is too large!", productNCHW);
        return FAILED;
    }
    elementCnt = productNCHW;
    return SUCCESS;
}

static Status SetNHWCTensorDimAndCalcCount(
    ccTensor_t& tensorDesc, std::vector<int32_t>& dims, int32_t& elementCnt)
{
    tensorDesc.dim[0] = dims[NCHW_DIM_N];
    tensorDesc.dim[1] = dims[NCHW_DIM_H];
    tensorDesc.dim[2] = dims[NCHW_DIM_W];
    tensorDesc.dim[3] = dims[NCHW_DIM_C];
    int32_t productNHWC = 0;
    int32_t operandsNHWC[4] = {dims[NCHW_DIM_N], dims[NCHW_DIM_H], dims[NCHW_DIM_W], dims[NCHW_DIM_C]};
    // check overflow
    if (CheckInt32ArrayMulOverflow(operandsNHWC, 4, &productNHWC) != SUCCESS) {
        FMK_LOGD("Integer multiplication can result in overflow!");
        return FAILED;
    }
    if (productNHWC > MAX_TENSOR_ELEMENT_COUNT) {
        FMK_LOGD("The tensor element count %d is too large!", productNHWC);
        return FAILED;
    }
    elementCnt = productNHWC;
    return SUCCESS;
}

static Status SetNC1HWC0TensorDimAndCalcCount(
    ccTensor_t& tensorDesc, DataType_t dataType, std::vector<int32_t>& dims, int32_t& elementCnt)
{
    int32_t c0 =
        ((dataType == CC_DATA_BOOL) || (dataType == CC_DATA_INT8) || (dataType == CC_DATA_UINT8) ||
            (dataType == CC_DATA_DUAL_SUB_UINT8) || (dataType == CC_DATA_QINT8) || (dataType == CC_DATA_QUINT8) ||
            (dataType == CC_DATA_DUAL_SUB_INT8) || (dataType == CC_DATA_2BITS)) ?
        CC_INT8_C0_SIZE :
        CC_CUBE_SIZE;
    int32_t c1 = static_cast<int32_t>(std::ceil(dims[NCHW_DIM_C] * 1.0 / c0));
    int32_t productNC1HWC0 = 0;
    int32_t operandsNC1HWC0[5] = {dims[NCHW_DIM_N], c1, dims[NCHW_DIM_H], dims[NCHW_DIM_W], c0};
    tensorDesc.dim[0] = dims[NCHW_DIM_N];
    tensorDesc.dim[1] = dims[NCHW_DIM_C];
    tensorDesc.dim[2] = dims[NCHW_DIM_H];
    tensorDesc.dim[3] = dims[NCHW_DIM_W];
    // check overflow
    if (CheckInt32ArrayMulOverflow(operandsNC1HWC0, 5, &productNC1HWC0) != SUCCESS) {
        FMK_LOGD("Integer multiplication can result in overflow!");
        return FAILED;
    }
    if (productNC1HWC0 > MAX_TENSOR_ELEMENT_COUNT) {
        FMK_LOGD("The tensor element count %d is too large!", productNC1HWC0);
        return FAILED;
    }
    elementCnt = productNC1HWC0;
    return SUCCESS;
}

static Status SetTensor4dDescriptor(
    ccTensor_t& tensorDesc, Format format, DataType_t dataType, std::vector<int32_t>& dims)
{
    if ((dims[NCHW_DIM_N] <= 0) || (dims[NCHW_DIM_C] <= 0) || (dims[NCHW_DIM_H] <= 0) || (dims[NCHW_DIM_W] <= 0)) {
        FMK_LOGD("Parameter is bad: n=%d, c=%d, h=%d, w==%d!",
            dims[NCHW_DIM_N], dims[NCHW_DIM_C], dims[NCHW_DIM_H], dims[NCHW_DIM_W]);
        return FAILED;
    }

    tensorDesc.dataType = dataType;
    tensorDesc.dimCnt = CC_4D_FORMAT_DIMCNT;
    tensorDesc.realDimCnt = -1;
    tensorDesc.format = format;
    // calc data type size
    uint32_t dataTypeSize = 0;
    Status ret = GetDataTypeSize(dataType, dataTypeSize);
    CHECK((ret == SUCCESS), ret, "GetDataTypeSize failed, ret is %d!", ret);

    tensorDesc.dataSize = dataTypeSize;
    int32_t elementCnt = 1;
    switch (format) {
        case FORMAT_NCHW:
            ret = SetNCHWTensorDimAndCalcCount(tensorDesc, dims, elementCnt);
            CHECK_ONLY_RET((ret == SUCCESS), ret);
            break;
        case FORMAT_NHWC:
            ret = SetNHWCTensorDimAndCalcCount(tensorDesc, dims, elementCnt);
            CHECK_ONLY_RET((ret == SUCCESS), ret);
            break;
        case FORMAT_NC1HWC0:
        case FORMAT_C1HWNC0:
            ret = SetNC1HWC0TensorDimAndCalcCount(tensorDesc, dataType, dims, elementCnt);
            CHECK_ONLY_RET((ret == SUCCESS), ret);
            break;
        default:
            FMK_LOGE("format %d is not supported!", format);
            return FAILED;
    }
    // the stride of the last dimension is 1
    tensorDesc.stride[3] = 1;
    // stride[i] is equal to dim[i+1]*stride[i+1]
    tensorDesc.stride[2] = (tensorDesc.dim[3]) * (tensorDesc.stride[3]);
    FMK_INT32_MULCHECK(tensorDesc.dim[2], tensorDesc.stride[2]);
    tensorDesc.stride[1] = (tensorDesc.dim[2]) * (tensorDesc.stride[2]);
    FMK_INT32_MULCHECK(tensorDesc.dim[1], tensorDesc.stride[1]);
    tensorDesc.stride[0] = (tensorDesc.dim[1]) * (tensorDesc.stride[1]);
    FMK_UINT32_MULCHECK(tensorDesc.dataSize, static_cast<uint32_t>(elementCnt));
    tensorDesc.dataSize *= static_cast<uint32_t>(elementCnt);
    return SUCCESS;
}

static Status GetTensor4dDescriptor(const ccTensor_t& tensorDesc, DataType_t& dataType, int32_t& n, int32_t& c,
    int32_t& h, int32_t& w, int32_t& nStride, int32_t& cStride, int32_t& hStride, int32_t& wStride)
{
    int32_t dimCnt = tensorDesc.dimCnt;
    if (dimCnt != CC_4D_FORMAT_DIMCNT) {
        FMK_LOGD("tensor dimcnt: %d != %d!", dimCnt, CC_4D_FORMAT_DIMCNT);
        return FAILED;
    }
    dataType = tensorDesc.dataType;
    switch (tensorDesc.format) {
        case FORMAT_NCHW:
        case FORMAT_NC1HWC0:
        case FORMAT_C1HWNC0:
        case FORMAT_NHWC1C0:
        case FORMAT_NC1C0HWPAD:
        case FORMAT_FSR_NCHW:
        case FORMAT_FRACTAL_Z:
        case FORMAT_FRACTAL_Z_C04:
            n = tensorDesc.dim[0];
            c = tensorDesc.dim[1];
            h = tensorDesc.dim[2];
            w = tensorDesc.dim[3];
            nStride = tensorDesc.stride[0];
            cStride = tensorDesc.stride[1];
            hStride = tensorDesc.stride[2];
            wStride = tensorDesc.stride[3];
            break;
        case FORMAT_NHWC:
            n = tensorDesc.dim[0];
            h = tensorDesc.dim[1];
            w = tensorDesc.dim[2];
            c = tensorDesc.dim[3];
            nStride = tensorDesc.stride[0];
            hStride = tensorDesc.stride[1];
            wStride = tensorDesc.stride[2];
            cStride = tensorDesc.stride[3];
            break;
        default:
            FMK_LOGD("format %d is not supported!", tensorDesc.format);
            return FAILED;
    }
    return SUCCESS;
};

/* for ND case: suppoort 5D now. */
static Status SetNDTensorDescriptor(
    ccTensor_t& tensorDesc, Format format, DataType_t dataType, std::vector<int32_t>& dims)
{
    if (((format != FORMAT_NCDHW) && (format != FORMAT_NDHWC)) || (dims.size() != 5)) {
        FMK_LOGE("para is error. format: %d, dimSize: %zu", static_cast<int32_t>(format), dims.size());
        return FAILED;
    }
    // set ND info
    tensorDesc.dataType = dataType;
    tensorDesc.dimCnt = static_cast<int32_t>(dims.size());
    tensorDesc.realDimCnt = -1;
    tensorDesc.format = format;
    tensorDesc.dim[0] = dims[0];
    tensorDesc.dim[1] = dims[1];
    tensorDesc.dim[2] = dims[2];
    tensorDesc.dim[3] = dims[3];
    tensorDesc.dim[4] = dims[4];
    // calc data type size
    uint32_t dataTypeSize = 0;
    Status ret = GetDataTypeSize(dataType, dataTypeSize);
    if (ret != SUCCESS) {
        FMK_LOGE("GetDataTypeSize failed, ret is %d!", ret);
        return ret;
    }
    tensorDesc.dataSize = dataTypeSize;

    int32_t elementCnt = 0;
    // check overflow
    if (CheckInt32ArrayMulOverflow(dims.data(), static_cast<int32_t>(dims.size()), &elementCnt) != SUCCESS) {
        FMK_LOGE("Integer multiplication can result in overflow!");
        return FAILED;
    }
    if (static_cast<int>(elementCnt) > MAX_TENSOR_ELEMENT_COUNT) {
        FMK_LOGE("The tensor element count %d is too large!", elementCnt);
        return FAILED;
    }
    FMK_UINT32_MULCHECK(tensorDesc.dataSize, static_cast<uint32_t>(elementCnt));
    tensorDesc.dataSize *= static_cast<uint32_t>(elementCnt);
    return SUCCESS;
}

static Status SetTensorNdDescriptor(ccTensor_t& tensorDesc, DataType_t dataType, int32_t dimCnt, int32_t dimA[])
{
    if ((dimCnt > 0) && (dimA == nullptr)) {
        FMK_LOGD("when dimCnt > 0, requires dimA not be nullptr!");
        return FAILED;
    }

    if ((dimCnt < 0) || (dimCnt > CC_DIM_MAX)) {
        FMK_LOGD("dimCnt: %d is not support!", dimCnt);
        return FAILED;
    }

    tensorDesc.dataType = dataType;
    tensorDesc.dimCnt = dimCnt;
    tensorDesc.format = FORMAT_ND;

    // calc data type size
    uint32_t dataTypeSize = 0;
    Status ret = GetDataTypeSize(dataType, dataTypeSize);
    if (ret != SUCCESS) {
        FMK_LOGD("GetDataTypeSize failed, ret is %d!", ret);
        return ret;
    }
    tensorDesc.dataSize = dataTypeSize;

    int32_t elementCnt = 1;
    for (int32_t i = 0; i < tensorDesc.dimCnt; i++) {
        if (dimA[i] <= 0) {
            FMK_LOGD("found illegal data in dimA!");
            return FAILED;
        }
        tensorDesc.dim[i] = dimA[i];
        FMK_INT32_MULCHECK(elementCnt, tensorDesc.dim[i]);
        elementCnt *= tensorDesc.dim[i];
    }

    if (elementCnt > MAX_TENSOR_ELEMENT_COUNT) {
        FMK_LOGD("The tensor element count %d is too large!", elementCnt);
        return FAILED;
    }

    FMK_INT32_MULCHECK(static_cast<int32_t>(tensorDesc.dataSize), static_cast<int32_t>(elementCnt));
    tensorDesc.dataSize *= static_cast<uint32_t>(elementCnt);

    // compute stride
    if (dimCnt == 0) {
        return SUCCESS;
    }

    if (dimCnt == 1) {
        tensorDesc.stride[0] = 1;
        return SUCCESS;
    }

    // the stride of the last dimension is 1
    tensorDesc.stride[dimCnt - 1] = 1;
    for (int32_t i = tensorDesc.dimCnt - 2; i >= 0; --i) {
        FMK_INT32_MULCHECK(
            tensorDesc.dim[static_cast<uint64_t>(static_cast<uint32_t>(i)) + 1],
            tensorDesc.stride[static_cast<uint64_t>(static_cast<uint32_t>(i)) + 1]);
        tensorDesc.stride[i] = static_cast<int32_t>(
            static_cast<uint64_t>(static_cast<uint32_t>(tensorDesc.dim[i + 1])) *
            static_cast<uint64_t>(
                static_cast<uint32_t>(tensorDesc.stride[static_cast<uint64_t>(static_cast<uint32_t>(i)) + 1])));
    }
    return SUCCESS;
};

static Status GetTensorNdDescriptor(const ccTensor_t& tensorDesc, uint32_t arrLength, DataType_t& dataType,
    int32_t& dimCnt, int32_t dimA[], int32_t strideA[])
{
    if ((dimA == nullptr) || (strideA == nullptr)) {
        FMK_LOGD("input param is nullptr!");
        return FAILED;
    }

    if (tensorDesc.format != FORMAT_ND && !IsHashTableTensorFormat(tensorDesc.format)) {
        FMK_LOGD("foramt: %d is illegal!", tensorDesc.format);
        return FAILED;
    }

    dataType = tensorDesc.dataType;
    dimCnt = tensorDesc.dimCnt;

    CHECK(static_cast<uint32_t>(dimCnt) <= arrLength, FAILED, "dimCntReq are not allowed to exceed %u.", arrLength);
    for (int32_t i = 0; i < dimCnt; i++) {
        dimA[i] = tensorDesc.dim[i];
        strideA[i] = tensorDesc.stride[i];
    }
    return SUCCESS;
}

static Status GetTensorMemorySizeInBytes(const ccTensor_t& tensorDesc, uint32_t& size)
{
    FMK_UINT32_ADDCHECK(tensorDesc.dataSize, 2 * DATA_MEMORY_ALIGN_SIZE - 1);

    FMK_UINT32_MULCHECK(
        (tensorDesc.dataSize + 2 * DATA_MEMORY_ALIGN_SIZE - 1) / DATA_MEMORY_ALIGN_SIZE, DATA_MEMORY_ALIGN_SIZE);

    size = ((tensorDesc.dataSize + 2 * DATA_MEMORY_ALIGN_SIZE - 1) / DATA_MEMORY_ALIGN_SIZE) * DATA_MEMORY_ALIGN_SIZE;
    return SUCCESS;
};

static Status GetDataTypeTransModeFunc0(const DataType_t xType, DataTypeTransMode_t& dataTypeTransmode)
{
    if (xType == CC_DATA_HALF) {
        dataTypeTransmode = CC_DATATYPE_TRANS_FP16_NO_TRANS;
    } else if (xType == CC_DATA_FLOAT) {
        dataTypeTransmode = CC_DATATYPE_TRANS_FLOAT_NO_TRANS;
    } else if (xType == CC_DATA_INT8) {
        dataTypeTransmode = CC_DATATYPE_TRANS_INT8_NO_TRANS;
    } else if (xType == CC_DATA_INT32) {
        dataTypeTransmode = CC_DATATYPE_TRANS_INT32_NO_TRANS;
    } else if (xType == CC_DATA_INT64) {
        dataTypeTransmode = CC_DATATYPE_TRANS_INT64_NO_TRANS;
    } else if ((xType == CC_DATA_UINT8) || (xType == CC_DATA_BOOL) || (xType == CC_DATA_QUINT8)) {
        dataTypeTransmode = CC_DATATYPE_TRANS_UINT8_NO_TRANS;
    } else {
        FMK_LOGD("TransDataType %d is not supported!", xType);
        return FAILED;
    }
    return SUCCESS;
}

static Status GetDataTypeTransMode(
    const DataType_t xType, const DataType_t yType, DataTypeTransMode_t& dataTypeTransmode)
{
    if (xType == yType) {
        return GetDataTypeTransModeFunc0(xType, dataTypeTransmode);
    } else if ((xType == CC_DATA_FLOAT) && (yType == CC_DATA_HALF)) {
        dataTypeTransmode = CC_DATATYPE_TRANS_FLOAT_TO_FP16;
    } else if ((xType == CC_DATA_HALF) && (yType == CC_DATA_FLOAT)) {
        dataTypeTransmode = CC_DATATYPE_TRANS_FP16_TO_FLOAT;
    } else if ((xType == CC_DATA_UINT8 || xType == CC_DATA_QUINT8) && (yType == CC_DATA_FLOAT)) {
        dataTypeTransmode = CC_DATATYPE_TRANS_UINT8_TO_FLOAT;
    } else if ((xType == CC_DATA_INT8) && (yType == CC_DATA_FLOAT)) {
        dataTypeTransmode = CC_DATATYPE_TRANS_INT8_TO_FLOAT;
    } else {
        FMK_LOGD("TransDataType from %d to %d is not supported!", xType, yType);
        return FAILED;
    }
    return SUCCESS;
}

static Status TransDataNCHWToNC1HWC0(
    uint32_t n, uint32_t c, uint32_t h, uint32_t w, const void* x, void* y, DataTypeTransMode_t dataTypeTransmode)
{
    uint32_t c0 = ((dataTypeTransmode == CC_DATATYPE_TRANS_UINT8_NO_TRANS) ||
                      (dataTypeTransmode == CC_DATATYPE_TRANS_INT8_NO_TRANS)) ?
        CC_CUBE_SIZE * 2 :
        CC_CUBE_SIZE;
    uint32_t c1 = static_cast<uint32_t>(std::ceil(c * 1.0 / c0));
    uint32_t hw = h * w;
    uint32_t chw = c * h * w;
    uint32_t c1hwc0 = c1 * hw * c0;
    uint32_t hwc0 = hw * c0;
    uint32_t wc0 = w * c0;
    const fp16_t fpZero = 0;

    for (uint32_t nIdx = 0; nIdx < n; nIdx++) {
        /* head address of n dimension */
        uint32_t nFstAddr = nIdx * c1hwc0;
        for (uint32_t c1Idx = 0; c1Idx < c1; c1Idx++) {
            /* head address of c1 dimension */
            uint32_t c1FstAddr = nFstAddr + c1Idx * hwc0;
            for (uint32_t hIdx = 0; hIdx < h; hIdx++) {
                /* head address of h dimension */
                uint32_t hFstAddr = c1FstAddr + hIdx * wc0;
                for (uint32_t wIdx = 0; wIdx < w; wIdx++) {
                    /* head address of w dimension */
                    uint32_t wFstAddr = hFstAddr + wIdx * c0;
                    for (uint32_t c0Idx = 0; c0Idx < c0; c0Idx++) {
                        /* address of c0 dimension */
                        uint32_t index = c0Idx + wFstAddr;
                        /* head address of c dimension */
                        uint32_t cIdx = c0Idx + c1Idx * c0;
                        /* address of w dimension */
                        uint32_t srcIdx = nIdx * chw + cIdx * hw + hIdx * w + wIdx;
                        switch (dataTypeTransmode) {
                            case CC_DATATYPE_TRANS_FLOAT_TO_FP16:
                                if (cIdx < c) {
                                    static_cast<fp16_t*>(y)[index] = static_cast<const float*>(x)[srcIdx];
                                } else {
                                    static_cast<fp16_t*>(y)[index] = fpZero;
                                }
                                break;
                            case CC_DATATYPE_TRANS_INT32_NO_TRANS:
                                static_cast<uint32_t*>(y)[index] =
                                    (cIdx < c) ? static_cast<const uint32_t*>(x)[srcIdx] : 0;
                                break;
                            case CC_DATATYPE_TRANS_FLOAT_NO_TRANS:
                                static_cast<float*>(y)[index] =
                                    (cIdx < c) ? static_cast<const float*>(x)[srcIdx] : 0.0f;
                                break;
                            case CC_DATATYPE_TRANS_FP16_NO_TRANS:
                                static_cast<fp16_t*>(y)[index] =
                                    (cIdx < c) ? static_cast<const fp16_t*>(x)[srcIdx] : fpZero;
                                break;
                            case CC_DATATYPE_TRANS_UINT8_NO_TRANS:
                                static_cast<uint8_t*>(y)[index] =
                                    (cIdx < c) ? static_cast<const uint8_t*>(x)[srcIdx] : 0;
                                break;
                            default:
                                static_cast<uint8_t*>(y)[index] =
                                    (cIdx < c) ? static_cast<const uint8_t*>(x)[srcIdx] : 0;
                                break;
                        }
                    }
                }
            }
        }
    }
    return SUCCESS;
}

static Status TransTensorNCHWToNC1HWC0(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y)
{
    if ((xDesc.dim[0] != yDesc.dim[0]) || (xDesc.dim[1] != yDesc.dim[1]) || (xDesc.dim[2] != yDesc.dim[2]) ||
        (xDesc.dim[3] != yDesc.dim[3])) {
        FMK_LOGD("The input and output dims are not equal!");
        return FAILED;
    }
    DataTypeTransMode_t dataTypeTransmode = CC_DATATYPE_TRANS_FLOAT_NO_TRANS;
    if (GetDataTypeTransMode(xDesc.dataType, yDesc.dataType, dataTypeTransmode) != SUCCESS) {
        FMK_LOGD("GetDataTypeTransMode error!");
        return FAILED;
    }
    return TransDataNCHWToNC1HWC0(xDesc.dim[0], xDesc.dim[1], xDesc.dim[2], xDesc.dim[3], x, y, dataTypeTransmode);
}

static Status TransTensorNHWCToNC1HWC0(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y)
{
    CHECK((xDesc.dim[0] == yDesc.dim[0]), FAILED, "The input and output dims are not equal!");
    CHECK((xDesc.dim[1] == yDesc.dim[2]), FAILED, "The input and output dims are not equal!");
    CHECK((xDesc.dim[2] == yDesc.dim[3]), FAILED, "The input and output dims are not equal!");
    CHECK((xDesc.dim[3] == yDesc.dim[1]), FAILED, "The input and output dims are not equal!");

    uint32_t n = static_cast<uint32_t>(xDesc.dim[0]);
    uint32_t h = static_cast<uint32_t>(xDesc.dim[1]);
    uint32_t w = static_cast<uint32_t>(xDesc.dim[2]);
    uint32_t c = static_cast<uint32_t>(xDesc.dim[3]);

    uint32_t c0 = ((xDesc.dataType == CC_DATA_BOOL) || (xDesc.dataType == CC_DATA_UINT8) ||
                      (xDesc.dataType == CC_DATA_INT8) || (xDesc.dataType == CC_DATA_QUINT8)) ?
        CC_CUBE_SIZE * 2 :
        CC_CUBE_SIZE;
    uint32_t c1 = static_cast<uint32_t>(std::ceil(c * 1.0 / c0));
    uint32_t wc = w * c;
    uint32_t hw = h * w;
    uint32_t hwc = hw * c;
    uint32_t c1hwc0 = c1 * hw * c0;
    uint32_t hwc0 = hw * c0;
    uint32_t wc0 = w * c0;
    const fp16_t fpZero = 0;

    DataTypeTransMode_t dataTypeTransmode = CC_DATATYPE_TRANS_FLOAT_NO_TRANS;
    CHECK((GetDataTypeTransMode(xDesc.dataType, yDesc.dataType, dataTypeTransmode) == SUCCESS), FAILED,
        "GetDataTypeTransMode error!");
#ifdef ARM_NEON
    if ((c == 3) && (dataTypeTransmode == CC_DATATYPE_TRANS_FLOAT_TO_FP16)) {
        return TransTensorNHWCToNC1HWC0_C3_neon(xDesc, x, yDesc, y);
    }

    if ((c == 3) && (dataTypeTransmode == CC_DATATYPE_TRANS_UINT8_NO_TRANS)) {
        return TransTensorNHWCToNC1HWC0Uint8_C3_neon(xDesc, x, yDesc, y);
    }
#endif
    // trans
    for (uint32_t n_Idx = 0; n_Idx < n; n_Idx++) {
        /* head address of n dimension */
        uint32_t nFstAddr = n_Idx * c1hwc0;
        for (uint32_t c1Idx = 0; c1Idx < c1; c1Idx++) {
            /* head address of c1 dimension */
            uint32_t c1FstAddr = nFstAddr + c1Idx * hwc0;
            for (uint32_t hIdx = 0; hIdx < h; hIdx++) {
                /* head address of h dimension */
                uint32_t hFstAddr = c1FstAddr + hIdx * wc0;
                for (uint32_t wIdx = 0; wIdx < w; wIdx++) {
                    /* head address of w dimension */
                    uint32_t wFstAddr = hFstAddr + wIdx * c0;
                    for (uint32_t c0Idx = 0; c0Idx < c0; c0Idx++) {
                        /* address of c0 dimension */
                        uint32_t idx = c0Idx + wFstAddr;
                        /* head address of c dimension */
                        uint32_t cIdx = c0Idx + c1Idx * c0;
                        /* address of w dimension */
                        uint32_t srcIdx = n_Idx * hwc + hIdx * wc + wIdx * c + cIdx;
                        switch (dataTypeTransmode) {
                            case CC_DATATYPE_TRANS_FLOAT_TO_FP16:
                                if (cIdx < c) {
                                    static_cast<fp16_t*>(y)[idx] = static_cast<const float*>(x)[srcIdx];
                                } else {
                                    static_cast<fp16_t*>(y)[idx] = fpZero;
                                }
                                break;
                            case CC_DATATYPE_TRANS_INT32_NO_TRANS:
                                static_cast<uint32_t*>(y)[idx] =
                                    (cIdx < c) ? static_cast<const uint32_t*>(x)[srcIdx] : 0;
                                break;
                            case CC_DATATYPE_TRANS_FLOAT_NO_TRANS:
                                static_cast<float*>(y)[idx] = (cIdx < c) ? static_cast<const float*>(x)[srcIdx] : 0.0f;
                                break;
                            case CC_DATATYPE_TRANS_UINT8_NO_TRANS:
                                static_cast<uint8_t*>(y)[idx] = (cIdx < c) ? static_cast<const uint8_t*>(x)[srcIdx] : 0;
                                break;
                            case CC_DATATYPE_TRANS_INT8_NO_TRANS:
                                static_cast<int8_t*>(y)[idx] = (cIdx < c) ? static_cast<const int8_t*>(x)[srcIdx] : 0;
                                break;
                            case CC_DATATYPE_TRANS_FP16_NO_TRANS:
                                static_cast<fp16_t*>(y)[idx] =
                                    (cIdx < c) ? static_cast<const fp16_t*>(x)[srcIdx] : fpZero;
                                break;
                            default:
                                FMK_LOGD(
                                    "TransDataType from %d to %d is not supported!", xDesc.dataType, yDesc.dataType);
                                return FAILED;
                        }
                    }
                }
            }
        }
    }
    return SUCCESS;
}

#define CopyToDstByDatatypetransmode(x, y, srcIdx, dstIdx) \
    do { \
        switch (dataTypeTransmode) { \
            case CC_DATATYPE_TRANS_FP16_TO_FLOAT: \
                fp = static_cast<const fp16_t*>(x)[srcIdx]; \
                static_cast<float*>(y)[dstIdx] = fp; \
                break; \
            case CC_DATATYPE_TRANS_FP16_NO_TRANS: \
                static_cast<fp16_t*>(y)[dstIdx] = static_cast<const fp16_t*>(x)[srcIdx]; \
                break; \
            case CC_DATATYPE_TRANS_UINT8_TO_FLOAT: \
                static_cast<float*>(y)[dstIdx] = static_cast<float>(static_cast<const uint8_t*>(x)[srcIdx]); \
                break; \
            case CC_DATATYPE_TRANS_INT8_TO_FLOAT: \
                static_cast<float*>(y)[dstIdx] = static_cast<float>(static_cast<const int8_t*>(x)[srcIdx]); \
                break; \
            case CC_DATATYPE_TRANS_UINT8_NO_TRANS: \
                static_cast<uint8_t*>(y)[dstIdx] = static_cast<const uint8_t*>(x)[srcIdx]; \
                break; \
            case CC_DATATYPE_TRANS_INT8_NO_TRANS: \
                static_cast<int8_t*>(y)[dstIdx] = static_cast<const int8_t*>(x)[srcIdx]; \
                break; \
            case CC_DATATYPE_TRANS_FLOAT_NO_TRANS: \
                static_cast<float*>(y)[dstIdx] = static_cast<const float*>(x)[srcIdx]; \
                break; \
            case CC_DATATYPE_TRANS_INT32_NO_TRANS: \
                static_cast<int32_t*>(y)[dstIdx] = static_cast<const int32_t*>(x)[srcIdx]; \
                break; \
            case CC_DATATYPE_TRANS_INT64_NO_TRANS: \
                static_cast<int64_t*>(y)[dstIdx] = static_cast<const int64_t*>(x)[srcIdx]; \
                break; \
            default: \
                FMK_LOGD("TransDataType from %d to %d is not supported!", xDesc.dataType, yDesc.dataType); \
                return FAILED; \
        } \
    } while (0)

static Status TransTensorNC1HWC0ToNCHW(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y)
{
    CHECK((xDesc.dim[0] == yDesc.dim[0]), FAILED, "The input and output dims are not equal!");
    CHECK((xDesc.dim[1] == yDesc.dim[1]), FAILED, "The input and output dims are not equal!");
    CHECK((xDesc.dim[2] == yDesc.dim[2]), FAILED, "The input and output dims are not equal!");
    CHECK((xDesc.dim[3] == yDesc.dim[3]), FAILED, "The input and output dims are not equal!");

    uint32_t n = static_cast<uint32_t>(xDesc.dim[0]);
    uint32_t c = static_cast<uint32_t>(xDesc.dim[1]);
    uint32_t h = static_cast<uint32_t>(xDesc.dim[2]);
    uint32_t w = static_cast<uint32_t>(xDesc.dim[3]);
    uint32_t c0 = (xDesc.dataType != CC_DATA_BOOL && xDesc.dataType != CC_DATA_UINT8 &&
                      xDesc.dataType != CC_DATA_INT8 && xDesc.dataType != CC_DATA_QUINT8) ?
        CC_CUBE_SIZE :
        CC_INT8_C0_SIZE;
    uint32_t c1 = (c + c0 - 1) / c0;
    uint32_t hw = w * h;
    uint32_t chw = c * h * w;
    uint32_t c1hwc0 = c1 * hw * c0;
    uint32_t hwc0 = hw * c0;
    uint32_t wc0 = c0 * w;
    fp16_t fp;

    DataTypeTransMode_t dataTypeTransmode = CC_DATATYPE_TRANS_FLOAT_NO_TRANS;
    CHECK((GetDataTypeTransMode(xDesc.dataType, yDesc.dataType, dataTypeTransmode) == SUCCESS), FAILED,
        "GetDataTypeTransMode error!");

    for (uint32_t nIdx = 0; nIdx < n; nIdx++) {
        /* head address of n dimension */
        uint32_t nFstAddr = nIdx * c1hwc0;
        for (uint32_t c1_Idx = 0; c1_Idx < c1; c1_Idx++) {
            /* head address of c1 dimension */
            uint32_t c1FstAddr = nFstAddr + c1_Idx * hwc0;
            for (uint32_t hIdx = 0; hIdx < h; hIdx++) {
                /* head address of h dimension */
                uint32_t hFstAddr = c1FstAddr + hIdx * wc0;
                for (uint32_t wIdx = 0; wIdx < w; wIdx++) {
                    /* head address of w dimension */
                    uint32_t wFstAddr = hFstAddr + wIdx * c0;
                    for (uint32_t c0Idx = 0; c0Idx < c0; c0Idx++) {
                        /* head of c0 dimension */
                        uint32_t idx = c0Idx + wFstAddr;
                        /* head address of c dimension */
                        uint32_t cIdx = c0Idx + c1_Idx * c0;
                        /* address of w dimension */
                        uint32_t dstIdx = nIdx * chw + cIdx * hw + hIdx * w + wIdx;
                        /* filter 0 padded from NCHW to NC1HWC0 */
                        if (cIdx < c) {
                            /* switch source and destination address so as to convert NC1HWC0 back to NCHW */
                            CopyToDstByDatatypetransmode(x, y, idx, dstIdx);
                        }
                    }
                }
            }
        }
    }
    return SUCCESS;
}

static Status TransTensorNHWCToNCHW(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y)
{
    CHECK((yDesc.dim[0] == xDesc.dim[0]), FAILED, "The input and output dims are not equal!");
    CHECK((yDesc.dim[1] == xDesc.dim[3]), FAILED, "The input and output dims are not equal!");
    CHECK((yDesc.dim[2] == xDesc.dim[1]), FAILED, "The input and output dims are not equal!");
    CHECK((yDesc.dim[3] == xDesc.dim[2]), FAILED, "The input and output dims are not equal!");

    uint32_t n = static_cast<uint32_t>(yDesc.dim[0]);
    uint32_t c = static_cast<uint32_t>(yDesc.dim[1]);
    uint32_t h = static_cast<uint32_t>(yDesc.dim[2]);
    uint32_t w = static_cast<uint32_t>(yDesc.dim[3]);
    uint32_t dstIdx = 0;
    uint32_t idx = 0;
    fp16_t fp;

    DataTypeTransMode_t dataTypeTransmode = CC_DATATYPE_TRANS_FLOAT_NO_TRANS;
    if (GetDataTypeTransMode(xDesc.dataType, yDesc.dataType, dataTypeTransmode) != SUCCESS) {
        FMK_LOGD("GetDataTypeTransMode error!");
        return FAILED;
    }
    for (uint32_t nIdx = 0; nIdx < n; nIdx++) {
        for (uint32_t cIdx = 0; cIdx < c; cIdx++) {
            for (uint32_t hIdx = 0; hIdx < h; hIdx++) {
                for (uint32_t wIdx = 0; wIdx < w; wIdx++) {
                    idx = cIdx + wIdx * c + hIdx * w * c + nIdx * h * w * c; // (n h w c)
                    CopyToDstByDatatypetransmode(x, y, idx, dstIdx);
                    dstIdx++;
                }
            }
        }
    }
    return SUCCESS;
}

static Status TransTensorNC1HWC0ToNHWC(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y)
{
    CHECK((xDesc.dim[0] == yDesc.dim[0]), FAILED, "The input and output dims are not equal!");
    CHECK((xDesc.dim[1] == yDesc.dim[3]), FAILED, "The input and output dims are not equal!");
    CHECK((xDesc.dim[2] == yDesc.dim[1]), FAILED, "The input and output dims are not equal!");
    CHECK((xDesc.dim[3] == yDesc.dim[2]), FAILED, "The input and output dims are not equal!");

    DataTypeTransMode_t dataTypeTransmode = CC_DATATYPE_TRANS_FLOAT_NO_TRANS;
    HIAI_EXPECT_EXEC_R(GetDataTypeTransMode(xDesc.dataType, yDesc.dataType, dataTypeTransmode), FAILED);
#ifdef ARM_NEON
    if (((yDesc.dim[3] % CC_CUBE_SIZE == 0) || (yDesc.dim[3] % CC_CUBE_SIZE == 3)) &&
        (dataTypeTransmode == CC_DATATYPE_TRANS_FP16_TO_FLOAT)) {
        return TransTensorNC1HWC0ToNHWC_neon(xDesc, x, yDesc, y);
    }
    if (((yDesc.dim[3] % (CC_CUBE_SIZE * 2) == 0) || (yDesc.dim[3] % CC_CUBE_SIZE == 3)) &&
        (dataTypeTransmode == CC_DATATYPE_TRANS_UINT8_NO_TRANS)) {
        return TransTensorNC1HWC0ToNHWCUint8_neon(xDesc, x, yDesc, y);
    }

#endif
    uint32_t n = static_cast<uint32_t>(xDesc.dim[0]);
    uint32_t c = static_cast<uint32_t>(xDesc.dim[1]);
    uint32_t h = static_cast<uint32_t>(xDesc.dim[2]);
    uint32_t w = static_cast<uint32_t>(xDesc.dim[3]);
    uint32_t c0 =
        ((xDesc.dataType == CC_DATA_UINT8) || (xDesc.dataType == CC_DATA_INT8) || (xDesc.dataType == CC_DATA_QUINT8) ||
            (xDesc.dataType == CC_DATA_BOOL) || (xDesc.dataType == CC_DATA_2BITS)) ?
        CC_CUBE_SIZE * 2 :
        CC_CUBE_SIZE;
    uint32_t c1 = static_cast<uint32_t>(std::ceil(c * 1.0 / c0));
    uint32_t wc = w * c;
    uint32_t hw = h * w;
    uint32_t hwc = hw * c;
    uint32_t c1hwc0 = c1 * hw * c0;
    uint32_t hwc0 = hw * c0;
    uint32_t wc0 = w * c0;
    fp16_t fp;

    for (uint32_t nIdx = 0; nIdx < n; nIdx++) {
        /* head address of n dimension */
        uint32_t nFstAddr = nIdx * c1hwc0;
        for (uint32_t c1Idx = 0; c1Idx < c1; c1Idx++) {
            /* head address of c1 dimension */
            uint32_t c1FstAddr = nFstAddr + c1Idx * hwc0;
            for (uint32_t hIdx = 0; hIdx < h; hIdx++) {
                /* head address of h dimension */
                uint32_t hFstAddr = c1FstAddr + hIdx * wc0;
                for (uint32_t wIdx = 0; wIdx < w; wIdx++) {
                    /* head address of w dimension */
                    uint32_t wFstAddr = hFstAddr + wIdx * c0;
                    for (uint32_t c0Idx = 0; c0Idx < c0; c0Idx++) {
                        /* head of c0 dimension */
                        uint32_t idx = c0Idx + wFstAddr;
                        /* head address of c dimension */
                        uint32_t cIdx = c0Idx + c1Idx * c0;
                        /* address of w dimension */
                        uint32_t srcIdx = nIdx * hwc + hIdx * wc + wIdx * c + cIdx;
                        /* filter 0 padded from NHWC to NC1HWC0 */
                        if (cIdx < c) {
                            /* switch source and destination address so as to convert NC1HWC0 back to NCHW */
                            switch (dataTypeTransmode) {
                                case CC_DATATYPE_TRANS_FP16_TO_FLOAT:
                                    fp = static_cast<const fp16_t*>(x)[idx];
                                    static_cast<float*>(y)[srcIdx] = fp;
                                    break;
                                case CC_DATATYPE_TRANS_FP16_NO_TRANS:
                                    static_cast<fp16_t*>(y)[srcIdx] = static_cast<const fp16_t*>(x)[idx];
                                    break;
                                case CC_DATATYPE_TRANS_UINT8_NO_TRANS:
                                    static_cast<uint8_t*>(y)[srcIdx] = static_cast<const uint8_t*>(x)[idx];
                                    break;
                                case CC_DATATYPE_TRANS_INT8_NO_TRANS:
                                    static_cast<int8_t*>(y)[srcIdx] = static_cast<const int8_t*>(x)[idx];
                                    break;
                                case CC_DATATYPE_TRANS_FLOAT_NO_TRANS:
                                    static_cast<float*>(y)[srcIdx] = static_cast<const float*>(x)[idx];
                                    break;
                                default:
                                    FMK_LOGD("TransDataType from %d to %d is not supported!", xDesc.dataType,
                                        yDesc.dataType);
                                    return FAILED;
                            }
                        }
                    }
                }
            }
        }
    }
    return SUCCESS;
}

Status TransTensorFloatToHALF(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y)
{
    CHECK_NULL_WITH_RET(x, FAILED);
    CHECK_NULL_WITH_RET(y, FAILED);

    uint32_t dataCnt = xDesc.dataSize / sizeof(float);
    if (yDesc.dataSize < dataCnt * sizeof(fp16_t)) {
        FMK_LOGE("outputDataSize:%u not enough!", yDesc.dataSize);
        return FAILED;
    }

#if defined(ARM_NEON)
    uint32_t loopTime = dataCnt / 8;
    uint32_t lastTime = dataCnt % 8;
    uint64_t inPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(x)) + lastTime * 4;
    uint64_t outPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(y)) + lastTime * 2;
    FMK_LOGD("start TransTensorFloatToHALF loopTime:%u", loopTime);
    for (uint32_t i = 0; i < loopTime; i++) {
        TransTensorFp32ToFp16_C8_neon(inPtr, outPtr);
        inPtr = inPtr + 32;
        outPtr = outPtr + 16;
    }
    for (uint32_t i = 0; i < lastTime; i++) {
        fp16_t fp16Data;
        fp16Data = static_cast<const float*>(x)[i];
        static_cast<uint16_t*>(y)[i] = fp16Data.val;
    }
    FMK_LOGD("End TransTensorFloatToHALF");

#elif defined(ARM_NEON_32)

    uint32_t loopTime = dataCnt / 4;
    uint32_t lastTime = dataCnt % 4;
    uint64_t inPtr = (uint64_t)((uintptr_t)x) + lastTime * 4;
    uint64_t outPtr = (uint64_t)((uintptr_t)y) + lastTime * 2;

    FMK_LOGD("start TransTensorFloatToHALF loopTime:%u", loopTime);

    float16x4_t tmpfp16;
    float32x4_t tmpfp32;
    for (uint32_t i = 0; i < loopTime; i++) {
        tmpfp32 = vld1q_f32(reinterpret_cast<float32_t*>(inPtr));
        tmpfp16 = vcvt_f16_f32(tmpfp32);
        vst1_f16(reinterpret_cast<__fp16*>(outPtr), tmpfp16);
        inPtr = inPtr + 16;
        outPtr = outPtr + 8;
    }
    FMK_LOGD("start TransTensorFloatToHALF lastTime:%u", lastTime);
    for (uint32_t i = 0; i < lastTime; i++) {
        fp16_t fp16Data;
        fp16Data = static_cast<const float*>(x)[i];
        static_cast<uint16_t*>(y)[i] = fp16Data.val;
    }
    FMK_LOGD("End TransTensorFloatToHALF");

#else
    for (uint32_t i = 0; i < dataCnt; i++) {
        fp16_t fp16Data;
        fp16Data = static_cast<const float*>(x)[i];
        static_cast<uint16_t*>(y)[i] = fp16Data.val;
    }
#endif
    return SUCCESS;
}

static Status TransTensorFloatToFloat(const ccTensor_t& xDesc, const float* x, const ccTensor_t& yDesc, float* y)
{
    uint32_t dataCnt = xDesc.dataSize / sizeof(float);
    if (yDesc.dataSize < dataCnt * sizeof(float)) {
        FMK_LOGD("outputDataSize:%u not enough!", yDesc.dataSize);
        return FAILED;
    }
    CHECK_ONLY_RET(memcpy_s(y, yDesc.dataSize, x, dataCnt * sizeof(float)) == 0, FAILED);
    return SUCCESS;
}

static Status TransTensorINT32ToINT32(const ccTensor_t& xDesc, const int32_t* x, const ccTensor_t& yDesc, int32_t* y)
{
    uint32_t dataCnt = xDesc.dataSize / sizeof(int32_t);
    if (yDesc.dataSize < dataCnt * sizeof(int32_t)) {
        FMK_LOGD("outputDataSize:%u not enough!", yDesc.dataSize);
        return FAILED;
    }
    for (uint32_t i = 0; i < dataCnt; i++) {
        y[i] = x[i];
    }
    return SUCCESS;
}

static Status TransTensorINT64ToINT32(const ccTensor_t& xDesc, const int64_t* x, const ccTensor_t& yDesc, int32_t* y)
{
    uint32_t dataCnt = xDesc.dataSize / sizeof(int64_t);
    if (yDesc.dataSize < dataCnt * sizeof(int32_t)) {
        FMK_LOGD("outputDataSize:%u not enough!", yDesc.dataSize);
        return FAILED;
    }
    for (uint32_t i = 0; i < dataCnt; i++) {
        y[i] = static_cast<int32_t>(x[i]);
    }
    return SUCCESS;
}

static Status TransTensorINT64ToFloat(const ccTensor_t& xDesc, const int64_t* x, const ccTensor_t& yDesc, float* y)
{
    uint32_t dataCnt = xDesc.dataSize / sizeof(int64_t);
    if (yDesc.dataSize < dataCnt * sizeof(float)) {
        FMK_LOGD("outputDataSize:%u not enough!", yDesc.dataSize);
        return FAILED;
    }
    for (uint32_t i = 0; i < dataCnt; i++) {
        y[i] = static_cast<float>(x[i]);
    }
    return SUCCESS;
}

Status TransTensorHALFToFloat(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y)
{
    uint32_t dataCnt = xDesc.dataSize / sizeof(fp16_t);
    if (yDesc.dataSize < dataCnt * sizeof(float)) {
        FMK_LOGE("outputDataSize:%u not enough!", yDesc.dataSize);
        return FAILED;
    }

#if defined(ARM_NEON)
    uint32_t loopTime = dataCnt / 16;
    uint32_t lastTime = dataCnt % 16;
    uint64_t inPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(x)) + lastTime * 2;
    uint64_t outPtr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(y)) + lastTime * 4;

    FMK_LOGD("start transfer loopTime:%u", loopTime);
    for (uint32_t i = 0; i < loopTime; i++) {
        TransTensorHalfToFloat16_neon(inPtr, outPtr);
        inPtr = inPtr + 32;
        outPtr = outPtr + 64;
    }
    FMK_LOGD("start transfer lastTime:%u", lastTime);
    for (uint32_t i = 0; i < lastTime; i++) {
        fp16_t fp16Data = static_cast<const uint16_t*>(x)[i];
        float fp32Data = fp16Data;
        static_cast<float*>(y)[i] = fp32Data;
    }
    FMK_LOGD("End TransTensorHALFToFloat");

#elif defined(ARM_NEON_32)
    uint32_t loopTime = dataCnt / 4;
    uint32_t lastTime = dataCnt % 4;
    uint64_t inPtr = (uint64_t)((uintptr_t)x) + lastTime * 2;
    uint64_t outPtr = (uint64_t)((uintptr_t)y) + lastTime * 4;

    FMK_LOGD("start transfer loopTime:%u", loopTime);

    float16x4_t tmpfp16;
    float32x4_t tmpfp32;
    for (uint32_t i = 0; i < loopTime; i++) {
        tmpfp16 = vld1_f16(reinterpret_cast<__fp16*>(inPtr));
        tmpfp32 = vcvt_f32_f16(tmpfp16);
        vst1q_f32(reinterpret_cast<float32_t*>(outPtr), tmpfp32);
        inPtr = inPtr + 8;
        outPtr = outPtr + 16;
    }
    FMK_LOGD("start transfer lastTime:%u", lastTime);
    for (uint32_t i = 0; i < lastTime; i++) {
        fp16_t fp16Data = static_cast<const uint16_t*>(x)[i];
        float fp32Data = fp16Data;
        static_cast<float*>(y)[i] = fp32Data;
    }
    FMK_LOGD("End TransTensorHALFToFloat");

#else
    for (uint32_t i = 0; i < dataCnt; i++) {
        fp16_t fp16Data = static_cast<const uint16_t*>(x)[i];
        float fp32Data = fp16Data;
        static_cast<float*>(y)[i] = fp32Data;
    }
#endif
    return SUCCESS;
}

static Status TransTensorHALFToUINT8(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y)
{
    uint32_t dataCnt = xDesc.dataSize / sizeof(fp16_t);
    if (yDesc.dataSize < dataCnt * sizeof(uint8_t)) {
        FMK_LOGE("outputDataSize:%u not enough!", yDesc.dataSize);
        return FAILED;
    }

    for (uint32_t i = 0; i < dataCnt; i++) {
        fp16_t fp16Data = (static_cast<const uint16_t*>(x))[i];
        static_cast<uint8_t*>(y)[i] = fp16Data.toUInt8();
    }
    return SUCCESS;
}

#define DoTransTensor(tempDesc, xDesc, x, yDesc, y, ySizeInBytes) \
    do { \
        if (((xDesc).format == FORMAT_NC1HWC0) && ((yDesc).format == FORMAT_NCHW)) { \
            return TransTensorNC1HWC0ToNCHW(xDesc, x, yDesc, y); \
        } else if (((xDesc).format == FORMAT_NC1HWC0) && ((yDesc).format == FORMAT_NHWC)) { \
            return TransTensorNC1HWC0ToNHWC(xDesc, x, yDesc, y); \
        } else if (((xDesc).format == FORMAT_NHWC) && ((yDesc).format == FORMAT_NC1HWC0)) { \
            return TransTensorNHWCToNC1HWC0(xDesc, x, yDesc, y); \
        } else if (((xDesc).format == FORMAT_NCHW) && ((yDesc).format == FORMAT_NC1HWC0)) { \
            return TransTensorNCHWToNC1HWC0(xDesc, x, yDesc, y); \
        } else if (((xDesc).format == (yDesc).format) || \
            (((xDesc).format == FORMAT_ND) && ((yDesc).format == FORMAT_NCHW)) || \
            (((xDesc).format == FORMAT_NCHW) && ((yDesc).format == FORMAT_ND)) || \
            (((xDesc).format == FORMAT_NHWC) && ((yDesc).format == FORMAT_ND))) { \
            if (((xDesc).dataType == CC_DATA_FLOAT) && ((yDesc).dataType == CC_DATA_HALF)) { \
                return TransTensorFloatToHALF(xDesc, x, yDesc, y); \
            } else if (((xDesc).dataType == CC_DATA_HALF) && ((yDesc).dataType == CC_DATA_FLOAT)) { \
                return TransTensorHALFToFloat(xDesc, x, yDesc, y); \
            } else if (((xDesc).dataType == CC_DATA_FLOAT) && ((yDesc).dataType == CC_DATA_FLOAT)) { \
                return TransTensorFloatToFloat(xDesc, static_cast<const float*>(x), yDesc, static_cast<float*>(y)); \
            } else if (((xDesc).dataType == CC_DATA_INT32) && ((yDesc).dataType == CC_DATA_INT32)) { \
                return TransTensorINT32ToINT32( \
                    (xDesc), (static_cast<const int32_t*>(x)), (yDesc), (static_cast<int32_t*>(y))); \
            } else if (((xDesc).dataType == CC_DATA_INT64) && ((yDesc).dataType == CC_DATA_INT32)) { \
                return TransTensorINT64ToINT32( \
                    (xDesc), (static_cast<const int64_t*>(x)), (yDesc), (static_cast<int32_t*>(y))); \
            } else if (((xDesc).dataType == CC_DATA_INT64) && ((yDesc).dataType == CC_DATA_FLOAT)) { \
                return TransTensorINT64ToFloat( \
                    (xDesc), (static_cast<const int64_t*>(x)), (yDesc), (static_cast<float*>(y))); \
            } else if (((xDesc).dataType == CC_DATA_HALF) && ((yDesc).dataType == CC_DATA_UINT8)) { \
                return TransTensorHALFToUINT8(xDesc, x, yDesc, y); \
            } else { \
                FMK_LOGD( \
                    "TranTensor from datatype:[%d] to [%d] is not be supported!", (xDesc).dataType, (yDesc).dataType); \
                FMK_LOGD("TranTensor from format:[%d] to [%d]!", (xDesc).format, (yDesc).format); \
                FMK_LOGD("TranTensor inputDesc dim0=%d, dim1=%d, dim2=%d, dim3=%d!", (xDesc).dim[0], (xDesc).dim[1], \
                    (xDesc).dim[2], (xDesc).dim[3]); \
                FMK_LOGD("TranTensor outputDesc dim0=%d, dim1=%d, dim2=%d, dim3=%d!", (yDesc).dim[0], (yDesc).dim[1], \
                    (yDesc).dim[2], (yDesc).dim[3]); \
                return FAILED; \
            } \
        } else if (((xDesc).format == FORMAT_ND) && ((yDesc).format == FORMAT_NC1HWC0)) { \
            (void)memcpy_s(reinterpret_cast<void*>(&tempDesc), sizeof(ccTensor_t), \
                reinterpret_cast<const void*>(&xDesc), sizeof(ccTensor_t)); \
            CHECK_ONLY_RET(DescNd2NHWC(tempDesc) == SUCCESS, FAILED); \
            return TransTensorNHWCToNC1HWC0(tempDesc, x, yDesc, y); \
        } else if (((xDesc).format == FORMAT_NC1HWC0) && ((yDesc).format == FORMAT_ND)) { \
            (void)memcpy_s(reinterpret_cast<void*>(&tempDesc), sizeof(ccTensor_t), \
                reinterpret_cast<const void*>(&yDesc), sizeof(ccTensor_t)); \
            CHECK_ONLY_RET(DescNd2NHWC(tempDesc) == SUCCESS, FAILED); \
            return TransTensorNC1HWC0ToNHWC(xDesc, x, tempDesc, y); \
        } else if (((xDesc).format == FORMAT_NHWC) && ((yDesc).format == FORMAT_NCHW)) { \
            return TransTensorNHWCToNCHW(xDesc, x, yDesc, y); \
        } else { \
            FMK_LOGD("TransTensor:from format:[%d] to [%d] is not be supported!", (xDesc).format, (yDesc).format); \
            return FAILED; \
        } \
    } while (0);

static Status TransTensor(
    const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y, uint32_t ySizeInBytes)
{
    ccTensor_t tempDesc;

    if ((x == nullptr) || (y == nullptr)) {
        FMK_LOGD("input is nullptr!");
        return FAILED;
    }

    CHECK(CheckTensorOverFlow(xDesc) == SUCCESS, FAILED, "xDesc is overflow!!!");
    CHECK(CheckTensorOverFlow(yDesc) == SUCCESS, FAILED, "yDesc is overflow!!!");
    CHECK(xDesc.dimCnt <= CC_DIM_MAX, FAILED, "input tensor's dimCnt is out of range");
    uint32_t xSizeInBytes = sizeof(uint16_t);
    CHECK(GetDataTypeSize(xDesc.dataType, xSizeInBytes) == SUCCESS, FAILED, "not support this dataType.");
    for (int32_t i = 0; i < xDesc.dimCnt; ++i) {
        xSizeInBytes *= xDesc.dim[i];
    }
    CHECK(xDesc.dataSize >= xSizeInBytes, FAILED, "input data size is error.");

    if (yDesc.dataSize > ySizeInBytes) {
        FMK_LOGE("calc size fail or output data size is too small!");
        return FAILED;
    }
    DoTransTensor(tempDesc, xDesc, x, yDesc, y, ySizeInBytes);
}

static Status TransferDim(const std::vector<int64_t>& dim, std::vector<int32_t>& dimVector)
{
    uint32_t inputShapeSize = dim.size();
    std::list<uint32_t> newDimList;

    for (auto dim_temp : dim) {
        newDimList.push_back(dim_temp);
    }

    if (inputShapeSize > 5) {
        FMK_LOGE("Cannot support inputShapeSize %u", inputShapeSize);
        return FAILED;
    }
    switch (inputShapeSize) {
        case 0: {
            newDimList.push_back(1);
            newDimList.push_back(1);
            newDimList.push_back(1);
            newDimList.push_back(1);
            break;
        }
        case 1: {
            newDimList.push_front(1);
            newDimList.push_back(1);
            newDimList.push_back(1);
            break;
        }
        case 2: {
            newDimList.push_front(1);
            newDimList.push_back(1);
            break;
        }
        case 3: {
            newDimList.push_front(1);
            break;
        }
        default:
            break;
    }

    dimVector.clear();
    for (auto dimNew : newDimList) {
        dimVector.push_back(dimNew);
    }
    return SUCCESS;
}

static bool CheckEnumValid(int32_t check_value, int32_t min_enum_value, int32_t max_enum_value)
{
    return check_value < min_enum_value ? false : (check_value >= max_enum_value ? false : true);
}

Status InitTensorDescriptor(const ge::TensorDesc& tensor, ccTensor_t& ccTensor)
{
    Status ret = SUCCESS;

    Format format = tensor.GetFormat();
    int32_t data_type = static_cast<int32_t>(tensor.GetDataType());
    std::vector<int64_t> dim = tensor.GetShape().GetDims();

    // format
    if (!CheckEnumValid(format, FORMAT_NCHW, FORMAT_RESERVED)) {
        FMK_LOGE("not supported format, format = %d", format);
        return FAILED;
    }

    // data type
    if (!CheckEnumValid(data_type, CC_DATA_FLOAT, CC_DATA_RESERVED)) {
        FMK_LOGE("not supported data type, type = %d", data_type);
        return FAILED;
    }

    uint32_t real_dim_cnt = 0;
    (void)ge::TensorUtils::GetRealDimCnt(tensor, real_dim_cnt);
    if (real_dim_cnt > CC_DIM_MAX) {
        FMK_LOGE("param is invalid, real_dim_cnt:%u", real_dim_cnt);
        return FAILED;
    }
    ccTensor.realDimCnt = static_cast<int32_t>(real_dim_cnt);

    DataType_t dataType = tagDataType(data_type);
    if (format == FORMAT_ND) {
        int32_t realDim[CC_DIM_MAX] = {0};
        uint32_t i = 0;
        for (auto dim_temp : dim) {
            if (i >= real_dim_cnt) {
                break;
            }
            realDim[i] = static_cast<int32_t>(dim_temp);
            i++;
        }
        auto ccRet = SetTensorNdDescriptor(ccTensor, dataType, real_dim_cnt, realDim);
        if (ccRet != SUCCESS) {
            FMK_LOGE("Call SetTensorNdDescriptor failed. ccRet = %d", ccRet);
            return FAILED;
        }
        return ret;
    }

    std::vector<int32_t> dimVector;
    if (TransferDim(dim, dimVector) != SUCCESS) {
        FMK_LOGE("TransferDim failed.");
        return FAILED;
    }

    if (dim.size() == 1 && dim[0] == 0) {
        return SUCCESS;
    }

    if (format == FORMAT_NDHWC || format == FORMAT_NCDHW) {
        ret = SetNDTensorDescriptor(ccTensor, format, dataType, dimVector);
    } else if (format == FORMAT_NHWC) {
        std::vector<int32_t> dims = {dimVector.at(0), dimVector.at(3), dimVector.at(1), dimVector.at(2)};
        ret = SetTensor4dDescriptor(ccTensor, format, dataType, dims);
    } else {
        std::vector<int32_t> dims = {dimVector.at(0), dimVector.at(1), dimVector.at(2), dimVector.at(3)};
        ret = SetTensor4dDescriptor(ccTensor, format, dataType, dims);
    }
    if (ret != SUCCESS) {
        FMK_LOGE("SetTensorDescriptor failed. ret:%d, format:%d", ret, static_cast<int>(format));
        return FAILED;
    }

    return ret;
}

HCS_API_EXPORT Status TransTensor(const ge::TensorDesc& xDesc, const void* x, const ge::TensorDesc& yDesc, void* y)
{
    ccTensor_t xDescCC = {};
    ccTensor_t yDescCC = {};
    uint32_t ySizeInBytes = 0;

    Status ret = InitTensorDescriptor(xDesc, xDescCC);
    if (ret != SUCCESS) {
        FMK_LOGE("get input ccTensor descriptor failed.");
        return FAILED;
    }

    ret = InitTensorDescriptor(yDesc, yDescCC);
    if (ret != SUCCESS) {
        FMK_LOGE("call Init out_desc TensorDescriptor failed.");
        return FAILED;
    }

    // 获取数据大小
    ret = GetTensorMemorySizeInBytes(yDescCC, ySizeInBytes);
    if (ret != SUCCESS || ySizeInBytes == 0) {
        FMK_LOGE("GetTensorMemorySizeInBytes failed. ret = %d", ret);
        return FAILED;
    }

    ret = TransTensor(xDescCC, x, yDescCC, y, ySizeInBytes);
    if (ret != SUCCESS || ySizeInBytes == 0) {
        FMK_LOGE("TransTensor failed. ret = %d", ret);
        return FAILED;
    }
    return SUCCESS;
}
} // namespace ge
