#!/bin/bash

# Colors to use in echo
RED='\033[0;31m'
GREEN='\033[0;32m'
BOLD='\033[1m'
NC='\033[0m'

SOURCE="$0"
while [[ -h "$SOURCE" ]]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
DIRTMP=$DIR/options_tmp

trap "rm -rf $DIR/options_tmp" EXIT

mkdir $DIRTMP > /dev/null 2>&1

sed "s#DATA_DIR#$DIR#g;" $DIR/create_db.c > $DIRTMP/create_db_tmp.c

cp $DIR/Makefile $DIRTMP/ && cd $DIRTMP/ && make create_db_tmp && ./create_db_tmp

FILE_RETURN=$?

rm -rf $DIR/options_tmp

if [[ $SCP_RETURN -eq 0 ]]; then
   echo -e "\n${GREEN}#####${NC}  DB successfully created!  ${GREEN}#####${NC}\n"
else
   echo -e "\n${RED}->${NC} Problem creating DB. Try again\n"
   rm -f $DIRCONFIG/config_tmp
   exit 1
fi
