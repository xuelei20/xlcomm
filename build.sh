#!/bin/sh

echo "====start build xlcomm===="

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-out/build}
BUILD_TYPE=${BUILD_TYPE:-release}
INSTALL_DIR=${INSTALL_DIR:-out/${BUILD_TYPE}-install-cpp14}
CXX=${CXX:-g++}

mkdir -p $BUILD_DIR/$BUILD_TYPE-cpp14 \
  && cd $BUILD_DIR/$BUILD_TYPE-cpp14 \
  && cmake \
           -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
           -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
           -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
           $SOURCE_DIR \
  && make $*