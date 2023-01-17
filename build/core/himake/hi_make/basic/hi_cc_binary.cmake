include_guard()

include(CMakePrintHelpers)
include(hi_utils/hi_runtime)
include(hi_utils/hi_message) 
include(hi_utils/hi_srcs)

function(hi_cc_binary)
  set(opts)

  set(single_args
    NAME
  )

  set(multi_args
    INCS
    SRCS
    EXCLUDES
    DEPS
    CDEFS
    COPTS
    COPTS_C
    COPTS_CXX
    LOPTS
    LPATHS
    WHOLE_STATIC_LIBS
    GROUP_STATIC_LIBS
  )

  cmake_parse_arguments(HI_CC_BINARY "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_BINARY_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_binary(): \"${HI_CC_BINARY_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_BINARY_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_binary() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_CC_BINARY_NAME})

  if(HI_EXCLUDE_FROM_ALL)
    add_executable(${target_name} EXCLUDE_FROM_ALL)
  else()
    add_executable(${target_name})
  endif()

  if(HI_CC_BINARY_SRCS)
    hi_set_compile_target_srcs(${target_name} "${HI_CC_BINARY_SRCS}")
  endif()

  if(HI_CC_BINARY_INCS)
     # reltive paths
    set_property(TARGET ${target_name} 
      PROPERTY HI_INCS ${HI_CC_BINARY_INCS}
    )

    foreach(inc IN LISTS HI_CC_BINARY_INCS)
      list(APPEND target_incs ${ROOT_DIR}/${inc})
    endforeach()

    # absolute paths
    target_include_directories(${target_name}
      PRIVATE
        ${target_incs}
    )
  endif()

  if(HI_CC_BINARY_DEPS)
    target_link_libraries(${target_name}
      PRIVATE 
        ${HI_CC_BINARY_DEPS}
    )
  endif()

  if(HI_CC_BINARY_DEFS)
    target_compile_definitions(${target_name}
      PRIVATE 
        ${HI_CC_BINARY_DEFS}
    )
  endif()

  if(HI_CC_BINARY_COPTS)
    target_compile_options(${target_name}
      PRIVATE 
        ${HI_CC_BINARY_COPTS}
    )

    set_target_properties(${target_name}
      PROPERTIES HI_COPTS "${HI_CC_BINARY_COPTS}"
    )
  endif()

  if(HI_CC_BINARY_COPTS_C)
    target_compile_options(${target_name}
      PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${HI_CC_BINARY_COPTS_C}>
    )

    set_target_properties(${target_name}
      PROPERTIES HI_C_ONLY_OPTS "${HI_CC_BINARY_COPTS_C}"
    )
  endif()

  if(HI_CC_BINARY_COPTS_CXX)
    target_compile_options(${target_name}
      PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${HI_CC_BINARY_COPTS_CXX}>
    )

    set_target_properties(${target_name}
      PROPERTIES HI_CXX_ONLY_OPTS "${HI_CC_BINARY_COPTS_CXX}"
    )
  endif()

  if(HI_CC_BINARY_LOPTS)
    target_link_options(${target_name}
      PRIVATE 
        ${HI_CC_BINARY_LOPTS}
    )
  endif()

  if(HI_CC_BINARY_LPATHS)
    target_link_directories(${target_name}
      PRIVATE 
        ${HI_CC_BINARY_LPATHS}
    )
  endif()

  hi_runtime_get_target_output(${HI_CC_BINARY_NAME} output)
  if(output)
    set_target_properties(${target_name}
      PROPERTIES OUTPUT_NAME ${output}
    )
  endif()

  if(HI_CC_BINARY_WHOLE_STATIC_LIBS)
    target_link_libraries(${target_name} 
      PRIVATE -Wl,--whole-archive ${HI_CC_BINARY_WHOLE_STATIC_LIBS} -Wl,--no-whole-archive
    )
    set_target_properties(${target_name}
      PROPERTIES HI_WHOLE_STATIC_LIBS "${HI_CC_BINARY_WHOLE_STATIC_LIBS}" 
    )
  endif()

  if(HI_CC_BINARY_GROUP_STATIC_LIBS)
    target_link_libraries(${target_name} 
      -Wl,--start-group ${HI_CC_BINARY_GROUP_STATIC_LIBS} -Wl,--end-group
    )
    set_target_properties(${target_name}
      PROPERTIES HI_GROUP_STATIC_LIBS "${HI_CC_BINARY_GROUP_STATIC_LIBS}" 
    )
  endif()

  hi_add_to_gen_target(
    NAME
      ${target_name}
  )

  if(NOT HI_CC_BINARY_NAME STREQUAL target_name)
    add_executable(${HI_CC_BINARY_NAME} ALIAS ${target_name})
  endif()
endfunction()