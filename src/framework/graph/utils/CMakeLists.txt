set(cc_out_dirs ${ROOT_DIR}/api/framework/graph/op)

hi_cc_gen_rule(
  NAME
    gen_op_def
  CWD
    ${ROOT_DIR}/build/script
  PYTHON
    gen_op_def_h.py
  ARGS
    ${ROOT_DIR}
  OUT_DIR
    ${cc_out_dirs}
  OUTS
    # image_defs.h
    array_defs.h
    # nn_defs.h
    # random_defs.h
    # control_flow_defs.h
    # detection_defs.h
    # const_defs.h
    # math_defs.h
    # internal_defs.h
    # data_flow_defs.h
    # internal_nn_defs.h
)

hi_cc_library_static(
  NAME
    ai::fmk::graph::utils_static
  CDEFS
    GRAPH_API_VISIABLE
  SRCS
    *.cpp
    checker/*.cpp
    replacer/*.cpp
  DEPS
    gen_op_def
)