#!/bin/bash

cd ..
find ./src -name "*.?pp" | xargs clang-format -i -style=file