#!/bin/bash

script_dir=$(dirname "${0}")
project_dir=$(realpath "${script_dir}/..")
cd "${project_dir}" || exit

resource_dir_for_dev="$(cmake --preset=build_develop -N | grep RESOURCE_DIR | sed -n 's/.*"\(.*\)".*/\1/p')"
mkdir -p "${resource_dir_for_dev}"

# copy fonts
cp -r "${project_dir}/assets/fonts" "${resource_dir_for_dev}/fonts"

# generate version info file
cmake --preset=build_develop
cmake --build build_develop --target write_version_info_for_develop
