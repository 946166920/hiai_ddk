#include "control_client.h"

namespace hiai {
ControlClient& ControlClient::GetInstance()
{
    static ControlClient instance;
    return instance;
}

void ControlClient::Clear()
{
    expectValueMap_.clear();
}

int ControlClient::GetExpectValue(ClientKey key)
{
    if (expectValueMap_.find(key) != expectValueMap_.end()) {
        return expectValueMap_[key];
    }
    return -1;
}

void ControlClient::SetExpectValue(ClientKey key, int expectValue)
{
    expectValueMap_[key] = expectValue;
}
} // namespace hiai