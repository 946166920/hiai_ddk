include_guard()

include(CMakeParseArguments)
include(CMakePrintHelpers)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

include(hi_message)

function(hi_add_to_gen_target)
  set(opts)

  set(single_args
    NAME
  )

  cmake_parse_arguments(HI_GENERATOR "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_GENERATOR_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_generator(): \"${HI_GENERATOR_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_GENERATOR_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_generator() function")
    return()
  endif()

  string(REPLACE / _ hi_generator_file ${CMAKE_CURRENT_SOURCE_DIR})
  get_target_property(files hi_runtime HI_GENERATOR_FILES)
  if(NOT files)
    set_target_properties(hi_runtime PROPERTIES
      HI_GENERATOR_FILES
        ${hi_generator_file}
    )
  else()
    list(FIND files ${hi_generator_file} result)
    if(result STREQUAL -1)
        set_property(TARGET hi_runtime APPEND PROPERTY
            HI_GENERATOR_FILES
            ${hi_generator_file}
        )
    endif()
  endif()
  
  get_target_property(names hi_runtime ${hi_generator_file}_NAMES)
  if(NOT names)
    set_target_properties(hi_runtime PROPERTIES
        ${hi_generator_file}_SRC_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    set_target_properties(hi_runtime PROPERTIES
        ${hi_generator_file}_BIN_DIR
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    set_target_properties(hi_runtime PROPERTIES
        ${hi_generator_file}_NAMES
        ${HI_GENERATOR_NAME}
    )
  else()
    set_property(TARGET hi_runtime APPEND PROPERTY
        ${hi_generator_file}_NAMES
        ${HI_GENERATOR_NAME}
    )
  endif()

endfunction()