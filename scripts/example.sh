#!/bin/bash

BUILD_ROOT="$(pwd)/build"

APP_BIN="$BUILD_ROOT/bin/cloader-example"
EXTRA_LIBRARY_PATH="$BUILD_ROOT/lib"

case "$(uname -s)" in
    Darwin)
        export DYLD_LIBRARY_PATH="$EXTRA_LIBRARY_PATH${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}"
        ;;
    Linux)
        export LD_LIBRARY_PATH="$EXTRA_LIBRARY_PATH${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
        ;;
esac

$APP_BIN "$@"
