/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: omg hiai ir build
 *
 */
#ifndef HIAI_API_HIAI_IR_BUILD_H
#define HIAI_API_HIAI_IR_BUILD_H

#include "graph/model.h"
#include "compatible/HiAiModelBuilderType.h"
#include "model_builder/model_builder_types.h"

namespace hiai {
class IBuiltModel;
using Status = uint32_t;
struct ModelBufferData {
    void* data = nullptr;
    uint32_t length = 0;
};

const static BuildOptions defaultBuildOptions;

class HiaiIrBuild {
public:
    /**
     * @ingroup domi_omg
     * @brief 创建模型Buffer, 默认分配200M
     * @param [in] irModel 输入模型数据
     * @param [out] output 输出离线模型Buffer
     * @return bool 执行结果
     */
    bool CreateModelBuff(ge::Model& irModel, ModelBufferData& output);

    /**
     * @ingroup domi_omg
     * @brief 创建模型Buffer
     * @param [in] irModel 输入模型数据
     * @param [out] output 输出离线模型Buffer
     * @param [in] customSize
     * 输入用户指定Buffer大小，单位字节，必须为非负整数，建议范围200M以内，当customSize设置为0时，接口内部根据irModel自动计算合适的Buffer大小
     * @return bool 执行结果
     */
    bool CreateModelBuff(ge::Model& irModel, ModelBufferData& output, uint32_t customSize);

    /*
     * @ingroup domi_omg
     * @brief 在线编译
     * @param [in] irModel 输入模型数据
     * @param [out] output 输出离线模型
     * @return bool 执行结果
     */
    bool BuildIRModel(ge::Model& irModel, ModelBufferData& output);

    /*
     * @ingroup domi_omg
     * @brief 在线编译
     * @param [in] options 输入编译配置参数
     * @param [in] irModel 输入模型数据
     * @param [out] output 输出离线模型
     * @return bool 执行结果
     */
    bool BuildIRModel(ge::Model& irModel, ModelBufferData& output, const BuildOptions& options);

    void ReleaseModelBuff(ModelBufferData& output);

    Status Build(const hiai::ModelBuildOptions& options, const std::string& modelName,
        const std::shared_ptr<ge::Model>& model, std::shared_ptr<hiai::IBuiltModel>& builtModel);

private:
    /*
     * @ingroup domi_omg
     * @brief 在线编译生成模型
     * @return bool 成功返回true，失败返回false
     */
    bool BuildModelOnline(ge::Model& irModel, ModelBufferData& output, const BuildOptions& options);
    /*
     * @ingroup domi_omg
     * @brief 离线编译生成模型
     * @return bool 成功返回true，失败返回false
     */
    bool BuildModelOffline(ge::ComputeGraphPtr graph, ge::Model& irModel,
        ModelBufferData& output, const BuildOptions& options);
    /*
     * @ingroup domi_omg
     * @brief 检查Rom版本是否支持IR量化模型
     * @return bool 成功返回true，失败返回false
     */
    bool IsSupportQuantize(const std::string& hiaiVersion);
};
} // namespace hiai

namespace domi {
using hiai::BuildOptions;
using hiai::defaultBuildOptions;
using hiai::ExecuteDeviceSelectMode;
using hiai::HiaiIrBuild;
using hiai::ModelBufferData;
using hiai::OpExecuteDevice;
using hiai::WeightDataType;
} // namespace domi
#endif // HIAI_API_HIAI_IR_BUILD_H
