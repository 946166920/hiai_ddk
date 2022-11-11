### define protobuf::lite for DDK
set(GOOGLE__PROTOBUF_DIR ${ROOT_DIR}/third_party/protobuf)
set(goolge_protobuf_lite_src_dir ${GOOGLE__PROTOBUF_DIR}/src/google/protobuf)

hi_cc_headers(
  NAME
    google::protobuf_headers
  INCS
    third_party/protobuf/src
)

hi_cc_library_static(
  NAME
    google::protobuf_lite_static
  SRCS
    ${goolge_protobuf_lite_src_dir}/arena.cc
    ${goolge_protobuf_lite_src_dir}/extension_set.cc
    ${goolge_protobuf_lite_src_dir}/generated_message_table_driven_lite.cc
    ${goolge_protobuf_lite_src_dir}/generated_message_util.cc
    ${goolge_protobuf_lite_src_dir}/implicit_weak_message.cc
    ${goolge_protobuf_lite_src_dir}/io/coded_stream.cc
    ${goolge_protobuf_lite_src_dir}/io/zero_copy_stream.cc
    ${goolge_protobuf_lite_src_dir}/io/zero_copy_stream_impl_lite.cc
    ${goolge_protobuf_lite_src_dir}/message_lite.cc
    ${goolge_protobuf_lite_src_dir}/parse_context.cc
    ${goolge_protobuf_lite_src_dir}/repeated_field.cc
    ${goolge_protobuf_lite_src_dir}/stubs/bytestream.cc
    ${goolge_protobuf_lite_src_dir}/stubs/common.cc
    ${goolge_protobuf_lite_src_dir}/stubs/int128.cc
    ${goolge_protobuf_lite_src_dir}/stubs/status.cc
    ${goolge_protobuf_lite_src_dir}/stubs/statusor.cc
    ${goolge_protobuf_lite_src_dir}/stubs/stringpiece.cc
    ${goolge_protobuf_lite_src_dir}/stubs/stringprintf.cc
    ${goolge_protobuf_lite_src_dir}/stubs/structurally_valid.cc
    ${goolge_protobuf_lite_src_dir}/stubs/strutil.cc
    ${goolge_protobuf_lite_src_dir}/stubs/time.cc
    ${goolge_protobuf_lite_src_dir}/wire_format_lite.cc
    ${goolge_protobuf_lite_src_dir}/any_lite.cc
    ${goolge_protobuf_lite_src_dir}/generated_enum_util.cc
    ${goolge_protobuf_lite_src_dir}/io/strtod.cc
    ${goolge_protobuf_lite_src_dir}/io/zero_copy_stream_impl.cc
    
    ${goolge_protobuf_lite_src_dir}/map_field.cc
  CDEFS
    HAVE_PTHREAD
    PROTOBUF_INLINE_NOT_IN_HEADERS=0
    GOOGLE_PROTOBUF_NO_RTTI
    GOOGLE_PROTOBUF_NO_THREADLOCAL
  DEPS
    google::protobuf_headers
    log
)