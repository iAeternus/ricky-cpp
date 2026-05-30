#!/bin/bash
# 运行已构建的 example（不执行构建）。
#
# 用法:
#   ./run_example.sh                         列出可用 example
#   ./run_example.sh regression_mlp -t       运行指定 example 并传参
#
set -e

cd "$(dirname "$0")"

if [ $# -eq 0 ]; then
    echo "Usage: $0 <example_name> [args...]"
    echo ""
    echo "Available examples:"
    for f in build/bin/examples/*; do
        [ -f "$f" ] && [ -x "$f" ] && basename "$f"
    done | sort
    exit 0
fi

EXAMPLE="$1"
shift

EXE="./build/bin/examples/$EXAMPLE"
if [ ! -f "$EXE" ]; then
    echo "Error: '$EXAMPLE' not found (build/bin/examples/$EXAMPLE missing)"
    echo "Run 'cmake --build build --target $EXAMPLE' first, or build the project."
    exit 1
fi

exec "$EXE" "$@"
