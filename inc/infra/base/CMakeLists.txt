# ai::inc::infra::base
set(AI_INC_INFRA_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})

add_library(ai_inc_infra_base INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_inc_infra_base
    INTERFACE ${hdrs}
)


target_include_directories(ai_inc_infra_base
    INTERFACE
        "$<BUILD_INTERFACE:${AI_INC_INFRA_DIR}>"
        "$<BUILD_INTERFACE:${AI_INC_INFRA_BASE_DIR}>"
        "$<BUILD_INTERFACE:${AI_INC_DIR}>"
)

target_link_libraries(ai_inc_infra_base
   INTERFACE ai::inc::infra::alias
)

add_library(ai::inc::infra::base ALIAS ai_inc_infra_base)