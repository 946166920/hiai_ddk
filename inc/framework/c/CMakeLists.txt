### ai::inc::fmk::c
add_library(ai_inc_fmk_c INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_inc_fmk_c
    INTERFACE ${hdrs}
)

target_include_directories(ai_inc_fmk_c
    INTERFACE "$<BUILD_INTERFACE:${AI_INC_DIR}>"
)

add_library(ai::inc::fmk::c ALIAS ai_inc_fmk_c)