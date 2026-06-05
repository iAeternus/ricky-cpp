#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${root_dir}/build"

generator="Ninja"

build_type="${BUILD_TYPE:-Debug}"
cmake -S "${root_dir}" -B "${build_dir}" -G "${generator}" -DCMAKE_BUILD_TYPE="${build_type}"
cmake --build "${build_dir}" --parallel
