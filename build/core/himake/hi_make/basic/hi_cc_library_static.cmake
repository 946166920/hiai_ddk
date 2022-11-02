include_guard()

include(CMakeParseArguments)
include(hi_utils/hi_runtime)
include(hi_utils/hi_glob)
include(hi_utils/hi_message)
include(hi_utils/hi_gen_target)
include(hi_utils/hi_srcs)

function(hi_cc_library_static)
  set(opts)

  set(single_args
    NAME
  )

  set(multi_args
    INCS
    SRCS
    EXCLUDES
    DEPS
    PUB_DEPS
    CDEFS
    COPTS
    COPTS_C
    COPTS_CXX
    LOPTS
    LPATHS
  )

  cmake_parse_arguments(HI_CC_LIBRARY_STATIC "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_LIBRARY_STATIC_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_library_static(): \"${HI_CC_LIBRARY_STATIC_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_LIBRARY_STATIC_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_library_static() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_CC_LIBRARY_STATIC_NAME})

  if(HI_EXCLUDE_FROM_ALL)
    add_library(${target_name} STATIC EXCLUDE_FROM_ALL)
  else()
    add_library(${target_name} STATIC)
  endif()

  if(HI_CC_LIBRARY_STATIC_INCS)
    # reltive paths
    set_property(TARGET ${target_name} 
      PROPERTY HI_INCS ${HI_CC_LIBRARY_STATIC_INCS}
    )

    # foreach(inc IN LISTS HI_CC_LIBRARY_STATIC_INCS)
    #   list(APPEND target_incs ${ROOT_DIR}/${inc})
    # endforeach()

    # absolute paths
    target_include_directories(${target_name}
      PRIVATE
        ${HI_CC_LIBRARY_STATIC_INCS}
    )
  endif()

  if(HI_CC_LIBRARY_STATIC_SRCS)
    hi_set_compile_target_srcs(${target_name} "${HI_CC_LIBRARY_STATIC_SRCS}")
  endif()

  if(HI_CC_LIBRARY_STATIC_DEPS)
    target_link_libraries(${target_name}
      PRIVATE 
        ${HI_CC_LIBRARY_STATIC_DEPS}
    )
  endif()

  # if(HI_CC_LIBRARY_SHARED_PUB_DEPS)
  # target_link_libraries(${target_name}
  #   PUBLIC
  #     ${HI_CC_LIBRARY_SHARED_PUB_DEPS}
  # )
  # endif()

  if(HI_CC_LIBRARY_STATIC_PUB_DEPS)
    target_link_libraries(${target_name}
      PUBLIC
        ${HI_CC_LIBRARY_STATIC_PUB_DEPS}
    )
  endif()

  if(HI_CC_LIBRARY_STATIC_CDEFS)
    target_compile_definitions(${target_name}
      PRIVATE
        ${HI_CC_LIBRARY_STATIC_CDEFS}
    )
  endif()

  if(HI_CC_LIBRARY_STATIC_COPTS)
    target_compile_options(${target_name}
      PRIVATE
        ${HI_CC_LIBRARY_STATIC_COPTS}
    )

    set_target_properties(${target_name}
      PROPERTIES HI_COPTS "${HI_CC_LIBRARY_STATIC_COPTS}"
    )
  endif()

  if(HI_CC_LIBRARY_STATIC_COPTS_C)
    target_compile_options(${target_name}
      PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${HI_CC_LIBRARY_STATIC_COPTS_C}>
    )

    set_target_properties(${target_name}
      PROPERTIES HI_C_ONLY_OPTS "${HI_CC_LIBRARY_STATIC_COPTS_C}"
    )
  endif()

  if(HI_CC_LIBRARY_STATIC_COPTS_CXX)
    target_compile_options(${target_name}
      PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${HI_CC_LIBRARY_STATIC_COPTS_CXX}>
    )

    set_target_properties(${target_name}
      PROPERTIES HI_CXX_ONLY_OPTS "${HI_CC_LIBRARY_STATIC_COPTS_CXX}"
    )
  endif()

  if(HI_CC_LIBRARY_STATIC_LOPTS)
    target_link_options(${target_name}
      PRIVATE 
        ${HI_CC_LIBRARY_STATIC_LOPTS}
    )
  endif()

  if(HI_CC_LIBRARY_STATIC_LPATHS)
    target_link_directories(${target_name}
      PRIVATE 
        ${HI_CC_LIBRARY_STATIC_LPATHS}
    )
  endif()

  hi_runtime_get_target_output(${HI_CC_LIBRARY_STATIC_NAME} output)
  if(output)
    set_target_properties(${target_name}
      PROPERTIES OUTPUT_NAME ${output}
    )
  endif()

  hi_add_to_gen_target(
    NAME
      ${target_name}
  )

  if(NOT HI_CC_LIBRARY_STATIC_NAME STREQUAL target_name)
    add_library(${HI_CC_LIBRARY_STATIC_NAME} ALIAS ${target_name})
  endif()
endfunction()