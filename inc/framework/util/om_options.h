/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: om options
 *
 */
#ifndef _OM_OPTIONS_H_
#define _OM_OPTIONS_H_

#include <string>
#include <map>

namespace ge {
const char* const OM_MODEL_NAME = "model_name";
const char* const OM_ENABLE_PMU = "open_pmu";
enum class OMType { NONE, PROFILING, DUMP };

enum class OMAction {
    ACTION_OFF = 0,
    ACTION_ON = 1,
    ACTION_INVALID,
};

struct OMOptions {
    OMType type = OMType::NONE;
    OMAction action = OMAction::ACTION_OFF;
    std::map<std::string, std::string> config;
};
} // namespace ge
#endif
