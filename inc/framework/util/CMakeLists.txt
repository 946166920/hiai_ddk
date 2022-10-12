# ai::inc::fmk::util
add_library(ai_inc_fmk_util INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_inc_fmk_util
    INTERFACE ${hdrs}
)

target_include_directories(ai_inc_fmk_util
    INTERFACE "$<BUILD_INTERFACE:${AI_INC_DIR}>"
)

target_link_libraries(ai_inc_fmk_util
    INTERFACE ai::api::fmk::util
)

add_library(ai::inc::fmk::util ALIAS ai_inc_fmk_util)
