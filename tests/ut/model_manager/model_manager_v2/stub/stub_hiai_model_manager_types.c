#include "framework/c/hiai_model_manager_types.h"
#include "securec.h"
#include "framework/infra/log/log.h"

typedef struct HIAI_ModelInitOptions {
    HIAI_PerfMode perfMode;
    HIAI_ModelBuildOptions* buildOptions;
} HIAI_ModelInitOptions;

HIAI_ModelInitOptions* HIAI_ModelInitOptions_Create(void)
{
    HIAI_ModelInitOptions* option = (HIAI_ModelInitOptions*)malloc(sizeof(HIAI_ModelInitOptions));
    MALLOC_NULL_CHECK_RET_VALUE(option, NULL);
    (void)memset_s(option, sizeof(HIAI_ModelInitOptions), 0, sizeof(HIAI_ModelInitOptions));

    option->perfMode = HIAI_PERFMODE_NORMAL;
    option->buildOptions = NULL;
    return option;
}

void HIAI_ModelInitOptions_SetPerfMode(HIAI_ModelInitOptions* options, HIAI_PerfMode devPerf)
{
    if (options == NULL) {
        return;
    }
    if (devPerf < HIAI_PERFMODE_UNSET || devPerf > HIAI_PERFMODE_EXTREME) {
        FMK_LOGW("devPerf is invalid. set devPerf is HIAI_PERFMODE_NORMAL");
        options->perfMode = HIAI_PERFMODE_NORMAL;
        return;
    }
    options->perfMode = devPerf;
}

HIAI_PerfMode HIAI_ModelInitOptions_GetPerfMode(const HIAI_ModelInitOptions* options)
{
    return options == NULL ? HIAI_PERFMODE_UNSET : options->perfMode;
}

void HIAI_ModelInitOptions_SetBuildOptions(HIAI_ModelInitOptions* options, HIAI_ModelBuildOptions* buildOptions)
{
    if (options == NULL) {
        return;
    }

    if (options->buildOptions != NULL) {
        FMK_LOGW("buildOptions set repeatedly.");
        return;
    }
    options->buildOptions = buildOptions;
}

HIAI_ModelBuildOptions* HIAI_ModelInitOptions_GetBuildOptions(HIAI_ModelInitOptions* options)
{
    return options == NULL ? NULL : options->buildOptions;
}

void HIAI_ModelInitOptions_Destroy(HIAI_ModelInitOptions** options)
{
    if (options == NULL || *options == NULL) {
        return;
    }

    HIAI_ModelInitOptions* tmp = (HIAI_ModelInitOptions*)(*options);
    if (tmp->buildOptions != NULL) {
        HIAI_ModelBuildOptions_Destroy(&tmp->buildOptions);
    }

    free(*options);
    *options = NULL;
}
