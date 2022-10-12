### ai::inc::fmk::common
add_library(ai_inc_fmk_common INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_inc_fmk_common
    INTERFACE ${hdrs}
)

target_link_libraries(ai_inc_fmk_common
    INTERFACE ai::api::fmk::model_builder
              ai::inc::api::graph
)

target_include_directories(ai_inc_fmk_common
    INTERFACE
        "$<BUILD_INTERFACE:${AI_INC_DIR}>"
        "$<BUILD_INTERFACE:${AI_INC_FMK_DIR}>"
)

add_library(ai::inc::fmk::common ALIAS ai_inc_fmk_common)