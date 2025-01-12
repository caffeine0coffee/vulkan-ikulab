#!/bin/bash

script_dir=$(dirname "$0")
project_dir=$(realpath "$script_dir/..")

cd "$project_dir" || exit
mkdir -p "$project_dir/assets-for-dev"

# copy fonts
cp -r "$project_dir/assets/fonts" "$project_dir/assets-for-dev"

# generate version info file
cmake --preset=build_develop
cmake --build build_develop --target write_version_info_for_develop