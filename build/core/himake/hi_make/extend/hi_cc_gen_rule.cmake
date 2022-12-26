include_guard()

include(hi_utils/hi_glob)
include(CMakeParseArguments)
include(hi_utils/hi_message)
include(hi_make/extend/hi_cc_prebuilt_library)

function(hi_cc_gen_rule)
  set(opt)

  set(single_args
    NAME
    OUT_DIR # output path, should be absolute full path
    PYTHON
    SH
    CWD
  )

  set(multi_args
    SRCS
    OUTS
    ARGS
  )

  cmake_parse_arguments(HI_CC_GENRULE "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_CC_GENRULE_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_gen_rule(): \"${HI_CC_GENRULE_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_GENRULE_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_gen_rule() function")
    return()
  endif()

  if(NOT HI_CC_GENRULE_CWD)
    hi_message(FATAL_ERROR "required keyword CWD missing for hi_cc_gen_rule() function")
    return()
  endif()

  if(NOT HI_CC_GENRULE_OUT_DIR)
    hi_message(FATAL_ERROR "required keyword OUT_DIR missing for hi_cc_gen_rule() function")
    return()
  endif()

  if(NOT HI_CC_GENRULE_OUTS)
    hi_message(FATAL_ERROR "required keyword OUTS missing for hi_cc_gen_rule() function")
    return()
  endif()

  if(HI_CC_GENRULE_PY AND HI_CC_GENRULE_SH)
    hi_message(FATAL_ERROR "both python and shell scripts found for hi_cc_gen_rule()")
    return()
  endif()

  if(NOT HI_CC_GENRULE_PYTHON AND NOT HI_CC_GENRULE_SH)
    hi_message(FATAL_ERROR "both python and shell scripts not found for hi_cc_gen_rule()")
    return()
  endif()

  ## add gen command
  set(gen_out_dir ${HI_CC_GENRULE_OUT_DIR})
  if(NOT EXISTS ${gen_out_dir})
    file(MAKE_DIRECTORY ${gen_out_dir})
  endif()

  foreach(out_file IN LISTS HI_CC_GENRULE_OUTS)
      LIST(APPEND all_outs "${gen_out_dir}/${out_file}")
  endforeach()

  if(NOT "${HI_CC_GENRULE_ARGS}" STREQUAL "")
    string(REPLACE ";" "," args "${HI_CC_GENRULE_ARGS}")
  endif()

  set(srcs "")
  if(NOT "${HI_CC_GENRULE_SRCS}" STREQUAL "")
    hi_extract_all_srcs("${HI_CC_GENRULE_SRCS}" src_list inc_list)

    if("${inc_list}" STREQUAL "")
      string(REPLACE ";" "," srcs "${src_list}")
    else()
      string(REPLACE ";" "," srcs "${src_list};${inc_list}")
    endif()
  endif()

  if(HI_CC_GENRULE_PYTHON)
    string(REPLACE "," " " args "${HI_CC_GENRULE_ARGS}")
    set(cmd ${Python_EXECUTABLE} -B ${HI_CC_GENRULE_CWD}/${HI_CC_GENRULE_PYTHON} ${args} ${HI_CC_GENRULE_OUT_DIR} ${srcs})
  endif()

  if(HI_CC_GENRULE_SH)
    set(cmd ${HI_CC_GENRULE_CWD}/${HI_CC_GENRULE_SH} ${args} ${HI_CC_GENRULE_OUT_DIR} ${srcs})
  endif()

  string(REPLACE :: _ target_name ${HI_CC_GENRULE_NAME})

  add_custom_command(
    OUTPUT
      ${all_outs}
    COMMAND
      ${cmd}
    DEPENDS
      ${HI_CC_GENRULE_CWD}/${HI_CC_GENRULE_PYTHON}
      ${HI_CC_GENRULE_CWD}/${HI_CC_GENRULE_SH}
      ${HI_CC_GENRULE_SRCS}
    COMMENT
      "hi_genrule generates files"
  )

  add_custom_target(${target_name} DEPENDS ${all_outs})
  set_target_properties(${target_name}
    PROPERTIES HI_TYPE HI_GEN_RULE
  )

  # record info for gen
  hi_group_srcs_on_target(${target_name} "${HI_CC_GENRULE_SRCS}")

  set_target_properties(${target_name}
      PROPERTIES HI_GEN_OUTS "${all_outs}"
  )
  set_target_properties(${target_name}
      PROPERTIES HI_GEN_ARGS "${HI_CC_GENRULE_ARGS}"
  )
  set_target_properties(${target_name}
      PROPERTIES HI_GEN_PY "${HI_CC_GENRULE_PYTHON}"
  )
  set_target_properties(${target_name}
      PROPERTIES HI_GEN_SH "${HI_CC_GENRULE_SH}"
  )
endfunction()

function(hi_cc_gen_srcs)
  set(opts
  )

  set(single_args
    NAME
    OUT_DIR # output path, should be absolute full path
    PYTHON
    SH
    CWD
  )

  set(multi_args
    SRCS
    ARGS
    OUTS
  )

  cmake_parse_arguments(HI_CC_GENSRCS "${opts}" "${single_args}" "${multi_args}" ${ARGN})
  if(HI_CC_GENSRCS_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_gen_srcs(): \"${HI_CC_GENSRCS_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_GENSRCS_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_gen_srcs() function")
    return()
  endif()

  # record gen rule info for himake gen
  string(REPLACE :: _ target_name ${HI_CC_GENSRCS_NAME})

  hi_cc_gen_rule(
    NAME
      ${target_name}
    CWD
      ${HI_CC_GENSRCS_CWD}
    PYTHON
      ${HI_CC_GENSRCS_PYTHON}
    SH
      ${HI_CC_GENSRCS_SH}
    ARGS
      ${HI_CC_GENSRCS_ARGS}
    SRCS
      ${HI_CC_GENSRCS_SRCS}
    OUT_DIR
      ${HI_CC_GENSRCS_OUT_DIR}
    OUTS
      ${HI_CC_GENSRCS_OUTS}
  )

  get_target_property(all_outs ${target_name} HI_GEN_OUTS)

  foreach(file IN LISTS all_outs)
    string(FIND ${file} .h index)
    if(NOT index EQUAL -1)
      set(gen_hdrs ${HI_CC_GENSRCS_OUT_DIR})
    else()
      list(APPEND gen_srcs ${file})
    endif()
  endforeach()

  set_target_properties(${target_name}
      PROPERTIES HI_GEN_INCS "${gen_hdrs}"
  )
  set_target_properties(${target_name}
      PROPERTIES HI_GEN_SRCS "${gen_srcs}"
  )

  hi_add_to_gen_target(
    NAME
      ${target_name}
  )
endfunction()

function(hi_cc_gen_lib)
  set(opts)

  set(single_args
    NAME
    OUT_DIR # output path, should be absolute full path
    PYTHON
    SH
    CWD
    OUT
  )

  set(multi_args
    SRCS
    ARGS
  )

  cmake_parse_arguments(HI_CC_GENLIB "${opts}" "${single_args}" "${multi_args}" ${ARGN})
  if(HI_CC_GENLIB_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_gen_lib(): \"${HI_CC_GENLIB_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT HI_CC_GENLIB_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_cc_gen_lib() function")
    return()
  endif()

  string(REPLACE :: _ target_name ${HI_CC_GENLIB_NAME})

  hi_cc_gen_rule(
    NAME
      ${target_name}
    CWD
      ${HI_CC_GENLIB_CWD}
    PYTHON
      ${HI_CC_GENLIB_PYTHON}
    SH
      ${HI_CC_GENLIB_SH}
    ARGS
      ${HI_CC_GENLIB_ARGS}
    SRCS
      ${HI_CC_GENLIB_SRCS}
    OUT_DIR
      ${HI_CC_GENLIB_OUT_DIR}
    OUTS
      ${HI_CC_GENLIB_OUT}
  )

  get_target_property(out ${target_name} HI_GEN_OUTS)

  set_target_properties(${target_name}
    PROPERTIES HI_GEN_LIB "${out}"
  )

  add_custom_target(${target_name}_gen DEPENDS ${out})
  add_dependencies(${target_name} ${target_name}_gen)

  hi_add_to_gen_target(
    NAME
      ${target_name}
  )
endfunction()