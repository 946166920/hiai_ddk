include_guard()

include(CMakeParseArguments)

include(hi_utils/hi_message)
include(hi_utils/hi_runtime)

function(hi_target_compile_definitions)
  set(opts)

  set(single_args
    NAME
  )

  set(multi_args
    PUBLIC
    PRIVATE
    INTERFACE
  )

  cmake_parse_arguments(HI_TARGET_CDEFS "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_TARGET_CDEFS_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_target_compile_definitions(): \"${HI_TARGET_CDEFS_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_TARGET_CDEFS_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_target_compile_definitions() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_TARGET_CDEFS_NAME})

  if(NOT TARGET ${target_name})
    return()
  endif()

  if(HI_TARGET_CDEFS_PUBLIC)
    target_compile_definitions(${target_name}
      PUBLIC ${HI_TARGET_CDEFS_PUBLIC}
    )  
  endif()

  if(HI_TARGET_CDEFS_PRIVATE)
    target_compile_definitions(${target_name}
      PRIVATE ${HI_TARGET_CDEFS_PRIVATE}
    )  
  endif()
  
  if(HI_TARGET_CDEFS_INTERFACE)
    target_compile_definitions(${target_name}
      INTERFACE ${HI_TARGET_CDEFS_INTERFACE}
    )  
  endif()
endfunction()

function(hi_target_include_directories)
  set(opts)

  set(single_args
    NAME
  )

  set(multi_args
    PUBLIC
    PRIVATE
    INTERFACE
  )

  cmake_parse_arguments(HI_TARGET_INCS "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_TARGET_INCS_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_target_include_directories(): \"${HI_TARGET_INCS_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_TARGET_INCS_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_target_include_directories() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_TARGET_INCS_NAME})

  if(NOT TARGET ${target_name})
    return()
  endif()

  if(HI_TARGET_INCS_PUBLIC)
    target_include_directories(${target_name}
      PUBLIC ${HI_TARGET_INCS_PUBLIC}
    )  
  endif()

  if(HI_TARGET_INCS_PRIVATE)
    target_include_directories(${target_name}
      PRIVATE ${HI_TARGET_INCS_PRIVATE}
    )  
  endif()
  
  if(HI_TARGET_INCS_INTERFACE)
    target_include_directories(${target_name}
      INTERFACE ${HI_TARGET_INCS_INTERFACE}
    )  
  endif()
endfunction()

function(hi_group_impl target dep)
  if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    target_link_libraries(${target} 
      PRIVATE 
        -Wl,--start-group ${dep} -Wl,--end-group
    )
  else()
    hi_message(FATAL_ERROR "no implements for hi_group() for ${CMAKE_CXX_COMPILER_ID}")
  endif()
endfunction()

function(hi_target_group)
  set(opts)
  
  set(single_args
    NAME
  )

  set(multi_args
    DEPS
  )

  cmake_parse_arguments(HI_GROUP "${opts}" "${single_args}" "${multi_args}" ${ARGN})
  
  if(HI_GROUP_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_group(): \"${HI_GROUP_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_GROUP_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_group() function")
    return()
  endif()

  string(REPLACE :: _ name ${HI_GROUP_NAME})  

  if(NOT TARGET ${target_name})
    return()
  endif()

  foreach(dep IN LISTS HI_GROUP_DEPS)
    hi_group_impl(${name} ${dep})
  endforeach()
endfunction()

function(hi_append_target_property target_name prop value)
  get_target_property(type ${target_name} TYPE)
  if(type STREQUAL INTERFACE_LIBRARY)
    set_property(TARGET ${target_name} APPEND
      PROPERTY INTERFACE_${prop} ${value}
    )
  else()
    set_property(TARGET ${target_name} APPEND
      PROPERTY ${prop} ${value}
    )
  endif()
endfunction()

function(hi_set_target_property target_name prop value)
  get_target_property(type ${target_name} TYPE)
  if(type STREQUAL INTERFACE_LIBRARY)
    set_target_properties(${target_name}
      PROPERTIES INTERFACE_${prop} "${value}"
    )
  else()
    set_target_properties(${target_name}
      PROPERTIES ${prop} "${value}"
    )
  endif()
endfunction()

function(hi_get_target_property target_name prop value)
  string(REPLACE :: _ target_name ${target_name})

  get_target_property(type ${target_name} TYPE)
  if(type STREQUAL INTERFACE_LIBRARY)
    get_target_property(prop_value ${target_name} INTERFACE_${prop})
  else()
    get_target_property(prop_value ${target_name} ${prop})
  endif()
  
  if(NOT prop_value)
    set(prop_value)
  endif()

  set(${value} ${prop_value} PARENT_SCOPE)
endfunction()

function(hi_get_target_aliased_name target_name aliased_name)
  hi_get_target_property(${target_name} ALIASED_TARGET real_name)
  if(NOT real_name)
    set(real_name ${target_name})
  endif()

  set(${aliased_name} ${real_name} PARENT_SCOPE)
endfunction()

set(HI_ENABLE_EMPTY_CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})  

function(hi_enable_empty_srcs)
  set(opts)

  set(single_args
    NAME
  )

  set(multi_args)

  cmake_parse_arguments(HI_ENABLE_EMPTY "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_ENABLE_EMPTY_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_enable_empty_srcs(): \"${HI_EXEC_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_ENABLE_EMPTY_NAME})

  hi_get_target_property(${target_name} SOURCES srcs)
  if(NOT srcs)
    configure_file(${HI_ENABLE_EMPTY_CURRENT_DIR}/hi_empty.cc.in hi_empty.cc)

    target_sources(${target_name}
      PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/hi_empty.cc
    )

    hi_message(STATUS "target ${target_name} has no sources in ${CMAKE_CURRENT_LIST_FILE}")
  endif()

  set_target_properties(${target_name}
    PROPERTIES HI_EMPTY_SRCS TRUE
  )
endfunction()

function(hi_target_path)
  set(opts)

  set(single_args
    NAME
  )

  set(multi_args
    LIST
  )

  cmake_parse_arguments(HI_TARGET_PATH "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_TARGET_PATH_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_target_path(): \"${HI_TARGET_PATH_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  foreach(target IN LISTS HI_TARGET_PATH_LIST)
    string(REPLACE " " "" target ${target})  # trim all spaces
    string(FIND ${target} ":" idx)            # split by ':'

    if(NOT idx EQUAL -1)                         # contains ':'
      string(SUBSTRING ${target} 0 ${idx} target_name)
      math(EXPR idx "${idx} + 1")
      string(SUBSTRING ${target} ${idx} -1 path)

      set_target_properties(hi_runtime PROPERTIES
        ${target_name}_CMAKE_PATH ${path}
      )
    else()
      hi_message(STATUS "${target} missing path.")
    endif()
  endforeach()
endfunction()

function(hi_target_add_unique target all_dep_targets)
  list(FIND all_dep_targets ${target} result)
  if(result EQUAL -1)
    list(APPEND all_dep_targets ${target})
  endif()
  set(all_dep_targets "${all_dep_targets}" PARENT_SCOPE)
endfunction()

function(hi_target_get_deps target all_dep_targets)
  # direct link librarary
  get_target_property(dep_targets ${target} LINK_LIBRARIES)
  if(dep_targets)
    foreach(dep_target IN LISTS dep_targets)
      string(REPLACE :: _ dep_target ${dep_target})
      hi_target_add_unique(${dep_target} "${all_dep_targets}")
      hi_target_get_deps(${dep_target} "${all_dep_targets}")
    endforeach()
  endif()

  # abstract depends
  hi_runtime_contains_abstract_target(${target} is_abstract)
  if(is_abstract)
    hi_get_target_aliased_name(${target} target_aliased)
    string(REPLACE :: _ target_aliased ${target_aliased})
    hi_target_add_unique(${target_aliased} "${all_dep_targets}")
  endif()

  # custom depends
  get_target_property(custom_dep_targets ${target} HI_CUSTOM_TARGET_DEPS)
  if(custom_dep_targets)
    foreach(custom_dep_target IN LISTS custom_dep_targets)
      string(REPLACE :: _ custom_dep_target ${custom_dep_target})
      hi_target_add_unique(${custom_dep_target} "${all_dep_targets}")
      hi_target_get_deps(${custom_dep_target} "${all_dep_targets}")
    endforeach()
  endif()
  set(all_dep_targets "${all_dep_targets}" PARENT_SCOPE)
endfunction()

function(hi_target_get_all_deps targets all_dep_targets)
  foreach(target IN LISTS targets)
    if(NOT TARGET ${target})
      hi_message(FATAL_ERROR "target not exist: ${target}")
      return()
    endif()
    hi_target_add_unique(${target} "${all_dep_targets}")
    hi_target_get_deps(${target} "${all_dep_targets}")
  endforeach()
  set(all_dep_targets ${all_dep_targets} PARENT_SCOPE)
endfunction()