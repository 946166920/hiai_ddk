#ifndef STUB_LOAD_MODELS_H
#define STUB_LOAD_MODELS_H
#include "compatible/HiAiModelManagerService.h"
#include "compatible/HiAiModelManagerType.h"
#include "model_manager.h"
#include "compatible/AiModelBuilder.h"

using namespace std;
using namespace hiai;

class StubLoadModels {
public:
    void LoadModelsByV1();

    void UnloadModels();

    shared_ptr<AiModelMngerClient> GetClient();

private:
    shared_ptr<AiModelMngerClient> client {nullptr};
};
#endif