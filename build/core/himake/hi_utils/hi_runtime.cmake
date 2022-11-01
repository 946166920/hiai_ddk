
include_guard()

include(hi_utils/hi_target)

if(TARGET hi_runtime)
  hi_message(STATUS "hi_runtime is exist")
else()
  add_custom_target(hi_runtime
    COMMENT "global hi_runtime target"
  )
endif()

# Example 1:
# target_name: ai::infra::log_shared -> ai_infra_log_shared
# 
# NOTE: ${target_name} is concert name but not abstract name. 
#
function(hi_runtime_append_target target_name target_output)
  string(REPLACE :: _ target_name ${target_name}) # ai_infra_log_shared

  if(NOT ${target_name} STREQUAL ${target_output})
    set_target_properties(hi_runtime PROPERTIES
      ${target_name}_OUTPUT ${target_output}
    )
  endif()
endfunction()

function(hi_runtime_append_abstract_target target_name)
  string(REPLACE :: _ target_name ${target_name})

  set_target_properties(hi_runtime PROPERTIES
    ${target_name}_IS_ABSTRACT TRUE
  )
endfunction()

function(hi_runtime_contains_abstract_target target_name is_abstract)
  string(REPLACE :: _ target_name ${target_name})

  hi_get_target_property(hi_runtime ${target_name}_IS_ABSTRACT hi_is_abstract)
  
  set(${is_abstract} ${hi_is_abstract} PARENT_SCOPE)
endfunction()

function(hi_runtime_append_not_used_target target_name)
  set_target_properties(hi_runtime
    PROPERTIES ${target_name}_IS_NOT_USED TRUE
  )
endfunction()

function(hi_runtime_is_not_used_target target_name is_not_used)
  string(REPLACE :: _ target_name ${target_name})
  hi_get_target_property(hi_runtime ${target_name}_IS_NOT_USED hi_is_not_used)

  set(${is_not_used} ${hi_is_not_used} PARENT_SCOPE)
endfunction()

# Example 1:
# target_name: ai::infra::log_shared -> ai_infra_log_shared
# 
# NOTE: ${target_name} is concert name but not abstract name.
#
function(hi_runtime_get_target_output target_name output)
  string(REPLACE :: _ target_name ${target_name})
  hi_get_target_property(hi_runtime ${target_name}_OUTPUT hi_output)
  set(${output} ${hi_output} PARENT_SCOPE)
endfunction()
