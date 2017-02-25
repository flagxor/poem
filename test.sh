#!/bin/bash

set -e

if [[ "$1" = *arm* ]]; then
  if [[ "${TRAVIS}" = true ]]; then
    echo "skipping $1"
    exit 0
  else
    CMD="qemu-arm $1"
  fi
else
  CMD="$1"
fi

echo ${CMD}
if [[ "$(echo bye | ${CMD})" == "ABCDEFGHIJKLMNOPQRST" ]]; then
  echo PASS
  exit 0
else
  echo FAIL
  exit 1
fi
