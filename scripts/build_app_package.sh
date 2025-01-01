#!/bin/bash

set -e

function green() {
    echo -e "\033[0;32m${*}\033[0m"
}

script_dir=$(readlink -f "$(dirname "$0")")
project_dir=$(readlink -f "${script_dir}/../..")

echo "script_dir: $script_dir"
echo "project_dir: $project_dir"

cd "$project_dir" || exit

green "Building binary..."

cmake --preset=release
cmake --build build_release
cmake --build build_release --target write_version_info

cd build_release

green "Generating package..."

cpack -G DragNDrop

green "Done"
