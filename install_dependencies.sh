#!/usr/bin/env bash

mkdir -p ext/simdjson/include
mkdir ext/simdjson/src

wget https://raw.githubusercontent.com/simdjson/simdjson/master/singleheader/simdjson.h -O ext/simdjson/include/simdjson.h
wget https://raw.githubusercontent.com/simdjson/simdjson/master/singleheader/simdjson.cpp -O ext/simdjson/src/simdjson.cpp
wget https://raw.githubusercontent.com/simdjson/simdjson/master/jsonexamples/twitter.json -O data/twitter.json

wget "https://github.com/google/or-tools/releases/download/v9.4/or-tools_arm64_MacOsX-12.5_cpp_v9.4.1874.tar.gz" -O ext/or-tools.tar.gz
tar -xvf ext/or-tools.tar.gz -C ext -s/^or-tools_cpp_MacOsX-12.5_v9.4.1874/or-tools/
rm ext/or-tools.tar.gz
