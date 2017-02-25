#!/bin/bash

set -e

FORTH=$1
RESULT=PASS

Test() {
  if [ "$1" != "$2" ]; then
    RESULT=FAIL
  fi
}

TestLoop() {
  CMD=": test1 $ 14 for i 'A' + emit loop cr ; test1"
  Test $(echo "${CMD}" | ${FORTH}) "ABCDEFGHIJKLMNOPQRST"
}

TestLoop
echo $RESULT
if [ "${RESULT}" != PASS ]; then
  exit 1
fi
