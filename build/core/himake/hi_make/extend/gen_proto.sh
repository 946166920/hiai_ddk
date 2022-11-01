#!/bin/bash
cur_dir=$(cd $(dirname $0); pwd)

protoc=/usr1/c00574543/Code/hiai_ddk_q/hiai_ddk/third_party/protoc-3.13.0/protoc

args=$(echo $1 | tr "," " ")

touch testcxxxx.txt

echo cxxxxxxxxxxxxx "${protoc} ${args}" > testcxxxx.txt
${protoc} ${args}

