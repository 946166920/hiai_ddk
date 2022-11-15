#!/bin/bash
cur_dir=$(cd $(dirname $0); pwd)

protoc=${cur_dir}/../../../../../third_party/protoc/bin/protoc

args=$(echo $1 | tr "," " ")

${protoc} ${args}

