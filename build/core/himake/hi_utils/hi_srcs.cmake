include_guard()

include(hi_utils/hi_message)

function(hi_extract_srcs_targets target_srcs out_filegroups out_genrules)
  if("${target_srcs}" STREQUAL "")
    return()
  endif()

  foreach(src IN LISTS target_srcs)
    if(TARGET ${src})
      string(REPLACE :: _ src ${src})
      hi_get_target_property(${src} HI_TYPE target_type)

      if(target_type STREQUAL HI_FILEGROUP)
        list(APPEND src_filegroups ${src})
      else()
        list(APPEND src_genrules ${src})
      endif()
    endif()
  endforeach()

  set(${out_filegroups} ${src_filegroups} PARENT_SCOPE)
  set(${out_genrules} ${src_genrules} PARENT_SCOPE)
endfunction()

function(hi_group_srcs_on_target target_name target_srcs)
  if("${target_srcs}" STREQUAL "")
    return()
  endif()

  hi_extract_srcs_targets(${target_srcs} filegroups genrules)

  hi_set_target_property(${target_name} HI_SRCS "${target_srcs}")

  if(NOT "${src_filegroups}" STREQUAL "")
    hi_set_target_property(${target_name} HI_FILEGROUPS "${filegroups}")
  endif()

  if(NOT "${src_genrules}" STREQUAL "")
    hi_append_target_property(${target_name} HI_GENRULES "${genrules}")
  endif()
endfunction()

function(hi_extract_srcs_from_path path all_srcs_for_path_out)
  string(FIND ${src} "*" result)
  if (result EQUAL -1)  # non glob
    list(APPEND ex_srcs ${src})
  else()
    hi_glob(files
      INCLUDES
      ${src}
    )
    list(APPEND ex_srcs ${files})
  endif()
  set(${all_srcs_for_path_out} "${ex_srcs}" PARENT_SCOPE)
endfunction()

function(hi_extract_srcs_from_target target_name all_srcs_for_target_out all_incs_for_target_out)
  string(REPLACE :: _ src ${src})
  hi_get_target_property(${src} HI_TYPE target_type)

  if(target_type STREQUAL HI_FILEGROUP)
    hi_filegroup_get_all_srcs(${src} sub_abs_srcs sub_abs_incs)
    list(APPEND t_srcs ${sub_abs_srcs})
    list(APPEND t_incs ${sub_abs_incs})
  else()
    hi_get_target_property(${src} HI_GEN_INCS gen_incs)
    hi_get_target_property(${src} HI_GEN_SRCS gen_srcs)
    list(APPEND t_srcs ${gen_srcs})
    list(APPEND t_incs ${gen_incs})
  endif()
  set(${all_srcs_for_target_out} "${t_srcs}" PARENT_SCOPE)
  set(${all_incs_for_target_out} "${t_incs}" PARENT_SCOPE)
endfunction()

# path in filegroup will be extract as absolute path
# glob will extract to exact path
function(hi_extract_all_srcs target_srcs all_srcs_out all_incs_out)
  foreach(src IN LISTS target_srcs)
    if(NOT TARGET ${src})
      hi_extract_srcs_from_path(${src} extract_path_srcs)
      list(APPEND srcs ${extract_path_srcs})
    else()
      hi_extract_srcs_from_target(${src} target_srcs target_incs)
      list(APPEND srcs ${target_srcs})
      list(APPEND incs ${target_incs})
    endif()
  endforeach()

  set(${all_srcs_out} "${srcs}" PARENT_SCOPE)
  set(${all_incs_out} "${incs}" PARENT_SCOPE)
endfunction()

function(hi_set_compile_target_srcs target_name target_srcs)
  hi_group_srcs_on_target(${target_name} "${target_srcs}")

  hi_extract_all_srcs("${target_srcs}" src_list inc_list)
  target_sources(${target_name} PRIVATE ${src_list})
  target_include_directories(${target_name} PUBLIC ${inc_list})
endfunction()

# TODO: remove
# 1、extract all file group sources
# 1、record srcs and filegroup to seperate varible
function(extract_all_srcs_and_record_seperatly target_name target_srcs target_excludes all_srcs all_incs)
  set(all_srcs)

  foreach(src IN LISTS target_srcs)
    if(NOT TARGET ${src})
      list(APPEND direct_src_files ${src})
    else()
      string(REPLACE :: _ src ${src})

      hi_get_target_property(${src} HI_TYPE target_type)
      if(${target_type} STREQUAL HI_GEN_SRCS)
        list(APPEND src_genrules ${src})

        hi_get_target_property(${src} HI_GEN_SRCS gen_srcs)
        list(APPEND all_srcs ${gen_srcs})

        hi_get_target_property(${src} HI_GEN_INCS gen_incs)
        list(APPEND all_incs ${gen_incs})
        # target_include_directories(${target_name} PRIVATE ${gen_hdrs})

        set_property(TARGET ${target_name}
          APPEND PROPERTY HI_CUSTOM_TARGET_DEPS ${src}
        )
      else() # filegroup
        list(APPEND src_filegroups ${src})

        hi_get_target_property(${src} HI_SRCS fg_srcs)
        list(APPEND all_srcs ${fg_srcs})

        # add func srcs
        hi_get_target_property(${src} HI_FUNCTIONS funcs)
        foreach(func IN LISTS funcs)
          if(${func})
            hi_get_target_property(${src} ${func}_SRCS func_srcs)
            if(NOT func_srcs)
                list(APPEND all_srcs ${func_srcs})
            endif()
          endif()
        endforeach()
      endif()
    endif()
  endforeach()

  if(NOT "${direct_src_files}" STREQUAL "")
    hi_glob(src_files
      INCLUDES
        ${direct_src_files}
      EXCLUDES
        ${target_excludes}
    )
    list(APPEND all_srcs ${src_files})
  endif()

  hi_set_target_property(${target_name} HI_SRCS "${src_files}")

  hi_set_target_property(${target_name} HI_FILEGROUPS "${src_filegroups}")

  if(NOT "${src_genrules}" STREQUAL "")
    hi_append_target_property(${target_name} HI_GENRULES "${src_genrules}")
  endif()

  # for run in cmake
  set(all_srcs "${all_srcs}" PARENT_SCOPE)
  set(all_incs "${all_incs}" PARENT_SCOPE)
endfunction()