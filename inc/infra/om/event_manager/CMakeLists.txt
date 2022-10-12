# ai::inc::infra::om::event_manager

add_library(ai_inc_infra_om_event_manager INTERFACE)

file(GLOB_RECURSE hdrs CONFIGURE_DEPENDS
    *.h
)

target_sources(ai_inc_infra_om_event_manager
    INTERFACE ${hdrs}
)

target_include_directories(ai_inc_infra_om_event_manager
    INTERFACE
        "$<BUILD_INTERFACE:${AI_INC_DIR}>"
)

add_library(ai::inc::infra::om::event_manager ALIAS ai_inc_infra_om_event_manager)