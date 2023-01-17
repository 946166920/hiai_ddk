include_guard()

include(CMakeParseArguments)

include(hi_utils/hi_message)

function(hi_cc_headers)
  set(opts)

  set(single_args
    NAME
  )

  set(multi_args
    INCS
    DEPS
  )

  cmake_parse_arguments(HI_CC_HEADERS "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_HEADERS_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_headers(): \"${HI_CC_HEADERS_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_HEADERS_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_headers() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_CC_HEADERS_NAME})

  add_library(${target_name} INTERFACE)

  if(HI_CC_HEADERS_INCS)
    set_property(TARGET ${target_name}
       PROPERTY INTERFACE_HI_INCS
        ${HI_CC_HEADERS_INCS}
    )
    if(NOT {HI_CC_HEADERS_INCS} STREQUAL '.')
      foreach(inc IN LISTS HI_CC_HEADERS_INCS)
      string(REPLACE :: _ inc ${inc})
        if(TARGET ${inc})
          hi_get_target_property(${inc} HI_GEN_INCS gen_inc)
          if(NOT gen_inc)
            hi_message(FATAL_ERROR "target ${inc} need contain HI_GEN_INCS value")
          endif()
          list(APPEND target_incs ${gen_inc})
          add_dependencies(${target_name} ${inc})
        else()
          list(APPEND target_incs ${ROOT_DIR}/${inc})
        endif()
      endforeach()
    endif()

    # absolute paths
    target_include_directories(${target_name}
      INTERFACE
        ${target_incs}
    )
  endif()

  if(HI_CC_HEADERS_DEPS)
    target_link_libraries(${target_name}
      INTERFACE 
        ${HI_CC_HEADERS_DEPS}
    )
  endif()

  if(NOT HI_CC_HEADERS_NAME STREQUAL target_name)
    add_library(${HI_CC_HEADERS_NAME} ALIAS ${target_name})
  endif()
endfunction()