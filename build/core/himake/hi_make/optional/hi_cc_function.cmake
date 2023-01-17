include_guard()

include(CMakeParseArguments)
include(hi_make/extend/hi_cc_prebuilt_library)
include(hi_utils/hi_message)

function(hi_cc_function_prebuilt_library target_name func lib)
  if(NOT lib)
    hi_message(FATAL_ERROR "required keyword SRCS missing for hi_cc_function() function")
    return()
  endif()

  hi_set_target_property(${target_name} ${func}_SRCS ${lib})

  if(${func})
    hi_cc_prebuilt_library_import_location(${target_name} ${lib})
  endif()
endfunction()

function(hi_cc_function_file_group target_name func srcs)
  if("${srcs}" STREQUAL "")
    hi_message(FATAL_ERROR "required keyword SRCS missing for hi_cc_function() function")
    return()
  endif()

  hi_set_target_property(${target_name}
    ${func_name}_SRCS "${srcs}"
  )
endfunction()

function(hi_cc_function func_name)
  set(opts)
  
  set(single_args
    NAME
  )

  set(multi_args
    INCS
    SRCS
    DEPS
    COPTS
    CDEFS
    LOPTS
    WHOLE_STATIC_LIBS
  )

  cmake_parse_arguments(HI_FUNCTION "${opts}" "${single_args}" "${multi_args}" ${ARGN})
  
  if(HI_FUNCTION_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_function(): \"${HI_FUNCTION_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_FUNCTION_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_function() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_FUNCTION_NAME})

  if(NOT TARGET ${target_name})
    hi_message(FATAL_ERROR "target ${HI_FUNCTION_NAME} is not exist before hi_cc_function(${func_name})")  
    return()
  endif()

  # Append ${func_name} to property HI_FUNCTIONS of ${target_name}
  hi_append_target_property(${target_name} HI_FUNCTIONS ${func_name})

  hi_get_target_property(${target_name} HI_TYPE hi_type)

  ## prebuilt library
  if(hi_type STREQUAL PREBUIT_LIBRARY_SHARED OR hi_type STREQUAL PREBUILT_LIBRARY_STATIC)
    hi_cc_function_prebuilt_library(${target_name} ${func_name} ${HI_FUNCTION_SRCS})
    return()
  endif()

  ## filegroup
  if(hi_type STREQUAL HI_FILEGROUP)
    hi_cc_function_file_group(${target_name} ${func_name} ${HI_FUNCTION_SRCS})
    return()
  endif()

  ## others
  if(HI_FUNCTION_SRCS)
    if(${func_name})
      hi_extract_all_srcs("${HI_FUNCTION_SRCS}" srcs incs)
      target_sources(${target_name}
        PRIVATE
          ${srcs}
      )
    endif()    

    hi_set_target_property(${target_name}
      ${func_name}_SRCS "${HI_FUNCTION_SRCS}"
    )

    hi_extract_srcs_targets(${HI_FUNCTION_SRCS} filegroups genrules)
    hi_set_target_property(${target_name}
      ${func_name}_FILEGROUPS "${filegroups}"
    )
    hi_set_target_property(${target_name}
      ${func_name}_GENRULES "${genrules}"
    )
  endif()

  if(HI_FUNCTION_INCS)
   set_target_properties(${target_name}
      PROPERTIES ${func_name}_INCS "${HI_FUNCTION_INCS}"
    )

    foreach(inc IN LISTS HI_FUNCTION_INCS)
      list(APPEND target_incs ${ROOT_DIR}/${inc})
    endforeach()

    if(${func_name})
      target_include_directories(${target_name}
        PRIVATE
          ${target_incs} # absolute path
      )
    endif()
  endif()

  if(HI_FUNCTION_DEPS)
    get_target_property(target_type ${target_name} TYPE)      

    # enable the dependencies propagation when macro function is ON 
    if(${func_name})
      target_link_libraries(${target_name}
        PRIVATE
          ${HI_FUNCTION_DEPS}
      )
    endif()

    set_target_properties(${target_name}
       PROPERTIES ${func_name}_DEPS "${HI_FUNCTION_DEPS}"
    )
  endif()

  if(HI_FUNCTION_WHOLE_STATIC_LIBS)
    if(${func_name})
      target_link_libraries(${target_name}
        PRIVATE -Wl,--whole-archive ${HI_FUNCTION_WHOLE_STATIC_LIBS} -Wl,--no-whole-archive
      )
    endif()
    set_target_properties(${target_name}
      PROPERTIES ${func_name}_WHOLE_STATIC_LIBS "${HI_FUNCTION_WHOLE_STATIC_LIBS}"
    )
  endif()

  if(HI_FUNCTION_CDEFS)
    if(${func_name})
      target_compile_definitions(${target_name}
        PRIVATE
          ${HI_FUNCTION_CDEFS}
      )
    endif()
    
    set_target_properties(${target_name}
       PROPERTIES ${func_name}_CDEFS "${HI_FUNCTION_CDEFS}"
    )
  endif()

  if(HI_FUNCTION_COPTS)
    if(${func_name})
      target_compile_options(${target_name}
        PRIVATE
          ${HI_FUNCTION_COPTS}
      )
    endif()
    
    set_target_properties(${target_name}
       PROPERTIES ${func_name}_COPTS "${HI_FUNCTION_COPTS}"
    )
  endif()

  if(HI_FUNCTION_LOPTS)
    if(${func_name})
      target_link_options(${target_name}
        PRIVATE
          ${HI_FUNCTION_LOPTS}
      )
    endif()
    
    set_target_properties(${target_name}
       PROPERTIES ${func_name}_LOPTS "${HI_FUNCTION_LOPTS}"
    )
  endif()

endfunction()