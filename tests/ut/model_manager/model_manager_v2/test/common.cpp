#include "common.h"

namespace hiai {
void SetConfigures(ControlFuncParam param)
{
    if (param.expectErrorFuncs.size() != 0) {
        for (auto& func : param.expectErrorFuncs) {
            ControlC::GetInstance().SetExpectStatus(func.first, HIAI_FAILURE, func.second);
        }
    }
}
}