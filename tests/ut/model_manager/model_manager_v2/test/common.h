#ifndef V2_COMMON_H
#define V2_COMMON_H

#include "control_c.h"
#include "base/error_types.h"

namespace hiai {
struct ControlFuncParam {
    std::vector<std::pair<CKey, int>> expectErrorFuncs;
    Status expectStatus;
};

void SetConfigures(ControlFuncParam param);
}
#endif