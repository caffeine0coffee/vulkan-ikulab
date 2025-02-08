#!/bin/bash

set -e

script_dir=$(readlink -f "$(dirname "$0")")
project_dir=$(readlink -f "${script_dir}/..")

cd "$project_dir" || exit

if [ -z "$1" ]; then
    echo "Usage: $0 <run-clang-tidy executable path>"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "run-clang-tidy executable not found: $1"
    exit 1
fi

if [ ! -x "$1" ]; then
    echo "run-clang-tidy is not executable: $1"
  exit 1
fi

run_clang_tidy_bin="$1"

"${run_clang_tidy_bin}" -p build_develop -use-color
