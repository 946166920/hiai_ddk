hi_cc_gen_srcs(
  NAME
    hiai::inc::ops_hdrs_gen
  CWD
    ${ROOT_DIR}/build/script
  PYTHON
    gen_op_def_h.py
  ARGS
    ${ROOT_DIR} inc
  OUT_DIR
    ${ROOT_DIR}/inc/framework
  OUTS
    graph/op/internal_defs.h
    graph/op/data_flow_defs.h
    graph/op/internal_nn_defs.h
)

hi_cc_headers(
NAME
    hiai::inc::ops
INCS
    hiai::inc::ops_hdrs_gen
)