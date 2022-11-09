#ifndef CONTROL_CLIENT_H
#define CONTROL_CLIENT_H

#include <map>
#include <string>

namespace hiai {

enum ClientKey {
    CLIENT_PERF_MODE,
    CLIENT_ImageBuffer_create,
    CLIENT_GetHandleInfo_From_BufferHandle,
    CLIENT_HIAI_MemBuffer_createV2,
};
class ControlClient {
public:
    static ControlClient& GetInstance();

    void Clear();

    int GetExpectValue(ClientKey key);

    void SetExpectValue(ClientKey key, int expectValue);

private:
    std::map<ClientKey, int> expectValueMap_;
};
} // namespace hiai
#endif // CONTROL_CLIENT_H