include_guard()

include(CMakeParseArguments)
include(hi_utils/hi_target)
include(hi_utils/hi_message)

function(hi_cc_system systems)
  list(FIND systems ${HI_SYSTEM_NAME} index)
  if(index EQUAL -1)
    return()
  endif()

  set(single_args
    NAME
    SCRIPT
  )

  set(multi_args
    INCS
    SRCS
    DEPS
    PUB_DEPS
    CDEFS
    COPTS
    LOPTS
    MULTILIB
    WHOLE_STATIC_LIBS
    PUB_WHOLE_STATIC_LIBS
    GROUP_STATIC_LIBS
  )

  cmake_parse_arguments(HI_SYS "${opts}" "${single_args}" "${multi_args}" ${ARGN})
  
  if(HI_SYS_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_system(): \"${HI_SYS_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_SYS_NAME)
    hi_message(FATAL_ERROR "required keyword TARGET missing for hi_cc_system() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_SYS_NAME})

  if(NOT TARGET ${target_name})
    hi_message(FATAL_ERROR "target ${HI_SYS_NAME} is not exist before hi_cc_system() function")  
    return()
  endif()

  hi_get_target_property(${target_name} HI_TYPE hi_type)

  if(HI_SYS_INCS)
    # relative path
    set_property(
      TARGET
        ${target_name}
      APPEND
      PROPERTY
        HI_INCS ${HI_SYS_INCS}
    )

    foreach(inc IN LISTS HI_SYS_INCS)
      list(APPEND target_incs ${ROOT_DIR}/${inc})
    endforeach()

    target_include_directories(${target_name}
      PRIVATE
        ${target_incs}
    )

  endif()

  if(HI_SYS_SRCS)
    hi_extract_all_srcs(${HI_SYS_SRCS} all_srcs all_incs)
    target_sources(${target_name}
      PRIVATE
        ${all_srcs}
    )

    hi_append_target_property(${target_name} HI_SRCS "${HI_SYS_SRCS}")

    hi_extract_srcs_targets(${HI_SYS_SRCS} filegroups genrules)
    hi_append_target_property(${target_name} HI_FILEGROUPS "${filegroups}")
    hi_append_target_property(${target_name} HI_GENRULES "${genrules}")
  endif()

  if(HI_SYS_CDEFS)
    target_compile_definitions(${target_name}
      PRIVATE
        ${HI_SYS_CDEFS}
    )
  endif()

  if(HI_SYS_COPTS)
    target_compile_options(${target_name}
      PRIVATE 
        ${HI_SYS_COPTS}
    )

    set_property(
      TARGET
        ${target_name}
      APPEND
      PROPERTY
        HI_COPTS ${HI_SYS_COPTS}
    )
  endif()

  if(HI_SYS_LOPTS)
    target_link_options(${target_name}
      PRIVATE 
        ${HI_SYS_LOPTS}
    )
  endif()

  if(HI_SYS_MULTILIB)
    set_target_properties(${target_name}
      PROPERTIES
        HI_MULTILIB ${HI_SYS_MULTILIB}
    )
  endif()

  if(HI_SYS_DEPS)
    target_link_libraries(${target_name}
      PRIVATE 
        ${HI_SYS_DEPS}
    )
  endif()

  if(HI_SYS_PUB_DEPS)
    target_link_libraries(${target_name}
      PUBLIC
        ${HI_SYS_PUB_DEPS}
    )
  endif()

  if(HI_SYS_WHOLE_STATIC_LIBS)
    target_link_libraries(${target_name} 
      PRIVATE -Wl,--whole-archive ${HI_SYS_WHOLE_STATIC_LIBS} -Wl,--no-whole-archive
    )
    set_property(
      TARGET
        ${target_name}
      APPEND
      PROPERTY
        HI_WHOLE_STATIC_LIBS ${HI_SYS_WHOLE_STATIC_LIBS}
    )
  endif()

  if(HI_SYS_PUB_WHOLE_STATIC_LIBS)
    target_link_libraries(${target_name} 
      PUBLIC -Wl,--whole-archive ${HI_SYS_PUB_WHOLE_STATIC_LIBS} -Wl,--no-whole-archive
    )
    set_property(
      TARGET
        ${target_name}
      APPEND
      PROPERTY
        HI_WHOLE_STATIC_LIBS ${HI_SYS_PUB_WHOLE_STATIC_LIBS}
    )
  endif()

  if(HI_SYS_GROUP_STATIC_LIBS)
    target_link_libraries(${target_name} 
      PRIVATE -Wl,--start-group ${HI_SYS_GROUP_STATIC_LIBS} -Wl,--end-group
    )
    set_property(
      TARGET
        ${target_name}
      APPEND
      PROPERTY
        HI_GROUP_STATIC_LIBS ${HI_SYS_GROUP_STATIC_LIBS}
    )
  endif()

  if(HI_SYS_SCRIPT)
    hi_set_target_property(${target_name} HI_SCRIPT ${HI_SYS_SCRIPT})
  endif()

endfunction()