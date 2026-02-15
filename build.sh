#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${root_dir}/build"

generator=""
if command -v ninja >/dev/null 2>&1; then
  generator="Ninja"
else
  generator="Unix Makefiles"
fi

build_type="${BUILD_TYPE:-Debug}"
cmake -S "${root_dir}" -B "${build_dir}" -G "${generator}" -DCMAKE_BUILD_TYPE="${build_type}"
cmake --build "${build_dir}" --parallel
