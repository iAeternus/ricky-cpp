#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${root_dir}/build"

if [[ ! -f "${build_dir}/CMakeCache.txt" ]]; then
  "${root_dir}/build.sh"
fi

if command -v ctest >/dev/null 2>&1; then
  ctest --test-dir "${build_dir}" --output-on-failure
else
  "${build_dir}/bin/tests/RICKY_CPP_TESTS"
fi
