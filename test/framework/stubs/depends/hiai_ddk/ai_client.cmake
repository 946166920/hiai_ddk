
set(LOCAL_SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/../hiai_ddk/src/stub_hiai_model_manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../hiai_ddk/src/control_client.cpp
)

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread -fexceptions -frtti -Wno-unused-result")

set(LIBAI_CLIENT_STUB_DDK ai_client_stub_ddk)
add_library(${LIBAI_CLIENT_STUB_DDK} SHARED ${LOCAL_SRC_FILES})

target_compile_definitions(${LIBAI_CLIENT_STUB_DDK}
    PRIVATE
    _GLIBCXX_USE_CXX11_ABI=0
)

target_include_directories(${LIBAI_CLIENT_STUB_DDK}
    PRIVATE
    ${TOP_DIR}/inc/framework/c
    ${TOP_DIR}/src/framework/tensor/base
    ${THIRD_PARTY_PATH}
    ${THIRD_PARTY_CSEC_PATH}/include
)