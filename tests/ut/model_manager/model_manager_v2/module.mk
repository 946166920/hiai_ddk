LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := model_manager_v2_ut

FRAMEWORK_BASE_DIR_FOR_SRC_FILES := ../../../../../src/framework

TEST_FILES := \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/model_builder/ir/build/hiai_ir_build.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/model_builder/compatible/BuildOptionUtil.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/model/aipp/aipp_input_converter.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/model/built_model/built_model_impl.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/model/built_model/customdata_util.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/model_builder/om/model_builder_impl.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/model_builder/om/model_build_options_util.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/model_manager/core/model_manager_impl.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/infra/buffer/local_buffer.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/infra/buffer/base_buffer.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/infra/buffer/hiai_native_handle.c \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/util/hiai_foundation_dl_helper.c \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/util/file_util.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/util/hiai_base_types.c \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/util/model_type_util.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/base/nd_tensor_buffer_impl.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/base/hiai_nd_tensor_buffer.c \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/base/hiai_nd_tensor_buffer_legacy.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/base/hiai_nd_tensor_buffer_local.c \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/base/hiai_nd_tensor_buffer_util.c \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/aipp/aipp_para_impl.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/aipp/hiai_tensor_aipp_para.c \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/aipp/hiai_tensor_aipp_para_legacy.c \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/aipp/hiai_tensor_aipp_para_local.c \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/image/image_tensor_buffer.cpp \
    $(FRAMEWORK_BASE_DIR_FOR_SRC_FILES)/tensor/image/image_tensor_buffer_impl.cpp \
    stub/control_c.cpp \
    stub/stub_hiai_built_model.cpp \
    stub/stub_hiai_built_model_aipp.c \
    stub/stub_hiai_model_manager.cpp \
    stub/stub_hiai_model_manager_types.c \
    stub/stub_hiai_model_build_types.cpp \
    stub/stub_hiai_model_builder.cpp \
    stub/stub_hiai_nd_tensor_desc.cpp \
    stub/stub_model_manager_listener.cpp \

TESTCASES_FILES := \
    test/built_model_ut.cpp \
    test/model_manager_ut.cpp \
    test/model_manager_aipp_ut.cpp \
    test/static_shape_ut.cpp \
    test/dynamic_shape_ut.cpp \
    test/model_device_config_ut.cpp \
    test/hiai_ir_build_ut.cpp \
    test/load_model_ut.cpp \

LOCAL_SRC_FILES := \
    main.cpp \
    test/common.cpp \
    $(TEST_FILES) \
    $(TESTCASES_FILES) \

LOCAL_C_INCLUDES := \
    $(TOPDIR)../../open_source/bounds_checking_function/include \
    $(TOPDIR)api/framework \
    $(TOPDIR)api/infra \
    $(TOPDIR)inc \
    $(TOPDIR)src/framework \
    $(TOPDIR)src/framework/inc \
    $(TOPDIR)test/framework/ut/model_manager/model_manager_v2/stub \
    $(TOPDIR)test/framework/stubs/depends/hiai_ddk/src \

LOCAL_SHARED_LIBRARIES += \
    libc_secshared \
    libmmpa \
    libai_infra_log \
    libai_client_stub_ddk \
    libhiai_ir_ddk \
    libhiai_ir_build_aipp_ddk \

LOCAL_CPPFLAGS := \
    -std=c++14 \
    -pthread \
    -fPIC \
    -frtti \
    -Os \
    -fpermissive \
    -ffunction-sections \
    -fdata-sections \
    -fno-asynchronous-unwind-tables \
    -fomit-frame-pointer \
    -fno-unwind-tables \
    -fno-access-control \
    -DHIAI_DDK \
    -D_FORTIFY_SOURCE=2 \
    -DHAVE_PTHREAD \
    -DHOST_VISIBILITY \
    -DFMK_FEATURE_DYNAMIC_SHAPE \

LOCAL_CFLAGS += \
    -std=c99 \
    -DAI_SUPPORT_AIPP_API \
    -DAI_SUPPORT_GRAPH_API \
    -DAI_SUPPORT_BUILT_MODEL_SAVE \
    -DAI_SUPPORT_SPECIAL_3RD_MODEL \
    -DAI_SUPPORT_CL_CUSTOMIZATION \
    -DHIAI_UTIL_API_EXPORT \
    -DANDROID \
    -D_Float16=uint16_t \

LOCAL_CFLAGS += -fexceptions -g -fprofile-arcs -ftest-coverage
LOCAL_CPPFLAGS += -fexceptions -g -fprofile-arcs -ftest-coverage
LOCAL_LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov

LOCAL_CLASSFILE_RULE := framework_model_manager

include $(BUILD_UT_TEST)
