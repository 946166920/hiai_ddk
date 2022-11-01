include_guard()

include(CMakeParseArguments)
include(hi_utils/hi_runtime)
include(hi_utils/hi_glob)
include(hi_utils/hi_message)
include(hi_utils/hi_srcs)
include(hi_make/extend/hi_cc_prebuilt_library)

function(hi_cc_library_shared)
  set(opts)

  set(single_args
    NAME
  )

  set(multi_args
    INCS
    SRCS
    EXCLUDES
    DEPS
    PUB_DEPS # TODO：can remove?
    CDEFS
    COPTS
    COPTS_C
    COPTS_CXX
    LOPTS
    LPATHS
    WHOLE_STATIC_LIBS
    GROUP_STATIC_LIBS
    EXCLUDE_STATIC_LIBS
  )

  cmake_parse_arguments(HI_CC_LIBRARY_SHARED "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_LIBRARY_SHARED_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_library_shared(): \"${HI_CC_LIBRARY_SHARED_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_LIBRARY_SHARED_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_library_shared() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_CC_LIBRARY_SHARED_NAME})

  if(HI_EXCLUDE_FROM_ALL)
    add_library(${target_name} SHARED EXCLUDE_FROM_ALL)
  else()
    add_library(${target_name} SHARED)
  endif()

  if(HI_CC_LIBRARY_SHARED_INCS)
    # reltive paths
    set_property(TARGET ${target_name}
      PROPERTY HI_INCS ${HI_CC_LIBRARY_SHARED_INCS}
    )

    foreach(inc IN LISTS HI_CC_LIBRARY_SHARED_INCS)
      list(APPEND target_incs ${ROOT_DIR}/${inc})
    endforeach()

    # absolute paths
    target_include_directories(${target_name}
      PRIVATE
        ${target_incs}
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_SRCS)
    hi_set_compile_target_srcs(${target_name} "${HI_CC_LIBRARY_SHARED_SRCS}")
  endif()

  if(HI_CC_LIBRARY_SHARED_DEPS)
    target_link_libraries(${target_name}
      PRIVATE
        ${HI_CC_LIBRARY_SHARED_DEPS}
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_PUB_DEPS)
    target_link_libraries(${target_name}
      PUBLIC
        ${HI_CC_LIBRARY_SHARED_PUB_DEPS}
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_CDEFS)
    target_compile_definitions(${target_name}
      PRIVATE
        ${HI_CC_LIBRARY_SHARED_CDEFS}
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_COPTS)
    target_compile_options(${target_name}
      PRIVATE 
        ${HI_CC_LIBRARY_SHARED_COPTS}
    )
    set_target_properties(${target_name}
      PROPERTIES HI_COPTS "${HI_CC_LIBRARY_SHARED_COPTS}"
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_COPTS_C)
    target_compile_options(${target_name}
      PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${HI_CC_LIBRARY_SHARED_COPTS_C}>
    )

    set_target_properties(${target_name}
      PROPERTIES HI_C_ONLY_OPTS "${HI_CC_LIBRARY_SHARED_COPTS_C}"
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_COPTS_CXX)
    target_compile_options(${target_name}
      PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${HI_CC_LIBRARY_SHARED_COPTS_CXX}>
    )

    set_target_properties(${target_name}
      PROPERTIES HI_CXX_ONLY_OPTS "${HI_CC_LIBRARY_SHARED_COPTS_CXX}"
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_LOPTS)
    target_link_options(${target_name}
      PRIVATE 
        ${HI_CC_LIBRARY_SHARED_LOPTS}
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_LPATHS)
    target_link_directories(${target_name}
      PRIVATE 
        ${HI_CC_LIBRARY_SHARED_LPATHS}
    )
  endif()

  hi_runtime_get_target_output(${HI_CC_LIBRARY_SHARED_NAME} output)
  if(output)
    set_target_properties(${target_name}
      PROPERTIES OUTPUT_NAME ${output}
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_WHOLE_STATIC_LIBS)
    target_link_libraries(${target_name}
      PRIVATE -Wl,--whole-archive ${HI_CC_LIBRARY_SHARED_WHOLE_STATIC_LIBS} -Wl,--no-whole-archive
    )
    set_target_properties(${target_name}
      PROPERTIES HI_WHOLE_STATIC_LIBS "${HI_CC_LIBRARY_SHARED_WHOLE_STATIC_LIBS}" 
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_GROUP_STATIC_LIBS)
    target_link_libraries(${target_name}
      -Wl,--start-group ${HI_CC_LIBRARY_SHARED_GROUP_STATIC_LIBS} -Wl,--end-group
    )
    set_target_properties(${target_name}
      PROPERTIES HI_GROUP_STATIC_LIBS "${HI_CC_LIBRARY_SHARED_GROUP_STATIC_LIBS}"
    )
  endif()

  if(HI_CC_LIBRARY_SHARED_EXCLUDE_STATIC_LIBS)
    target_link_libraries(${target_name}
      PRIVATE
        ${HI_CC_LIBRARY_SHARED_EXCLUDE_STATIC_LIBS}
    )

    foreach(exclude_static_lib IN LISTS HI_CC_LIBRARY_SHARED_EXCLUDE_STATIC_LIBS)
      hi_runtime_contains_abstract_target(${exclude_static_lib} is_abstract)
      if(is_abstract)
        hi_get_target_aliased_name(${exclude_static_lib} exclude_static_target)
      else()
        string(REPLACE :: _ exclude_static_target ${exclude_static_lib})
      endif()

      target_link_options(${target_name}
        PRIVATE -Wl,--exclude-libs,lib${exclude_static_target}.a
      )
    endforeach()

    set_target_properties(${target_name}
      PROPERTIES HI_EXCLUDE_STATIC_LIBS "${HI_CC_LIBRARY_SHARED_EXCLUDE_STATIC_LIBS}"
    )
  endif()

  if(NOT HI_CC_LIBRARY_SHARED_SRCS AND
    (HI_CC_LIBRARY_SHARED_WHOLE_STATIC_LIBS OR
     HI_CC_LIBRARY_SHARED_GROUP_STATIC_LIBS OR
     HI_CC_LIBRARY_SHARED_EXCLUDE_STATIC_LIBS OR
     HI_CC_LIBRARY_SHARED_DEPS OR
     HI_CC_LIBRARY_SHARED_PUB_DEPS))
    hi_enable_empty_srcs(NAME ${target_name})
  endif()

  hi_add_to_gen_target(
    NAME
      ${target_name}
  )

  if(NOT HI_CC_LIBRARY_SHARED_NAME STREQUAL target_name)
    add_library(${HI_CC_LIBRARY_SHARED_NAME} ALIAS ${target_name})
  endif()
endfunction()