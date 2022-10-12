### ai::inc::fmk::compatible
add_library(ai_inc_fmk_compatible INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_inc_fmk_compatible
    INTERFACE ${hdrs}
)

target_include_directories(ai_inc_fmk_compatible
    INTERFACE "$<BUILD_INTERFACE:${AI_INC_DIR}>"
)

add_library(ai::inc::fmk::compatible ALIAS ai_inc_fmk_compatible)