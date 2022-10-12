### define ai::api::fmk::model_builder
add_library(ai_api_fmk_model_builder INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_api_fmk_model_builder
    INTERFACE ${hdrs}
)

target_include_directories(ai_api_fmk_model_builder
    INTERFACE "$<BUILD_INTERFACE:${AI_API_DIR}/infra>"
              "$<BUILD_INTERFACE:${AI_API_FMK_DIR}>"
)

add_library(ai::api::fmk::model_builder ALIAS ai_api_fmk_model_builder)