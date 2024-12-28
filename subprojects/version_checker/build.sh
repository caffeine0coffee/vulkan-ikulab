#!/bin/bash

script_dir="$(realpath "$(dirname "${0}")")"
project_root="$(realpath "${script_dir}/../..")"

cd "${script_dir}" || exit 1
pyinstaller main.py --onefile --distpath . --name imv_version_checker -y

cp imv_version_checker "${project_root}/assets/imv_version_checker"
