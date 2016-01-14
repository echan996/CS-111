#!/bin/bash

function should_succeed() {
  result=$?;

  echo -n "==> $1 ";

  if [ $result -gt 0 ]; then
    echo "FAILURE";
    exit 1;
  else
    echo;
  fi
}

tmp_file=/tmp/foo
tmp_file2=/tmp/foo2
> "$tmp_file"
> "$tmp_file2"

./simpsh --verbose --command 1 2 3 echo foo 2>&1 | grep "Bad file descriptor" > /dev/null
should_succeed "using a non existent file descriptor should report the error"
