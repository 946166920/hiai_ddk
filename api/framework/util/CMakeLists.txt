### define ai::api::fmk::util
add_library(ai_api_fmk_util INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_api_fmk_util
    INTERFACE ${hdrs}
)

target_include_directories(ai_api_fmk_util
    INTERFACE "$<BUILD_INTERFACE:${AI_API_DIR}/infra>"
              "$<BUILD_INTERFACE:${AI_API_FMK_DIR}>"
)

add_library(ai::api::fmk::util ALIAS ai_api_fmk_util)