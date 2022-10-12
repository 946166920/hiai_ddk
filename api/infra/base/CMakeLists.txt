### define ai::api::infra::base
add_library(ai_api_infra_base INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_api_infra_base
    INTERFACE ${hdrs}
)

target_include_directories(ai_api_infra_base
    INTERFACE "$<BUILD_INTERFACE:${AI_API_DIR}>"
              "$<BUILD_INTERFACE:${AI_API_DIR}/infra>"
)

add_library(ai::api::infra::base ALIAS ai_api_infra_base)