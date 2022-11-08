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

#ifndef DEF_TYPES_H
#define DEF_TYPES_H

#include <memory>
#include <vector>
#include <atomic>
#include "graph/buffer.h"

namespace ge {
#define DEF_TYPE_DEC(type, name) \
    inline void set_##name(const type& value) \
    { \
        name = value; \
    }; \
    type* mutable_##name() \
    { \
        return &name; \
    }
#define DEF_TYPE_HAS_DEC(type, name) \
    inline void set_##name(const type& value) \
    { \
        name = value; \
    }; \
\
private: \
    bool has_mutable_##name {false}; \
\
public: \
    bool has_##name() const \
    { \
        return (has_mutable_##name) || QuantizeFactorHasData(name); \
    }; \
    type* mutable_##name() \
    { \
        has_mutable_##name = true; \
        return &name; \
    }
#define DEF_TYPE_VEC_DEC(type, name) do { \
    inline int name##_size() const \
    { \
        return name.size(); \
    } \
    inline void clear_##name() \
    { \
        name.clear(); \
    } \
    inline void set_##name(int index, type value) \
    { \
        name[index] = value; \
    }; \
    inline void add_##name(type value) \
    { \
        name.push_back(value); \
    } \
    inline std::vector<type>* mutable_##name() \
    { \
        return &name; \
    } \
} while (0)


#define DEF_TYPE_BYTES_DEC(name) \
    inline void clear_##name() \
    { \
        name.Clear(); \
    } \
    inline void set_##name(const void* value, size_t size) \
    { \
        Buffer::CopyFrom(reinterpret_cast<const std::uint8_t*>(value), size, name); \
    } \
    inline Buffer* mutable_##name() \
    { \
        return &name; \
    }

struct CompressInfo {
public:
    CompressInfo()
    {
    }

    CompressInfo(int32_t blockRow, int32_t blockCol, int32_t fractalK, int32_t fractalN, int32_t lastFractalK,
        int32_t lastFractalN, int32_t cubeSize, int32_t loadDir)
    {
        blockrow = blockRow;
        blockcol = blockCol;
        fractalk = fractalK;
        fractaln = fractalN;
        lastfractalk = lastFractalK;
        lastfractaln = lastFractalN;
        cubesize = cubeSize;
        loaddir = loadDir;
    };

    int32_t blockrow {0}; // block row
    int32_t blockcol {0}; // block col
    int32_t fractalk {0}; // fractal K
    int32_t fractaln {0}; // fractal N
    int32_t lastfractalk {0}; // K of last fractal
    int32_t lastfractaln {0}; // N of last fractal
    int32_t cubesize {0}; // cube's length
    int32_t loaddir {0}; // data load directtiono 0:col load 1:row load
    DEF_TYPE_DEC(int32_t, blockrow);

    DEF_TYPE_DEC(int32_t, blockcol);

    DEF_TYPE_DEC(int32_t, fractalk);

    DEF_TYPE_DEC(int32_t, fractaln);

    DEF_TYPE_DEC(int32_t, lastfractalk);

    DEF_TYPE_DEC(int32_t, lastfractaln);

    DEF_TYPE_DEC(int32_t, cubesize);

    DEF_TYPE_DEC(int32_t, loaddir);
};

enum QuantizeScaleType { VECTOR_SCALE = 0, SCALAR_SCALE = 1 };

enum QuantizeScaleMode { NORMAL_MODE = 0, SQRT_MODE = 1 };

enum QuantizeAlgorithm {
    NON_OFFSET_ALGO = 0,
    HALF_OFFSET_ALGO = 1,
    ALL_OFFSET_ALGO = 2,
};

enum QuantizeWinoFlag { WINO_ALGO = 0, NORMAL_ALGO = 1 };

enum QuantizeBiasOptimizeType { DEFAUT_BIAS_TYPE = 0, INT32_BIAS_TYPE = 1, INT16_BIAS_TYPE = 2, INT8_BIAS_TYPE = 3 };

struct QuantizeFactor {
public:
    uint32_t scale_mode {0};
    Buffer scale_value;
    int64_t scale_offset {0};
    Buffer offset_data_value;
    int64_t offset_data_offset {0};
    Buffer offset_weight_value;
    int64_t offset_weight_offset {0};
    Buffer offset_pad_value;
    int64_t offset_pad_offset {0};

    DEF_TYPE_DEC(uint32_t, scale_mode);

    DEF_TYPE_BYTES_DEC(scale_value);

    DEF_TYPE_DEC(int64_t, scale_offset);

    DEF_TYPE_BYTES_DEC(offset_data_value);

    DEF_TYPE_DEC(int64_t, offset_data_offset);

    DEF_TYPE_BYTES_DEC(offset_weight_value);

    DEF_TYPE_DEC(int64_t, offset_weight_offset);

    DEF_TYPE_BYTES_DEC(offset_pad_value);

    DEF_TYPE_DEC(int64_t, offset_pad_offset);
};

struct SparseAlgorithmParams {
public:
    int64_t sparseAlgorithmOffset {0};
    Buffer sparseAlgorithmData;

    DEF_TYPE_DEC(int64_t, sparseAlgorithmOffset);

    DEF_TYPE_BYTES_DEC(sparseAlgorithmData);
};

inline bool QuantizeFactorHasData(const QuantizeFactor& factor)
{
    return factor.scale_value.GetSize() > 0 || factor.offset_data_value.GetSize() > 0 ||
        factor.offset_weight_value.GetSize() > 0 || factor.offset_pad_value.GetSize() > 0;
}

struct AllOffsetQuantizeInfo {
public:
    AllOffsetQuantizeInfo()
    {
    }

    AllOffsetQuantizeInfo(float s, int32_t o) : scale(s), offset(o)
    {
    }

    float scale {0};
    int32_t offset {0};

    DEF_TYPE_DEC(float, scale);

    DEF_TYPE_DEC(int32_t, offset);
};

struct QuantizeCalcFactor {
public:
    Buffer offsetw;
    int64_t offsetw_offset {0};
    Buffer offsetd;
    int64_t offsetd_offset {0};
    Buffer scalereq;
    int64_t scaledreq_offset {0};
    Buffer offsetdnext;
    int64_t offsetdnext_offset {0};

    DEF_TYPE_BYTES_DEC(offsetw);

    DEF_TYPE_DEC(int64_t, offsetw_offset);

    DEF_TYPE_BYTES_DEC(offsetd);

    DEF_TYPE_DEC(int64_t, offsetd_offset);

    DEF_TYPE_BYTES_DEC(scalereq);

    DEF_TYPE_DEC(int64_t, scaledreq_offset);

    DEF_TYPE_BYTES_DEC(offsetdnext);

    DEF_TYPE_DEC(int64_t, offsetdnext_offset);
};

inline bool QuantizeFactorHasData(const QuantizeCalcFactor& factor)
{
    return factor.offsetw.GetSize() > 0 || factor.offsetd.GetSize() > 0 || factor.scalereq.GetSize() > 0 ||
        factor.offsetdnext.GetSize() > 0;
}

struct QuantizeFactorParams {
    uint32_t quantize_algo {0};
    uint32_t scale_type {0};
    QuantizeFactor quantize_param;
    QuantizeFactor dequantize_param;
    QuantizeFactor requantize_param;
    QuantizeCalcFactor quantizecalc_param;

    DEF_TYPE_DEC(uint32_t, quantize_algo);

    DEF_TYPE_DEC(uint32_t, scale_type);

    DEF_TYPE_HAS_DEC(QuantizeFactor, quantize_param);

    DEF_TYPE_HAS_DEC(QuantizeFactor, dequantize_param);

    DEF_TYPE_HAS_DEC(QuantizeFactor, requantize_param);

    DEF_TYPE_HAS_DEC(QuantizeCalcFactor, quantizecalc_param);
};

struct QuantizeInfo {
public:
    uint32_t quantize_algo {0};
    float scale_data_value {0.0}; // scaled, float
    float offset_data_value {0.0}; // offset, uint8
    uint32_t scale_weight_mode {0}; // weight mode vector or scalar
    Buffer scale_weight_value; // weight scale, float
    Buffer offset_weight_value; // weight offset, uint8, half offset invailid

    DEF_TYPE_DEC(uint32_t, quantize_algo);
    DEF_TYPE_DEC(float, scale_data_value);
    DEF_TYPE_DEC(float, offset_data_value);
    DEF_TYPE_DEC(uint32_t, scale_weight_mode);
    DEF_TYPE_BYTES_DEC(scale_weight_value);
    DEF_TYPE_BYTES_DEC(offset_weight_value);
};

struct QuantizeInfoExt {
public:
    uint32_t winoFlag {0}; // support wino of not
    int32_t nValue {0};
    uint32_t biasOptmizeType {0}; // scaled, float

    DEF_TYPE_DEC(uint32_t, winoFlag);
    DEF_TYPE_DEC(int32_t, nValue);
    DEF_TYPE_DEC(uint32_t, biasOptmizeType);
};

struct QuantizeFactorParamsV200 {
    uint32_t quantizeAlgo {0};
    uint16_t scaled {0};
    uint32_t scaledMode {0};
    uint8_t offsetd {0};
    int64_t offsetwOffset {0};
    int64_t deqscaleOffset {0};
    uint8_t offsetdNext {0};
    int64_t reqscaleOffset {0};
    Buffer offsetw;
    Buffer deqscale;
    Buffer reqscale;
    float scaledNext;

    DEF_TYPE_DEC(uint32_t, quantizeAlgo);
    DEF_TYPE_DEC(uint16_t, scaled);
    DEF_TYPE_DEC(uint32_t, scaledMode);
    DEF_TYPE_DEC(uint8_t, offsetd);
    DEF_TYPE_BYTES_DEC(offsetw);
    DEF_TYPE_DEC(int64_t, offsetwOffset);
    DEF_TYPE_BYTES_DEC(deqscale);
    DEF_TYPE_DEC(int64_t, deqscaleOffset);
    DEF_TYPE_BYTES_DEC(reqscale);
    DEF_TYPE_DEC(int64_t, reqscaleOffset);
    DEF_TYPE_DEC(float, scaledNext);
};

#undef DEF_TYPE_DEC
} // namespace ge
#endif // GE_PUBLIC_TYPES_H
