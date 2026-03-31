#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 [example_name]"
    echo "Example: $0 tcp_server"
    echo ""
    echo "Available examples:"
    ls -1 build/bin/examples/*  2>/dev/null | xargs -n1 basename || echo "No examples found"
    exit 1
fi

EXAMPLE="$1"
EXAMPLE_DIR="build/bin/examples"

if [ ! -f "$EXAMPLE_DIR/$EXAMPLE" ]; then
    echo "Error: Example '$EXAMPLE' not found"
    echo "Available examples:"
    ls -1 "$EXAMPLE_DIR" 2>/dev/null | xargs -n1 basename || echo "No examples found"
    exit 1
fi

"$EXAMPLE_DIR/$EXAMPLE"
