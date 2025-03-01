#!/bin/bash

version="$(cat msvc_version.txt)"
cp "/c/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Redist/MSVC/$version"/vc_redist.* .
