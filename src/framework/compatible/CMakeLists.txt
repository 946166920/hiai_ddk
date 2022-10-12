hi_cc_objects(
  NAME
    ai::fmk::compatible_objs
  SRCS
    array_op_transformer.cpp
    control_flow_op_transformer.cpp
    detection_op_transformer.cpp
    image_op_transformer.cpp
    internal_op_transformer.cpp
    ir_transformer.cpp
    ir_verify.cpp
    math_op_transformer.cpp
    nn_op_transformer.cpp
    transformer_utils.cpp
)

hi_cc_objects(
  NAME
    ai::fmk::compatible::model_compatibility_check_objs
  SRCS
    model_compatibility_check.cpp
)

hi_generator(
  NAMES
    ai::fmk::compatible_objs
    ai::fmk::compatible::model_compatibility_check_objs
)
