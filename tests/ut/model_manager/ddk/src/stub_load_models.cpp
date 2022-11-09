#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include "stub_load_models.h"
#include "model_manager.h"

using namespace std;
using namespace hiai;

void StubLoadModels::LoadModelsByV1()
{
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    client = make_shared<AiModelMngerClient>();
    client->Init(nullptr);

    string modelName1 = "tf_softmax_infershaped";
    string modelPath1 = "../bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    MemBuffer* buffer1 = builder->InputMemBufferCreate(modelPath1);
    if (buffer1 == nullptr) {
        std::cout << modelPath1 << " open failed" << std::endl;
        return;
    }

    shared_ptr<AiModelDescription> desc1 = make_shared<AiModelDescription>(modelName1, 3, 0, 0, 0);
    desc1->SetModelBuffer(buffer1->GetMemBufferData(), buffer1->GetMemBufferSize());

    string modelName2 = "tf_softmax_no_infershaped";
    const string modelPath2 = "../bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om";
    MemBuffer* buffer2 = builder->InputMemBufferCreate(modelPath2);
    if (buffer2 == nullptr) {
        std::cout << modelPath2 << " open failed" << std::endl;
        return;
    }
    shared_ptr<AiModelDescription> desc2 = make_shared<AiModelDescription>(modelName2, 3, 0, 0, 0);
    TensorDimension dims(1,2,3,4);
    std::vector<TensorDimension> inputDims = {dims};
    // desc2->SetInputDims(inputDims);
    desc2->SetModelBuffer(buffer2->GetMemBufferData(), buffer2->GetMemBufferSize());

    vector<shared_ptr<AiModelDescription>> modelDescs1;
    vector<shared_ptr<AiModelDescription>> modelDescs2;
    modelDescs1.push_back(desc1);
    modelDescs2.push_back(desc2);

    client->Load(modelDescs1);
    client->Load(modelDescs2);
    builder->MemBufferDestroy(buffer1);
    builder->MemBufferDestroy(buffer2);
}

void StubLoadModels::UnloadModels()
{
    // MockExecutorManager mockExecutorManager;
    // EXPECT_CALL(mockExecutorManager, UnloadModel(_)).WillRepeatedly(Return(0));
    if (client != nullptr) {
        client->UnLoadModel();
    }
}

shared_ptr<AiModelMngerClient> StubLoadModels::GetClient()
{
    return client;
}