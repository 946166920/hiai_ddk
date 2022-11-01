set(proto_shell_dir ${CMAKE_CURRENT_LIST_DIR})

function(hi_proto_gen)
  set(opts
    LITE
    FULL
  )

  set(single_args
    NAME
    PROTO_PATH
    PROTO_OUT
  )

  set(multi_args
    PROTO_SRCS
  )

  cmake_parse_arguments(HI_PROTO_GEN "${opts}" "${single_args}" "${multi_args}" ${ARGN})

  if(HI_PROTO_GEN_UNPARSED_ARGUMENTS)
    hi_message(FATAL_ERROR "unknown keywords given to hi_cc_library_proto(): \"${HI_PROTO_GEN_UNPARSED_ARGUMENTS}\"")
    return()
  endif()

  if(NOT EXISTS ${HI_PROTO_GEN_PROTO_OUT})
    file(MAKE_DIRECTORY ${HI_PROTO_GEN_PROTO_OUT})
  endif()

  if(NOT HI_PROTO_GEN_NAME)
    hi_message(FATAL_ERROR "required keyword NAME missing for hi_proto_gen() function")
    return()
  endif()

  foreach(proto_file IN LISTS HI_PROTO_GEN_PROTO_SRCS)
    get_filename_component(proto_dir_name ${proto_file} DIRECTORY)
    get_filename_component(proto_file_name ${proto_file} NAME_WE)

    if(NOT proto_dir_name STREQUAL "")
      string(CONCAT proto_file_name ${proto_dir_name} "/" ${proto_file_name})
    endif()

    LIST(APPEND all_cc_rlt_srcs "${proto_file_name}.pb.cc")
    LIST(APPEND all_cc_abs_srcs "${HI_PROTO_GEN_PROTO_OUT}/${proto_file_name}.pb.cc")
    LIST(APPEND all_cc_rlt_hdrs "${proto_file_name}.pb.h")
    LIST(APPEND all_cc_abs_hdrs "${HI_PROTO_GEN_PROTO_OUT}/${proto_file_name}.pb.h")
    LIST(APPEND all_proto_src "${HI_PROTO_GEN_PROTO_PATH}/${proto_file}")
  endforeach()

  set(lite_flag)
  if(HI_PROTO_GEN_LITE)
    set(lite_flag "lite:")
  endif()

  hi_message(WARNING "HI_PROTO_GEN_NAME:${HI_PROTO_GEN_NAME}")
  hi_message(WARNING "proto_shell_dir:${proto_shell_dir}")
  hi_message(WARNING "HI_PROTO_GEN_PROTO_PATH:${HI_PROTO_GEN_PROTO_PATH}")
  hi_message(WARNING "lite_flag:${lite_flag}")
  hi_message(WARNING "HI_PROTO_GEN_PROTO_OUT:${HI_PROTO_GEN_PROTO_OUT}")
  hi_message(WARNING "HI_PROTO_GEN_PROTO_SRCS:${HI_PROTO_GEN_PROTO_SRCS}")
  hi_message(WARNING "OUTS all_cc_rlt_srcs:${all_cc_rlt_srcs}")
  hi_message(WARNING "OUTS all_cc_rlt_hdrs:${all_cc_rlt_hdrs}")
  hi_message(WARNING "all_proto_src:${all_proto_src}")

  hi_cc_gen_srcs(
  NAME
    ${HI_PROTO_GEN_NAME}
  SH
    gen_proto.sh
  CWD
    ${proto_shell_dir}
  ARGS
    --proto_path ${HI_PROTO_GEN_PROTO_PATH} --cpp_out ${lite_flag}${HI_PROTO_GEN_PROTO_OUT} ${HI_PROTO_GEN_PROTO_SRCS}
  OUT_DIR
    ${HI_PROTO_GEN_PROTO_OUT}
  OUTS
    ${all_cc_rlt_srcs}
    ${all_cc_rlt_hdrs}
  SRCS
    ${all_proto_src}
  )
endfunction()