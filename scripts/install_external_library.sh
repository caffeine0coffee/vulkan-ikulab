#!/bin/bash

cd `dirname ${0}` && cd ../..
IKURA_DIR=`pwd`

# fetch shaderc source
mkdir ./downloads
cd ./downloads
wget https://github.com/google/shaderc/archive/refs/tags/v2022.3.tar.gz
tar -xf v2022.3.tar.gz

# build shaderc
cd ./shaderc-2022.3
./utils/git-sync-deps
cmake -S . -B build -G Ninja
cmake --build build

# place compiled library
cd $IKURA_DIR

mkdir -p ./libs/shaderc
cp ./downloads/shaderc-2022.3/build/libshaderc/libshaderc_combined.a ./libs/shaderc/shaderc_combined.a

echo "Done"
