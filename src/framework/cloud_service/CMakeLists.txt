hi_cc_library_static(
  NAME
    ai::fmk::cloud_service_static
  SRCS
    enhance_utils.cpp
    enhance_c_api.cpp
    cloud_service.cpp
  CDEFS
    GOOGLE_PROTOBUF_NO_RTTI
    PROTOBUF_INLINE_NOT_IN_HEADERS=0
    HIAI_CLOUD_SERVICE_API_VISIABLE
    ENHANCE_EXPORTS
  COPTS
    -frtti
    -std=c++11
)

hi_cc_function(AI_SUPPORT_UPGRADE_APK
  NAME
    ai::fmk::cloud_service_static
  CDEFS
    AI_SUPPORT_UPGRADE_APK
)

hi_cc_library_shared(
  NAME
    ai::fmk::cloud_service_shared
  WHOLE_STATIC_LIBS
    ai::fmk::cloud_service_static
    ai::fmk::model_manager_stats_static
    ai::infra::base::process_util_static
    ai::infra::log
    huawei::c_sec
)

hi_target_output_name(ai::fmk::cloud_service_shared hiai_enhance)