#!/bin/bash

ls "/c/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Redist/MSVC" \
  | grep -v "v" \
  | tail -n 1 \
  | tee msvc_version.txt
