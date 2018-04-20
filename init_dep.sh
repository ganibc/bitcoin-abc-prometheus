#!/bin/bash

wget https://github.com/google/protobuf/releases/download/v3.5.1/protobuf-all-3.5.1.tar.gz
tar zxf protobuf-all-3.5.1.tar.gz
cd protobuf-3.5.1
./autogen.sh
./configure
make -j15
sudo make install

sudo rm /usr/lib/x86_64-linux-gnu/libprotobuf*

sudo ldconfig

cd ..

git clone https://github.com/jupp0r/prometheus-cpp.git
cd prometheus-cpp
git checkout tags/v0.2
git submodule init
git submodule update
cmake . -DBUILD_SHARED_LIBS=ON -DCMAKE_POSITION_INDEPENDENT_CODE=ON
make -j15
sudo make install

sudo ldconfig