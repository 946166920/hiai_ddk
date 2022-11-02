#!/bin/bash
cur_dir=$(cd $(dirname $0); pwd)

protoc=${cur_dir}/../../../../../third_party/protoc-3.13.0/protoc

args=$(echo $1 | tr "," " ")

${protoc} ${args}

