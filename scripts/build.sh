#!/bin/bash

# Usage: ./scripts/build.sh [-t <build_type>] [options]
# Options:
#   --help, -h                     Show this help message and exit.
#   --build-type, -t <build_type>  Specify the build type (Debug, Release, etc.). Default is Debug.
#   --all, -a                      Build all targets (default).
#   --docs, -d                     Build documentation.
#   --examples, -e                 Build examples.
#   --coverage, -c                 Build coverage reports.

function usage() {
    echo "Usage: $0 [-t <build_type>] [options]"
    echo "Options:"
    echo "  --help, -h                     Show this help message and exit."
    echo "  --build-dir, -d <build_dir>    Specify the build directory. Default is 'build'."
    echo "  --build-type, -t <build_type>  Specify the build type (Debug|Release). Default is Debug."
    echo "  --all, -a                      Build all targets (default)."
    echo "  --docs, -d                     Build documentation."
    echo "  --examples, -e                 Build examples."
    echo "  --coverage, -c                 Build coverage reports."
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --help|-h)
            usage
            exit 0
            ;;
        --build-dir|-d)
            if [[ -n "$2" ]]; then
                OPT_BUILD_DIR="$2"
                shift 2
            else
                echo "Error: --build-dir requires an argument."
                usage
                exit 1
            fi
            ;;
        --build-type|-t)
            if [[ -n "$2" ]]; then
                OPT_BUILD_TYPE="$2"
                shift 2
            else
                echo "Error: --build-type requires an argument."
                usage
                exit 1
            fi
            ;;
        --all|-a)
            OPT_BUILD_ALL=true
            shift
            ;;
        --docs|-d)
            OPT_BUILD_DOCS=true
            shift
            ;;
        --examples|-e)
            OPT_BUILD_EXAMPLES=true
            shift
            ;;
        --coverage|-c)
            OPT_BUILD_COVERAGE=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

if [[ -z "$OPT_BUILD_DIR" ]]; then
    BUILD_DIR="build"
else
    BUILD_DIR="$OPT_BUILD_DIR"
fi

if [[ -z "$OPT_BUILD_TYPE" ]]; then
    BUILD_TYPE="Debug"
elif [[ "$OPT_BUILD_TYPE" =~ ^[D|d]ebug$ ]]; then
    BUILD_TYPE="Debug"
elif [[ "$OPT_BUILD_TYPE" =~ ^[R|r]elease$ ]]; then
    BUILD_TYPE="Release"
else
    echo "Error: Invalid build type '$OPT_BUILD_TYPE'."
    usage
    exit 1
fi

if [[ "$OPT_BUILD_ALL" == true ]] || [[ "$OPT_BUILD_DOCS" == true ]]; then
    CLOADER_BUILD_DOCS=ON
else
    CLOADER_BUILD_DOCS=OFF
fi
if [[ "$OPT_BUILD_ALL" == true ]] || [[ "$OPT_BUILD_EXAMPLES" == true ]]; then
    CLOADER_BUILD_EXAMPLES=ON
else
    CLOADER_BUILD_EXAMPLES=OFF
fi
if [[ "$OPT_BUILD_ALL" == true ]] || [[ "$OPT_BUILD_COVERAGE" == true ]]; then
    CLOADER_BUILD_COVERAGE=ON
else
    CLOADER_BUILD_COVERAGE=OFF
fi

echo "Build Configuration:"
echo "  - Build Directory: $BUILD_DIR"
echo "  - Build Type: $BUILD_TYPE"
echo "  - Build Docs: $CLOADER_BUILD_DOCS"
echo "  - Build Examples: $CLOADER_BUILD_EXAMPLES"
echo "  - Build Coverage: $CLOADER_BUILD_COVERAGE"

cmake -S . -B "$BUILD_DIR"                              \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"                    \
    -DCLOADER_BUILD_DOCS="$CLOADER_BUILD_DOCS"          \
    -DCLOADER_BUILD_EXAMPLES="$CLOADER_BUILD_EXAMPLES"  \
    -DCLOADER_BUILD_COVERAGE="$CLOADER_BUILD_COVERAGE"

if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

cmake --build "$BUILD_DIR" --parallel
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

if [[ "$CLOADER_BUILD_DOCS" == "ON" ]]; then
    cmake --build "$BUILD_DIR" --target docs
fi

if [[ "$CLOADER_BUILD_COVERAGE" == "ON" ]]; then
    cmake --build "$BUILD_DIR" --target coverage
fi
