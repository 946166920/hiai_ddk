include_guard()

include(CMakeParseArguments)
include(hi_utils/hi_message)

function(hi_cc_library)
  set(opts)

  set(single_args
    NAME
  )

  # format: switch -> impl
  set(multi_args
    IMPLS
  )

  cmake_parse_arguments(HI_CC_LIBRARY "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_LIBRARY_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_library(): \"${HI_CC_LIBRARY_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_LIBRARY_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_library() function")
    return()
  endif()

  set(has_valid_impl 0)

  list(LENGTH HI_CC_LIBRARY_IMPLS impls_len)
  math(EXPR is_odd "${impls_len} % 2")
  if(NOT is_odd EQUAL 0)
    hi_message(FATAL_ERROR "keyword OPTIONAL_DEPS num should be even for hi_package() function")
    return()
  endif()

  math(EXPR impls_len "${impls_len}")
  foreach(i RANGE 0 ${impls_len} 2)
    if(i EQUAL ${impls_len})
      break()
    endif()

    list(GET HI_CC_LIBRARY_IMPLS ${i} cond)

    math(EXPR value_idx "${i} + 1")
    list(GET HI_CC_LIBRARY_IMPLS ${value_idx} impl_name)
    string(REPLACE :: _ impl_name ${impl_name})

    if(${${cond}})
      add_library(${HI_CC_LIBRARY_NAME} ALIAS ${impl_name})

      string(REPLACE :: _ target_name ${HI_CC_LIBRARY_NAME})
      if(NOT HI_CC_LIBRARY_NAME STREQUAL target_name)
        add_library(${target_name} ALIAS ${impl_name})
      endif()
      hi_runtime_append_abstract_target(${HI_CC_LIBRARY_NAME})
      set(has_valid_impl 1)
    else()
      hi_runtime_append_not_used_target(${impl_name})
    endif()
  endforeach()

  if(has_valid_impl EQUAL 0)
    hi_message(FATAL_ERROR "abstract must specify one implementation.")
    return()
  endif()

  hi_add_to_gen_target(
    NAME
      ${HI_CC_LIBRARY_NAME}
  )
endfunction()