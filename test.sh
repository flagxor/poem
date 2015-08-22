#!/bin/bash

set -e

if [[ "$1" = *arm* ]]; then
  CMD="/usr/bin/qemu-arm $1"
else
  CMD="$1"
fi

echo ${CMD}
if [[ "$(${CMD})" == "ABCDEFGHIJKLMNOPQRSTU" ]]; then
  echo PASS
  exit 0
else
  echo FAIL
  exit 1
fi
