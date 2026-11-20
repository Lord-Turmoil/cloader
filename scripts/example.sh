#!/bin/bash

# usage: example.sh <example_name> [args...]

if [ $# -lt 1 ]; then
    echo "Usage: $0 <example_name> [args...]"
    echo "Available examples:"
    for example in $(ls examples); do
        if [ -d "examples/$example" ]; then
            echo "  - $example"
        fi
    done
    exit 1
fi

EXAMPLE_NAME="$1"
shift


BUILD_ROOT="$(pwd)/build"
APP_BIN="$BUILD_ROOT/bin/$EXAMPLE_NAME"
EXTRA_LIBRARY_PATH="$BUILD_ROOT/lib"

case "$(uname -s)" in
    Darwin)
        export DYLD_LIBRARY_PATH="$EXTRA_LIBRARY_PATH${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}"
        ;;
    Linux)
        export LD_LIBRARY_PATH="$EXTRA_LIBRARY_PATH${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        # Windows keeps the plugin DLLs next to the executable, so no library
        # path is needed - just the .exe suffix.
        APP_BIN="$APP_BIN.exe"
        ;;
esac

$APP_BIN "$@"
