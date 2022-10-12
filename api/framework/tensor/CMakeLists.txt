### define ai::api::fmk::tensor
add_library(ai_api_fmk_tensor INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_api_fmk_tensor
    INTERFACE ${hdrs}
)

target_include_directories(ai_api_fmk_tensor
    INTERFACE "$<BUILD_INTERFACE:${AI_API_DIR}/infra>"
              "$<BUILD_INTERFACE:${AI_API_FMK_DIR}>"
)

add_library(ai::api::fmk::tensor ALIAS ai_api_fmk_tensor)