include_guard()

## option range: ON/OFF
function(hi_cc_feature_option feature_name option)
  option(${feature_name} "option for ${feature_name}" ${option})
  if(${option})
    add_compile_definitions(${feature_name})
  endif()
endfunction()