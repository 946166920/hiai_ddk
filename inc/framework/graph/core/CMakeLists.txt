# ai::inc::fmk::graph::core
add_library(ai_inc_fmk_graph_core INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_inc_fmk_graph_core
    INTERFACE ${hdrs}
)

target_include_directories(ai_inc_fmk_graph_core
    INTERFACE "$<BUILD_INTERFACE:${AI_INC_DIR}>"
)

add_library(ai::inc::fmk::graph::core ALIAS ai_inc_fmk_graph_core)