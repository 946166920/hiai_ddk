include_guard()

include(hi_utils/hi_message)

# function(hi_filegroup_set_srcs target_name srcs_files excludes_files)
#   hi_glob(srcs
#     INCLUDES
#       ${HI_FILEGROUP_SRCS}
#     EXCLUDES
#       ${HI_FILEGROUP_EXCLUDES}
#   )

#   hi_set_target_properties(${target_name} HI_SRCS "${srcs}")

#   # set absolute srcs
#   foreach(src IN LISTS srcs)
#     get_filename_component(abs_src ${src} ABSOLUTE)
#     list(APPEND abs_srcs ${abs_src})
#   endforeach()
#   set_target_properties(${target_name}
#     PROPERTIES INTERFACE_HI_ABS_SRCS "${abs_srcs}"
#   )
# endfunction()

function(hi_filegroup_get_all_srcs target_name all_abs_srcs all_abs_incs)
  hi_get_target_property(${target_name} HI_SOURCE_DIR src_dir)
  hi_get_target_property(${target_name} HI_SRCS fg_srcs)

  foreach(src IN LISTS fg_srcs)
    if(NOT TARGET ${src})
      string(FIND ${src} "/" result)
      if(NOT result EQUAL 0) # not absolute path
        set(src ${src_dir}/${src})
      endif()

      hi_extract_srcs_from_path(${src} extract_path_srcs)
      list(APPEND all_srcs ${extract_path_srcs})
    else()
      hi_extract_srcs_from_target(${src} target_srcs target_incs)
      list(APPEND all_srcs ${target_srcs})
      list(APPEND all_incs ${target_incs})
    endif()
  endforeach()

  # add func srcs
  hi_get_target_property(${target_name} HI_FUNCTIONS funcs)
  foreach(func IN LISTS funcs)
    if(${func})
      hi_get_target_property(${src} ${func}_SRCS func_srcs)
      if(NOT func_srcs)
          list(APPEND all_srcs ${func_srcs})
      endif()
    endif()
  endforeach()

  # set to output
  set(${all_abs_srcs} "${all_srcs}" PARENT_SCOPE)
  set(${all_abs_incs} "${all_incs}" PARENT_SCOPE)
endfunction()

function(hi_filegroup)
  set(single_args
    NAME
  )

  set(multi_args
    SRCS
    EXCLUDES
  )

  cmake_parse_arguments(HI_FILEGROUP "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_FILEGROUP_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_filegroup(): \"${HI_FILEGROUP_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_FILEGROUP_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_filegroup() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_FILEGROUP_NAME})

  add_library(${target_name} INTERFACE)

  hi_set_target_property(${target_name} HI_TYPE HI_FILEGROUP)

  hi_set_target_property(${target_name} HI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

  if(NOT "${HI_FILEGROUP_SRCS}" STREQUAL "")
    hi_group_srcs_on_target(${target_name} "${HI_FILEGROUP_SRCS}")
  endif()

  hi_add_to_gen_target(
    NAME
      ${target_name}
  )

  if(NOT HI_FILEGROUP_NAME STREQUAL target_name)
    add_library(${HI_FILEGROUP_NAME} ALIAS ${target_name})
  endif()
endfunction()

