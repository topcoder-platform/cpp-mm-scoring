#!/bin/bash

set +x
set -e

mkdir -p .sources
cd .sources

CLING_BINARY="cling.tar.bz2"
BOOST_BINARY="boost.tar.bz2"
CURL_CMD="curl -Lo"
SAFE_PWD=$(echo "$PWD" | sed 's/ /\\ /g')
BASE_DIR=$HOME/.cpp-mm-scoring

mkdir -p $BASE_DIR
rm -rf ${BASE_DIR}/*

function ctrl_c() {
  echo ""
  echo "User interrupted"
  rm -f ${CLING_BINARY} ${BOOST_BINARY} nlohmann.zip
  exit -1
}

cling () {
  if [ ! -f ${CLING_BINARY} ]; then
    echo -e "\x1B[1m\x1B[31mcling binary not found.\x1B[97m\x1B[22m"
    if [[ $(uname -s) == 'Darwin' ]]; then
      MAC_VERSION=$(sw_vers|grep ProductVersion|awk -F" " '{print $2}'|tr -d .)
      # could exist 10126 or 10.12.6
      MAC_VERSION=${MAC_VERSION::4}
      echo "Downloading file for Mac OS X `sw_vers|grep ProductVersion|awk -F" " '{print $2}'`"
      $CURL_CMD $CLING_BINARY "https://root.cern.ch/download/cling/cling_2018-11-05_mac${MAC_VERSION}.tar.bz2"
    else
      . /etc/lsb-release
      UBUNTU_VERSION=$(echo $DISTRIB_RELEASE | awk -F"." '{print $1}')
      echo "Downloading file for debian"
      $CURL_CMD $CLING_BINARY "https://root.cern.ch/download/cling/cling_2018-11-05_ubuntu${UBUNTU_VERSION}.tar.bz2"
    fi
  fi

  echo -e "\x1B[97m\x1B[22minstalling \x1B[1m\x1B[32mcling\x1B[97m\x1B[22m"
  mkdir -p cling-0.5
  tar --strip-components=1 -xj -f $CLING_BINARY -C $BASE_DIR/cling-0.5/
  #ln -s "${PWD}/cling-0.5" ${BASE_DIR}/
  return 0
}

nlohmann () {
  if [ ! -f "nlohmann.zip" ]; then
    echo -e "\x1B[1m\x1B[31mnlohmann binary not found. Downloading\x1B[97m\x1B[22m"
    $CURL_CMD nlohmann.zip "https://github.com/nlohmann/json/releases/download/v3.4.0/include.zip"
  fi

  echo -e "\x1B[97m\x1B[22minstalling \x1B[1m\x1B[32mnlohmann\x1B[97m\x1B[22m"
  unzip -qo $BASE_DIR/nlohmann.zip
  
  #ln -s "${PWD}/include" ${BASE_DIR}/
  return 0
}

boost () {
  if [ ! -f ${BOOST_BINARY} ]; then
    echo -e "\x1B[1m\x1B[31mboost binary not found. Downloading\x1B[97m\x1B[22m"
    $CURL_CMD $BOOST_BINARY "https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.bz2"
  fi

  echo -e "\x1B[97m\x1B[22minstalling \x1B[1m\x1B[32mboost\x1B[97m\x1B[22m"
  mkdir -p boost
  tar --strip-components=1 -xj -f $BOOST_BINARY -C $BASE_DIR/boost/

 # ln -s "${PWD}/boost" ${BASE_DIR}/
  return 0
}
trap ctrl_c INT

nlohmann
cling
boost
