# ai::inc::fmk::infra::log
add_library(ai_inc_fmk_infra_log INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_inc_fmk_infra_log
    INTERFACE ${hdrs}
)

target_include_directories(ai_inc_fmk_infra_log
    INTERFACE "$<BUILD_INTERFACE:${AI_INC_DIR}>"
)

add_library(ai::inc::fmk::infra::log ALIAS ai_inc_fmk_infra_log)