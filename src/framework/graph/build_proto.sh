#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2010-2022. All rights reserved.

set -e

CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"



PROTO_DIR="${CURRENT_DIR}"/../../../../../../vendor/hisi/npu/inc/common/proto
PARSER_PROTO_DIR="${CURRENT_DIR}"/../../../../../../vendor/hisi/npu/src/framework/parser
DEDICATED_COMPUTE_PROTO_DIR="${CURRENT_DIR}"/../../../../../../vendor/hisi/npu/src/framework/dedicated_compute/generator/proto

BUILD_DIR="${CURRENT_DIR}"/../../../../../../out/hihms/obj/hiai_ddk

PROTOC="${CURRENT_DIR}"/../../../../../../tools/open_source/protoc/protoc-3.13.0/protoc

if [ $# -gt 0 ]
then
    LITE_FLAG="lite:"
    BUILD_DIR="${CURRENT_DIR}"/../../../../../../out/hihms/obj/hiai_ddk/app
fi

COMMON_DIR="${CURRENT_DIR}"/../../../../../../vendor/hisi/npu/inc/common/proto
COMMON_GEN_DIR="${BUILD_DIR}"/intermediate/proto
mkdir -p "${BUILD_DIR}"/intermediate/proto


${PROTOC} --proto_path ${COMMON_DIR} --cpp_out "${LITE_FLAG}"${COMMON_GEN_DIR} insert_op.proto
${PROTOC} --proto_path ${COMMON_DIR} --cpp_out "${LITE_FLAG}"${COMMON_GEN_DIR} ge_ir.proto
${PROTOC} --proto_path ${COMMON_DIR} --cpp_out "${LITE_FLAG}"${COMMON_GEN_DIR} task.proto
${PROTOC} --proto_path ${COMMON_DIR} --cpp_out "${LITE_FLAG}"${COMMON_GEN_DIR} om.proto


GENERATOR_DIR="${CURRENT_DIR}"/../../../../../../vendor/hisi/npu/src/framework/dedicated_compute/generator/proto
GENERATOR_GEN_DIR=${BUILD_DIR}/intermediate/vendor/hisi/npu/src/framework/dedicated_compute/generator/proto
mkdir -p ${GENERATOR_GEN_DIR}

${PROTOC} --proto_path ${GENERATOR_DIR} --cpp_out "${LITE_FLAG}"${GENERATOR_GEN_DIR} input_parameters.proto
${PROTOC} --proto_path ${GENERATOR_DIR} --cpp_out "${LITE_FLAG}"${GENERATOR_GEN_DIR} ispnn_parameter.proto

ONNX_DIR="${CURRENT_DIR}"/../../../../../../vendor/hisi/npu/src/framework/parser/onnx/proto
ONNX_PROTO_GEN_DIR=${BUILD_DIR}/intermediate/vendor/hisi/npu/src/framework/parser/onnx/proto
mkdir -p ${ONNX_PROTO_GEN_DIR}

${PROTOC} --proto_path ${ONNX_DIR} --cpp_out "${LITE_FLAG}"${ONNX_PROTO_GEN_DIR} onnx.proto

CAFFE_DIR="${CURRENT_DIR}"/../../../../../../vendor/hisi/npu/src/framework/parser/caffe/proto
CAFFE_PROTO_GEN_DIR=${BUILD_DIR}/intermediate/vendor/hisi/npu/src/framework/parser/caffe/proto
mkdir -p ${CAFFE_PROTO_GEN_DIR}
${PROTOC} --proto_path ${CAFFE_DIR} --cpp_out "${LITE_FLAG}"${CAFFE_PROTO_GEN_DIR} caffe.proto

TENSORFLOW_DIR="${CURRENT_DIR}"/../../../../../../vendor/hisi/npu/src/framework/parser/tensorflow/proto
TENSORFLOW_PROTO_GEN_DIR=${BUILD_DIR}/intermediate/vendor/hisi/npu/src/framework/parser/tensorflow/proto
mkdir -p ${TENSORFLOW_PROTO_GEN_DIR}

${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} attr_value.proto
${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} function.proto
${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} graph.proto
${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} node_def.proto
${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} op_def.proto
${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} resource_handle.proto
${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} tensor.proto
${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} tensor_shape.proto
${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} types.proto
${PROTOC} --proto_path ${TENSORFLOW_DIR} --cpp_out "${LITE_FLAG}"${TENSORFLOW_PROTO_GEN_DIR} versions.proto
#for cc and cpp mixed complie
cp ${BUILD_DIR}/intermediate/proto/ge_ir.pb.cc ${BUILD_DIR}/intermediate/proto/ge_ir.pb.cpp
