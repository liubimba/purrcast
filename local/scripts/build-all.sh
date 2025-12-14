#!/bin/bash

ERROR=1
ROOT_DIR="../.."

RED='\e[0;31m'
GREEN='\e[0;32m'
YELLOW='\e[0;33m'
NC='\e[0m'

LOG(){
  local message="$1"
  echo -e "[${GREEN}LOG${NC}]: $message"
}
ERROR(){
  local message="$1"
  echo -e "[${RED}ERROR${NC}] $message"
}

while [ "$#" -gt 0 ]; do
  case "$1" in
    --root_dir)
      if [ "$2" ]; then
        ROOT_DIR="$2"
        shift
      fi
  esac
  shift
done

if [[ -z "$ROOT_DIR" ]];  then
  ERROR "${YELLOW}--root_dir${NC} must be specified!"
  exit ${ERROR};
else
  if [ ! -d "${ROOT_DIR}" ]; then
    ERROR "--root_dir = ${ROOT_DIR} is not directory!"
    exit ${ERROR};
  fi
  ROOT_DIR=$(realpath ${ROOT_DIR})
  LOG "Set  ${YELLOW}--root_dir${NC} = ${YELLOW}${ROOT_DIR}${NC}"
fi

ARTIFACTS_DIR="${ROOT_DIR}/artifacts"
mkdir -p ${ARTIFACTS_DIR}

LAUNCHER_CPP="launcher-cpp"
mkdir -p "${ARTIFACTS_DIR}/${LAUNCHER_CPP}"
cmake -S "${ROOT_DIR}/${LAUNCHER_CPP}" -B "${ARTIFACTS_DIR}/${LAUNCHER_CPP}"
cmake --build "${ARTIFACTS_DIR}/${LAUNCHER_CPP}"

WEB_REACT="web-react"
mkdir -p "${ARTIFACTS_DIR}/${WEB_REACT}"
bash -c "cd '${ROOT_DIR}/${WEB_REACT}' && OUT_DIR='${ARTIFACTS_DIR}/${WEB_REACT}' npm run build"

BACKEND_GO="backend-go"
mkdir -p "${ARTIFACTS_DIR}/${BACKEND_GO}"
go build -C "${ROOT_DIR}/${BACKEND_GO}" -o "${ARTIFACTS_DIR}/${BACKEND_GO}"

CONFIG="config"
mkdir -p "${ARTIFACTS_DIR}/${CONFIG}"
cp -rf "${ROOT_DIR}/${CONFIG}/" "${ARTIFACTS_DIR}/"


