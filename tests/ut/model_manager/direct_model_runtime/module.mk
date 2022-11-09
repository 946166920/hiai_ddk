LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := direct_model_runtime_ut

TESTCASES_FILES := \
    direct_built_model_aipp_ut.cpp \
    direct_built_model_ut.cpp \
    direct_model_builder_ut.cpp \
    direct_model_manager_aipp_ut.cpp \
    direct_model_manager_ut.cpp \
    hiai_model_compatible_ut.cpp \
    plugin_version_ut.cpp \

LOCAL_SRC_FILES := \
    main.cpp \
    $(TESTCASES_FILES) \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/src \
    $(TOPDIR)inc \
    $(TOPDIR)inc/framework \
    $(TOPDIR)api \
    $(TOPDIR)api/infra \
    $(TOPDIR)api/framework \
    $(TOPDIR)inc/framework/compatible \
    $(TOPDIR)inc/framework/tensor \
    $(TOPDIR)inc/infra/mmpa \
    $(TOPDIR)inc/infra \
    $(TOPDIR)inc/infra/om \
    $(TOPDIR)src/framework \
    $(TOPDIR)src/framework/util \
    $(TOPDIR)src/framework/inc \
    $(TOPDIR)inc/framework/util \
    $(TOPDIR)inc/framework/model_manager \
    $(TOPDIR)../../open_source/bounds_checking_function/include/ \
    $(TOPDIR)../../open_source/protobuf/src/ \

LOCAL_SHARED_LIBRARIES := \
    libc_secshared \
    libmmpa \
    libhiai_ddk \
    libhiai_ir_ddk \
    libhiai_model_compatible \

LOCAL_SHARED_LIBRARIES += \
    lib_android_stub \
    libai_infra_log \
    libai_client_stub_ddk \
    lib_ir_graph \

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

LOCAL_CFLAGS += \
    -DAI_SUPPORT_AIPP_API \
    -DFMK_FEATURE_DYNAMIC_SHAPE \
    -DAI_SUPPORT_GRAPH_API \
    -DAI_SUPPORT_BUILT_MODEL_SAVE \
    -DAI_SUPPORT_HCL \
    -DAI_SUPPORT_PREFMODE_EXTEND \
    -DAI_SUPPORT_PRIORITY_EXTEND \
    -DAI_SUPPORT_SPECIAL_3RD_MODEL \

LOCAL_CLASSFILE_RULE := framework_model_manager

include $(BUILD_UT_TEST)
