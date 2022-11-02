include_guard()

include(hi_utils/hi_message)

function(hi_cc_default)
  set(single_args
    NAME
  )

  set(multi_args
    INCS
  )

  cmake_parse_arguments(HI_CC_DEFAULT "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_DEFAULT_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_default(): \"${HI_CC_DEFAULT_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_DEFAULT_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_default() function")
    return()
  endif()

  ## target
  string(REPLACE "::" "_" target_name ${HI_CC_DEFAULT_NAME})

  if(TARGET ${target_name})
    hi_message(FATAL_ERROR "hi_cc_default can only define once in single file.")
    return()
  endif()

  add_library(${target_name} MODULE EXCLUDE_FROM_ALL)

  set_target_properties(${target_name}
    PROPERTIES HI_TYPE HI_CC_DEFAULT
  )

  set_directory_properties(PROPERTIES HI_TARGET_DEFAULT ${target_name})

  if(HI_CC_DEFAULT_INCS)
    include_directories("${HI_CC_DEFAULT_INCS}")

    foreach(inc IN LISTS HI_CC_DEFAULT_INCS)
      get_filename_component(real_inc ${inc} ABSOLUTE)
      list(APPEND real_incs ${real_inc})
    endforeach()

    set_target_properties(${target_name} PROPERTIES
      HI_INCS ${real_incs}
    )
  endif()

  hi_enable_empty_srcs(
    NAME
      ${target_name}
  )

  hi_add_to_gen_target(
    NAME
      ${target_name}
  )

  if(NOT HI_CC_DEFAULT_NAME STREQUAL target_name)
    add_library(${HI_CC_DEFAULT_NAME} ALIAS ${target_name})
  endif()
endfunction()