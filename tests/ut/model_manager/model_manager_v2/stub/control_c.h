#ifndef CONTROL_C_H
#define CONTROL_C_H

#include <map>
#include <string>
#include "framework/c/hiai_error_types.h"
#include "framework/c/hiai_model_builder_types.h"
#include "framework/c/hiai_model_manager_types.h"
#include "model_builder/model_builder_types.h"
#include "model_manager/model_manager_types.h"

namespace hiai {

enum CKey {
    C_BUILD_OPTIONS_CREATE,
    C_ND_TENSOR_DESC_CREATE,
    C_BUILD_OPTIONS_DYNAMIC_SHAPE_CONFIG_CREATE,
    C_RUN_AIPP_MODEL_V2_FAILED,
    C_BUILD_OPTIONS_OPDEVICE_CREATE,
};

class ControlC {
public:
    static ControlC& GetInstance();

    void Clear();

    HIAI_Status GetExpectStatus(CKey key);
    void SetExpectStatus(CKey key, HIAI_Status expectStatus = HIAI_FAILURE, int witchTimeError = -1);
    void SetBuildOptions(ModelBuildOptions& buildOptions);
    bool CheckBuildOptions(const HIAI_ModelBuildOptions* options);

    void SetInitOptions(ModelInitOptions& initOptions);
    bool CheckInitOptions(const HIAI_ModelInitOptions* options);

    void ChangeNowTimes();
    int GetNowTime();

private:
    std::map<CKey, HIAI_Status> expectStatusMap_;
    std::map<CKey, int> expectWhichTimeErrorMap_;
    int nowTime_ {0};
    ModelBuildOptions expectBuildOptions_;
    ModelInitOptions expectInitOptions_;
};
} // namespace hiai
#endif // CONTROL_CLIENT_H