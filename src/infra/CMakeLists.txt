hi_default(
  NAME
    infra
  INCS
    vendor/hisi/npu/api/infra # api/infra
    vendor/hisi/npu/inc # inc
    vendor/hisi/npu/src/infra
    vendor/hisi/npu/src/framework/inc
)

### sub project
hi_add_subdirectory(dl_helper)
hi_add_subdirectory(log)
hi_add_subdirectory(memory)
hi_add_subdirectory(mmpa)
hi_add_subdirectory(om)
hi_add_subdirectory(rpc)
hi_add_subdirectory(base)
hi_add_subdirectory(memory_manager)
hi_add_subdirectory(math)
