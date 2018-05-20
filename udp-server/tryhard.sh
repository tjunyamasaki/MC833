#!/bin/bash

SOURCE="$0"
while [[ -h "$SOURCE" ]]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

for i in {1..1000}
do
    sed "s#{NUM}#$i#;" $DIR/test-inputs/testcase.in > $DIR/test-inputs/testcase-tmp.in
    $DIR/client localhost < $DIR/test-inputs/testcase-tmp.in
    rm $DIR/test-inputs/testcase-tmp.in
done
