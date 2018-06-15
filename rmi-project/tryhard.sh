#!/bin/bash

SOURCE="$0"
while [[ -h "$SOURCE" ]]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"


while [ true ]; do
  ( flock -n 9 || exit 1
  $DIR/java BancoDiscClient 177.220.85.115 < $DIR/test-inputs/testcase.in
  ) 9> /dev/null
done
