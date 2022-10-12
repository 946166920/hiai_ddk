### define ai::inc::api::graph
add_library(ai_inc_api_graph INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_inc_api_graph
    INTERFACE ${hdrs}
)

target_include_directories(ai_inc_api_graph
    INTERFACE "$<BUILD_INTERFACE:${AI_API_FMK_DIR}>"
)

add_library(ai::inc::api::graph ALIAS ai_inc_api_graph)