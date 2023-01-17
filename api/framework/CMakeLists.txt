hi_cc_gen_srcs(
  NAME
    hiai::api::ops_hdrs_gen
  CWD
    ${ROOT_DIR}/build/script
  PYTHON
    gen_op_def_h.py
  ARGS
    ${ROOT_DIR} api
  OUT_DIR
    ${ROOT_DIR}/api/framework
  OUTS
    graph/op/image_defs.h
    graph/op/array_defs.h
    graph/op/nn_defs.h
    graph/op/random_defs.h
    graph/op/control_flow_defs.h
    graph/op/detection_defs.h
    graph/op/const_defs.h
    graph/op/math_defs.h
)

hi_cc_headers(
NAME
    hiai::api::ops
INCS
    hiai::api::ops_hdrs_gen
)