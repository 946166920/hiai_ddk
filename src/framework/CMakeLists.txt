hi_default(
  NAME
    fmk
  INCS
    vendor/hisi/npu/api/framework
    vendor/hisi/npu/api/infra
    vendor/hisi/npu/inc
    vendor/hisi/npu/src/framework
    vendor/hisi/npu/src/framework/inc
)

### directory define
set(AI_SRC_FMK_DIR ${CMAKE_CURRENT_LIST_DIR})

add_compile_definitions(_FORTIFY_SOURCE=2 HOST_VISIBILITY GRAPH_API_VISIABLE)

hi_add_subdirectory(inc)

### hiai_ir main sub project
hi_add_subdirectory(om)
hi_add_subdirectory(common)
hi_add_subdirectory(graph)
hi_add_subdirectory(compatible)

### hiai main sub project
hi_add_subdirectory(kernel_executor)
hi_add_subdirectory(model)
hi_add_subdirectory(dedicated_compute)
hi_add_subdirectory(model_builder)
hi_add_subdirectory(model_manager)
hi_add_subdirectory(model_runtime)
hi_add_subdirectory(tensor)
hi_add_subdirectory(infra)
hi_add_subdirectory(util)
hi_add_subdirectory(pipe)
hi_add_subdirectory(v100_compute)
hi_add_subdirectory(general_compute)
hi_add_subdirectory(cl_manager)
hi_add_subdirectory(partition)
hi_add_subdirectory(optimizer)
hi_add_subdirectory(perf_manager)
hi_add_subdirectory(omg)
hi_add_subdirectory(security_ctrl)
hi_add_subdirectory(initializer)
hi_add_subdirectory(model_util)
hi_add_subdirectory(complete)
hi_add_subdirectory(parser)
hi_add_subdirectory(crypto)
hi_add_subdirectory(crypto/model_crypto)
hi_add_subdirectory(dedicated_model_manager)
hi_add_subdirectory(cls)
hi_add_subdirectory(special_3rd_compute)
hi_add_subdirectory(cloud_service)
hi_add_subdirectory(model_debugger)

### hiai tee sub project
if(HI_SYSTEM_NAME STREQUAL HI_LINUX_TEE)
hi_add_subdirectory(security)
endif()
