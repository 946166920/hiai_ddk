include_guard()

include(CMakeParseArguments)
include(hi_utils/hi_message)
include(hi_utils/hi_runtime)

function(hi_cc_prebuilt_library_import_location target_name lib)
  string(REPLACE :: _ target_lib ${lib})
  if(TARGET ${target_lib})
    get_target_property(import_loc ${target_lib} HI_GEN_LIB)

    add_dependencies(${target_name} ${target_lib})

    set_property(TARGET ${target_name}
      APPEND PROPERTY HI_CUSTOM_TARGET_DEPS ${target_lib}
    )
  else()
    set(import_loc ${target_lib})
  endif()

  set_target_properties(${target_name}
    PROPERTIES HI_SRCS ${import_loc}
  )

  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION
      ${import_loc}
  )

  ## output_name
  hi_runtime_get_target_output(${target_name} output)
  if(output)
    set(output_name ${output})
  else()
    get_filename_component(output_name ${import_loc} NAME_WE)

    string(FIND ${output_name} "lib" index)
    if(index STREQUAL "0")
      string(SUBSTRING ${output_name} 3 -1 output_name)
    endif()
  endif()

  if(output_name)
    set_target_properties(${target_name}
      PROPERTIES OUTPUT_NAME ${output_name}
    )
  endif()
endfunction()

function(hi_cc_prebuilt_library)
  set(opts
    SHARED
    STATIC
  )
  set(single_args
    NAME
    LIB
  )

  set(multi_args
    INCS
  )

  cmake_parse_arguments(HI_CC_PREBUIT_LIBRARY "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_PREBUIT_LIBRARY_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_prebuilt_library(): \"${HI_CC_PREBUIT_LIBRARY_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_PREBUIT_LIBRARY_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_prebuilt_library() function")
    return()
  endif()

  if(NOT HI_CC_PREBUIT_LIBRARY_SHARED AND NOT HI_CC_PREBUIT_LIBRARY_STATIC )
    hi_message(FATAL_ERROR "required option  SHARED OR STATIC missing for hi_cc_prebuilt_library() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_CC_PREBUIT_LIBRARY_NAME})

  if(HI_CC_PREBUIT_LIBRARY_SHARED)
    add_library(${target_name} SHARED IMPORTED GLOBAL)
    set_target_properties(${target_name}
       PROPERTIES HI_TYPE PREBUIT_LIBRARY_SHARED
    )
  else()
    add_library(${target_name} STATIC IMPORTED GLOBAL)
    set_target_properties(${target_name}
      PROPERTIES HI_TYPE PREBUIT_LIBRARY_STATIC
    )
  endif()

  if(HI_CC_PREBUIT_LIBRARY_INCS)
    target_include_directories(${target_name} INTERFACE ${HI_CC_PREBUIT_LIBRARY_INCS})
  endif()

  ## imported_location
  if(HI_CC_PREBUIT_LIBRARY_LIB)
    hi_cc_prebuilt_library_import_location(${target_name} ${HI_CC_PREBUIT_LIBRARY_LIB})
  endif()

  hi_add_to_gen_target(
    NAME
      ${target_name}
  )

  if(NOT HI_CC_PREBUIT_LIBRARY_NAME STREQUAL target_name)
    add_library(${HI_CC_PREBUIT_LIBRARY_NAME} ALIAS ${target_name})
  endif()

endfunction()