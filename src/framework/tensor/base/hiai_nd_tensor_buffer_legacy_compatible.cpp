#include "hiai_nd_tensor_buffer_legacy_compatible.h"

#include "securec.h"
#include "framework/infra/log/log.h"
#include "hiai_nd_tensor_buffer_def.h"
#include "util/hiai_foundation_dl_helper.h"
#include "framework/c/compatible/hiai_tensor_buffer.h"
#include "hiai_nd_tensor_buffer_legacy.h"

static void* HIAI_NDTensorBuffer_GetHandleFromNDTensorBuffer(HIAI_NDTensorBuffer* buffer)
{
    auto getHandleFunc = (void* (*)(HIAI_NDTensorBuffer*))HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_GetHandle");
    if (getHandleFunc == nullptr) {
        FMK_LOGE("sym not found.");
        return nullptr;
    }

    return getHandleFunc(buffer);
}

static HIAI_NDTensorDesc* HIAI_NDTensorBuffer_GetNDTensorDescFromNDTensorBuffer(HIAI_NDTensorBuffer* buffer)
{
    auto getNDTensorDescFunc = (HIAI_NDTensorDesc* (*)(HIAI_NDTensorBuffer*))
        HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_GetNDTensorDesc");
    if (getNDTensorDescFunc == nullptr) {
        FMK_LOGE("sym not found.");
        return nullptr;
    }

    return getNDTensorDescFunc(buffer);
}

static HIAI_TensorBuffer* ConvertNDTensorBufferToTensorBuffer(HIAI_NDTensorBuffer* ndBuffer)
{
    HIAI_TensorBuffer* buffer = (HIAI_TensorBuffer*)malloc(sizeof(HIAI_TensorBuffer));
    MALLOC_NULL_CHECK_RET_VALUE(buffer, nullptr);
    (void)memset_s(buffer, sizeof(HIAI_TensorBuffer), 0, sizeof(HIAI_TensorBuffer));

    buffer->data = HIAI_NDTensorBuffer_GetDataFromNDTensorBuffer(ndBuffer);
    buffer->size = static_cast<int>(HIAI_NDTensorBuffer_GetSizeFromNDTensorBuffer(ndBuffer));
    buffer->impl = HIAI_NDTensorBuffer_GetHandleFromNDTensorBuffer(ndBuffer);

    HIAI_NDTensorDesc* ndDesc = HIAI_NDTensorBuffer_GetNDTensorDescFromNDTensorBuffer(ndBuffer);
    size_t dimNum = HIAI_NDTensorDesc_GetDimNum(ndDesc);
    if (dimNum != 4) {
        FMK_LOGW("this is not 4D tensor.");
        free(buffer);
        return nullptr;
    }
    buffer->desc.number = HIAI_NDTensorDesc_GetDim(ndDesc, 0);
    buffer->desc.channel = HIAI_NDTensorDesc_GetDim(ndDesc, 1);
    buffer->desc.height = HIAI_NDTensorDesc_GetDim(ndDesc, 2);
    buffer->desc.width = HIAI_NDTensorDesc_GetDim(ndDesc, 3);
    buffer->desc.dataType = HIAI_NDTensorDesc_GetDataType(ndDesc);

    HIAI_NDTensorDesc_Destroy(&ndDesc);
    return buffer;
}

bool HIAI_ChangeNDTensorBuffersHandleToTensorBuffers(HIAI_NDTensorBuffer* buffers[], int32_t num)
{
    for (int32_t i = 0; i < num; i++) {
        HIAI_NDTensorBufferV2* buffersV2 = reinterpret_cast<HIAI_NDTensorBufferV2*>(buffers[i]);
        if (buffersV2->isLegacy) {
            FMK_LOGI("buffers's handle is HIAI_TensorBuffer.");
            return true;
        }

        HIAI_TensorBuffer* buffer = ConvertNDTensorBufferToTensorBuffer((HIAI_NDTensorBuffer*)(buffersV2->handle));
        if (buffer == nullptr) {
            FMK_LOGE("convert buffer failed.");
            return false;
        }

        free(buffersV2->handle);

        buffersV2->handle = static_cast<void*>(buffer);
        buffersV2->isLegacy = true;
    }
    FMK_LOGI("convert buffer success.");
    return true;
}