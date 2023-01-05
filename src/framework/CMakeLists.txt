add_compile_definitions(_FORTIFY_SOURCE=2 HOST_VISIBILITY GRAPH_API_VISIABLE)

## hiai_ir main sub project
hi_add_subdirectory(common)
hi_add_subdirectory(compatible)
hi_add_subdirectory(graph)
hi_add_subdirectory(om)

## hiai main sub project
hi_add_subdirectory(infra/buffer)
hi_add_subdirectory(model)
hi_add_subdirectory(model_builder)
hi_add_subdirectory(model_manager)
hi_add_subdirectory(model_runtime)
hi_add_subdirectory(tensor)
hi_add_subdirectory(util)
hi_add_subdirectory(cloud_service)