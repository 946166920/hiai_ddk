hi_add_subdirectory(persistance)
hi_add_subdirectory(utils)

hi_cc_library_static(
  NAME
    ai::fmk::graph::core_static
  SRCS
    core/cgraph/*.cpp
    core/edge/*.cpp
    core/node/*.cpp
    core/op/*.cpp
    ./*.cpp
  CDEFS
    GRAPH_API_VISIABLE
)

# libhiai_ir.so
hi_cc_library_shared(
  NAME
    ai::fmk::hiai_ir_shared
  WHOLE_STATIC_LIBS
    ai::infra::log
    huawei::c_sec
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
    ai::fmk::graph::core_static
    ai::fmk::graph::utils_static
    ai::fmk::compatible_static
    ai::fmk::common::helper_static
    ai::infra::math::fp16_t_static
    ai::fmk::util::tensor::trans_tensor_static
    google::protobuf_lite_static
)

hi_cc_function(STATIC_IR_GRAPH_PROTO
  NAME
    ai::fmk::hiai_ir_shared
  WHOLE_STATIC_LIBS
    ai::fmk::graph::persistance::proto_impl_static
    ai::fmk::graph::persistance::static_proxy_static
)

hi_target_output_name(ai::fmk::hiai_ir_shared hiai_ir)