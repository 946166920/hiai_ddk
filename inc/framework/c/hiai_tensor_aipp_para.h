/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: hiai tensor aipp param
 *
 */
#ifndef FRAMEWORK_C_HIAI_TENSOR_AIPP_PARA_H
#define FRAMEWORK_C_HIAI_TENSOR_AIPP_PARA_H
#include <stdint.h>
#include <stdlib.h>

#include "hiai_c_api_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_TensorAippBatchPara {
    int8_t cropSwitch; // crop switch
    int8_t scfSwitch; // resize switch
    int8_t paddingSwitch; // 0: unable padding, 1: padding config value,sfr_filling_hblank_ch0 sfr_filling_hblank_ch2
    int8_t rotateSwitch; // rotate switch0: non-ratate1: ratate 90 clockwise2: ratate 180 clockwise3: ratate 270
    int8_t reserve[4];
    uint32_t cropStartPosW; // the start horizontal position of cropping
    uint32_t cropStartPosH; // the start vertical position of cropping
    uint32_t cropSizeW; // crop width
    uint32_t cropSizeH; // crop height
    int32_t scfInputSizeW; // input width of scf
    int32_t scfInputSizeH; // input height of scf
    uint32_t scfOutputSizeW; // output width of scf
    uint32_t scfOutputSizeH; // output height of scf
    uint32_t paddingSizeTop; // top padding size
    uint32_t paddingSizeBottom; // bottom padding size
    uint32_t paddingSizeLeft; // left padding size
    uint32_t paddingSizeRight; // right padding size
    int16_t dtcPixelMeanChn0; // mean value of channel 0
    int16_t dtcPixelMeanChn1; // mean value of channel 1
    int16_t dtcPixelMeanChn2; // mean value of channel 2
    int16_t dtcPixelMeanChn3; // mean value of channel 3
    uint16_t dtcPixelMinChn0; // min value of channel 0
    uint16_t dtcPixelMinChn1; // min value of channel 1
    uint16_t dtcPixelMinChn2; // min value of channel 2
    uint16_t dtcPixelMinChn3; // min value of channel 3
    uint16_t dtcPixelVarReciChn0; // sfr_dtc_pixel_variance_reci_ch0
    uint16_t dtcPixelVarReciChn1; // sfr_dtc_pixel_variance_reci_ch1
    uint16_t dtcPixelVarReciChn2; // sfr_dtc_pixel_variance_reci_ch2
    uint16_t dtcPixelVarReciChn3; // sfr_dtc_pixel_variance_reci_ch3
    uint16_t paddingValueChn0; // padding value of channle 0
    uint16_t paddingValueChn1; // padding value of channle 1
    uint16_t paddingValueChn2; // padding value of channle 2
    uint16_t paddingValueChn3; // padding value of channle 3
    int8_t reserve1[8]; // 8B assign, for ub copy
} HIAI_TensorAippBatchPara;

typedef struct HIAI_TensorAippCommPara {
    uint8_t inputFormat; // input format YUV420SP_U8/XRGB8888_U8/RGB888_U8
    int8_t cscSwitch; // csc switch
    int8_t rbuvSwapSwitch; // rb/ub swap switch
    int8_t axSwapSwitch; // RGBA->ARGB, YUVA->AYUV swap switch
    uint8_t batchNum; // batch parameter number
    int8_t reserve1[3];
    int32_t srcImageSizeW; // source image width
    int32_t srcImageSizeH; // source image height
    int16_t cscMatrixR0C0; // csc_matrix_r0_c0
    int16_t cscMatrixR0C1; // csc_matrix_r0_c1
    int16_t cscMatrixR0C2; // csc_matrix_r0_c2
    int16_t cscMatrixR1C0; // csc_matrix_r1_c0
    int16_t cscMatrixR1C1; // csc_matrix_r1_c1
    int16_t cscMatrixR1C2; // csc_matrix_r1_c2
    int16_t cscMatrixR2C0; // csc_matrix_r2_c0
    int16_t cscMatrixR2C1; // csc_matrix_r2_c1
    int16_t cscMatrixR2C2; // csc_matrix_r2_c2
    int16_t reserve2[3];
    uint8_t cscOutputBiasR0; // output Bias for RGB to YUV, element of row 0, unsigned number
    uint8_t cscOutputBiasR1; // output Bias for RGB to YUV, element of row 1, unsigned number
    uint8_t cscOutputBiasR2; // output Bias for RGB to YUV, element of row 2, unsigned number
    uint8_t cscInputBiasR0; // input Bias for YUV to RGB, element of row 0, unsigned number
    uint8_t cscInputBiasR1; // input Bias for YUV to RGB, element of row 1, unsigned number
    uint8_t cscInputBiasR2; // input Bias for YUV to RGB, element of row 2, unsigned number
    uint8_t reserve3[2];
    int8_t reserve4[16];
    HIAI_TensorAippBatchPara aippBatchPara[0]; // allow transfer several batch para.
} HIAI_TensorAippCommPara;

typedef struct HIAI_TensorAippPara HIAI_TensorAippPara;

AICP_C_API_EXPORT HIAI_TensorAippPara* HIAI_TensorAippPara_Create(uint32_t batchNum);
AICP_C_API_EXPORT HIAI_TensorAippPara* HIAI_TensorAippPara_CreateWithHandle(void* data, size_t size, void* handle);

AICP_C_API_EXPORT void* HIAI_TensorAippPara_GetRawBuffer(HIAI_TensorAippPara* tensorAippPara);
AICP_C_API_EXPORT int32_t HIAI_TensorAippPara_GetRawBufferSize(HIAI_TensorAippPara* tensorAippPara);
AICP_C_API_EXPORT void* HIAI_TensorAippPara_GetHandle(HIAI_TensorAippPara* tensorAippPara);

AICP_C_API_EXPORT int32_t HIAI_TensorAippPara_GetInputIndex(HIAI_TensorAippPara* tensorAippPara);
AICP_C_API_EXPORT void HIAI_TensorAippPara_SetInputIndex(HIAI_TensorAippPara* tensorAippPara, uint32_t inputIndex);

AICP_C_API_EXPORT int32_t HIAI_TensorAippPara_GetInputAippIndex(HIAI_TensorAippPara* tensorAippPara);
AICP_C_API_EXPORT void HIAI_TensorAippPara_SetInputAippIndex(
    HIAI_TensorAippPara* tensorAippPara, uint32_t inputAippIndex);

AICP_C_API_EXPORT void HIAI_TensorAippPara_Destroy(HIAI_TensorAippPara** aippParas);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_TENSOR_AIPP_PARA_H
