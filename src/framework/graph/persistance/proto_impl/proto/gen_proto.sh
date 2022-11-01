#!/bin/bash
cur_dir=$(cd $(dirname $0); pwd)
echo "cxxxxxxxxxxxxxxxxxxx gen_proto.sh enter"

# protoc=${cur_dir}/../foo/open_source/protobuf/bin/protoc
protoc=/usr1/c00574543/Code/hiai_ddk_q/hiai_ddk/third_party/protoc-3.13.0/protoc

args=$(echo $1 | tr "," " ")

${protoc} ${args}

