#!/bin/bash

set +x
set -e

mkdir -p .sources
cd .sources

CLING_BINARY="cling.tar.bz2"
CURL_CMD="curl -Lso"
SAFE_PWD=$(echo "$PWD" | sed 's/ /\\ /g')

rm -rf /tmp/cpp-mm-scoring
mkdir /tmp/cpp-mm-scoring

function ctrl_c() {
  echo ""
  echo "User interrupted"
  rm -f ${CLING_BINARY} nlohmann.zip
  exit -1
}

cling () {
  if [ ! -f ${CLING_BINARY} ]; then
    echo -e "\x1B[1m\x1B[31mcling binary not found.\x1B[97m\x1B[22m"
    if [[ $(uname -s) == 'Darwin' ]]; then
      MAC_VERSION=$(sw_vers|grep ProductVersion|awk -F" " '{print $2}'|tr -d .)
      echo "Downloading file for Mac OS X `sw_vers|grep ProductVersion|awk -F" " '{print $2}'`"
      $CURL_CMD $CLING_BINARY "https://root.cern.ch/download/cling/cling_2018-11-05_mac${MAC_VERSION}.tar.bz2"
    else
      . /etc/lsb-release
      UBUNTU_VERSION=$(echo $DISTRIB_RELEASE | awk -F"." '{print $1}')
      echo "Downloading file for Ubuntu $DISTRIB_RELEASE"
      $CURL_CMD $CLING_BINARY "https://root.cern.ch/download/cling/cling_2018-11-05_ubuntu${UBUNTU_VERSION}.tar.bz2"
    fi
  fi

  echo -e "\x1B[97m\x1B[22minstalling \x1B[1m\x1B[32mcling\x1B[97m\x1B[22m"
  mkdir -p cling-0.5
  tar --strip-components=1 -xj -f $CLING_BINARY -C cling-0.5/
  for folder in `ls -1 ./cling-0.5/include/`
  do
    rm -f /tmp/cpp-mm-scoring/cling-0.5
    ln -s "${PWD}/cling-0.5" /tmp/cpp-mm-scoring
  done
  return 0
}

nlohmann () {
  if [ ! -f "nlohmann.zip" ]; then
    echo -e "\x1B[1m\x1B[31mnlohmann binary not found. Downloading\x1B[97m\x1B[22m"
    $CURL_CMD nlohmann.zip "https://github.com/nlohmann/json/releases/download/v3.4.0/include.zip"
  fi

  echo -e "\x1B[97m\x1B[22minstalling \x1B[1m\x1B[32mnlohmann\x1B[97m\x1B[22m"
  unzip -qo nlohmann.zip
  
  rm -f /tmp/cpp-mm-scoring/include
  ln -s "${PWD}/include" /tmp/cpp-mm-scoring
  return 0
}

trap ctrl_c INT

nlohmann
cling
