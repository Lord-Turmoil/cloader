#!/bin/bash

BUILD_ROOT="$(pwd)/build/examples"

APP_BIN="$BUILD_ROOT/app/cloader-example"

EXTENSION_DIR=(
    "$BUILD_ROOT/extension-a"
    "$BUILD_ROOT/extension-b"
)
EXTRA_LIBRARY_PATH=""
for dir in "${EXTENSION_DIR[@]}"; do
    if [ -d "$dir" ]; then
        EXTRA_LIBRARY_PATH="$dir${EXTRA_LIBRARY_PATH:+:$EXTRA_LIBRARY_PATH}"
    fi
done

case "$(uname -s)" in
    Darwin)
        export DYLD_LIBRARY_PATH="$EXTRA_LIBRARY_PATH${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}"
        ;;
    Linux)
        export LD_LIBRARY_PATH="$EXTRA_LIBRARY_PATH${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
        ;;
esac

$APP_BIN "$@"
