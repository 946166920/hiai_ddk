cmake_minimum_required(VERSION 3.14)

project(hiai VERSION 1.0 LANGUAGES C CXX)

### set c++ standard to c++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### ccache
set_property(GLOBAL APPEND_STRING PROPERTY RULE_LAUNCH_COMPILE "$ENV{CCACHE}")

### python
find_package(Python COMPONENTS Interpreter)

### bp
set_directory_properties(PROPERTIES HI_TARGET_DEFAULT "npu")

### global directory: ${ROOT_DIR} should be absolute path
### otherwise hi_glob::EXCLUDES is not working.
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)
get_filename_component(ROOT_DIR ${ROOT_DIR} ABSOLUTE)

set(AI_ROOT_DIR ${ROOT_DIR}/vendor/hisi/npu)
set(AI_API_DIR ${AI_ROOT_DIR}/api)
set(AI_INC_DIR ${AI_ROOT_DIR}/inc)
set(OPEN_SOURCE_ROOT_DIR ${ROOT_DIR}/vendor/open_source)

## global hi_runtime for gen system
add_custom_target(hi_runtime
  COMMENT "global hi_runtime target"
)

### module path
list(APPEND CMAKE_MODULE_PATH ${AI_ROOT_DIR}/build/core/himake)
list(APPEND CMAKE_MODULE_PATH ${AI_ROOT_DIR}/build/core/himake/modules)
list(APPEND CMAKE_MODULE_PATH ${AI_ROOT_DIR}/build/config)

### hi-make
include(hi_make/hi_cc_headers)
include(hi_make/hi_cc_objects)
include(hi_make/hi_cc_gen_rule)
include(hi_make/hi_cc_gen_target)
include(hi_make/hi_cc_library_static)
include(hi_make/hi_cc_library_shared)
include(hi_make/hi_cc_library_proto)
include(hi_make/hi_cc_prebuilt_library)
include(hi_make/hi_cc_library)
include(hi_make/hi_cc_binary)
include(hi_make/hi_cc_test)
include(hi_make/hi_cc_benchmark)
include(hi_make/hi_cc_package)

include(hi_make/hi_function)
include(hi_make/hi_system)
include(hi_make/hi_generator)
include(hi_make/hi_default)
include(hi_make/hi_filegroup)

### hi-utils
include(hi_utils/hi_glob)
include(hi_utils/hi_target)
include(hi_utils/hi_dir)
include(hi_utils/hi_command)

### preconditions
include(hi_utils/hi_assert)

hi_assert_not_empty(HI_PRODUCT)
hi_assert_not_empty(HI_SOLUTION)
hi_assert_not_empty(HI_PACKAGE)
hi_assert_not_empty(HI_TOOLCHAIN)
hi_assert_not_empty(HI_ABI)
hi_assert_not_empty(CMAKE_BUILD_TYPE)

### redirect output directory
set(output_prefix ${ROOT_DIR}/out/hi_build/${HI_PRODUCT}/${HI_SOLUTION}/${HI_PACKAGE}/${HI_TOOLCHAIN}/${HI_ABI}/${CMAKE_BUILD_TYPE})

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_prefix}/bin)
endif()

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_prefix}/lib)
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_prefix}/lib)
endif()

## build system generators: should before hi_add_subdirectory
hi_cc_gen_target(bp
  TOOLCHAIN aosp
)

hi_cc_gen_target(gn
  TOOLCHAIN hosp
)

hi_cc_gen_target(bazel
  TOOLCHAIN linux-x86
)

## package definitions: should before hi_add_subdirectory
list(APPEND CMAKE_MODULE_PATH ${AI_ROOT_DIR}/build/config/hiai/npu)

include(all_paths)
include(hiai_foundation_ddk)
include(hiai_foundation_ddk_external)
include(hiai_foundation_rom)
include(hiai_foundation_host)
include(hiai_security_foundation_rom)

### external deps
hi_add_subdirectory(build/core/cmake/external)

### define OS_TYPE global
include(hi_utils/hi_os)
hi_set_os_type()

### api
hi_add_subdirectory(api/framework)
hi_add_subdirectory(api/infra)

### inc
hi_add_subdirectory(inc/infra)
hi_add_subdirectory(inc/framework)

### src
hi_add_subdirectory(src/infra)
hi_add_subdirectory(src/framework)
hi_add_subdirectory(src/cls/npucl/driver/user/ree)
hi_add_subdirectory(src/cls/npucl/driver/user/tee)
hi_add_subdirectory(src/cls/npucl/runtime)
hi_add_subdirectory(src/cls/npucl/nn_core/facility)
hi_add_subdirectory(src/cls/npucl/nn_core/tiling)
hi_add_subdirectory(src/cls/cpucl)
hi_add_subdirectory(src/engine/mib)
hi_add_subdirectory(src/engine/cvengine)
hi_add_subdirectory(src/engine/igs/subscribe)
hi_add_subdirectory(src/cls/npucl/nn_core/fusion_engine)
hi_add_subdirectory(src/cls/npucl/nn_core/generated_lib)
hi_add_subdirectory(src/cls/dnnacl)
hi_add_subdirectory(src/cls/npucl/nn_core/elementary_lib)
hi_add_subdirectory(src/cls/npucl/nn_core/graph_executor)
hi_add_subdirectory(src/cls/npucl/nn_core/graph_plan)

hi_add_subdirectory(src/tools/development/model)

## start all gen systems: should be end of file
hi_generate_build_systems()
