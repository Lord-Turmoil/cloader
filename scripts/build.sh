#!/bin/bash

function usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  --help, -h                          Show this help message and exit."
    echo "  --build-dir     <build_dir>         Specify the build directory. Default is 'build'."
    echo "  --build-type, -b <build_type>       Specify the build type (Debug|Release). Default is Debug."
    echo "  --all, -a                           Build the library, examples, docs and coverage reports (default)."
    echo "  --tests, -t                         Build tests."
    echo "  --docs, -d                          Build documentation."
    echo "  --examples, -e                      Build examples."
    echo "  --coverage, -c                      Build coverage reports."
}

BUILD_DIR="build"
BUILD_TYPE="Debug"
BUILD_ALL=false
BUILD_TESTS=OFF
BUILD_DOCS=OFF
BUILD_EXAMPLES=OFF
BUILD_COVERAGE=OFF

while [[ $# -gt 0 ]]; do
    case "$1" in
        --help|-h)
            usage
            exit 0
            ;;
        --build-dir)
            if [[ -n "$2" ]]; then
                BUILD_DIR="$2"
                shift 2
            else
                echo "Error: --build-dir requires an argument."
                usage
                exit 1
            fi
            ;;
        --build-type|-b)
            if [[ -n "$2" ]]; then
                case "$2" in
                    [Dd]ebug) BUILD_TYPE="Debug" ;;
                    [Rr]elease) BUILD_TYPE="Release" ;;
                    *)
                        echo "Error: Invalid build type '$2'."
                        usage
                        exit 1
                        ;;
                esac
                shift 2
            else
                echo "Error: --build-type requires an argument."
                usage
                exit 1
            fi
            ;;
        --all|-a)
            BUILD_ALL=true
            shift
            ;;
        --tests|-t)
            BUILD_TESTS=ON
            shift
            ;;
        --docs|-d)
            BUILD_DOCS=ON
            shift
            ;;
        --examples|-e)
            BUILD_EXAMPLES=ON
            shift
            ;;
        --coverage|-c)
            BUILD_COVERAGE=ON
            shift
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# "All" is the default when no specific build option is requested.
if [[ "$BUILD_ALL" == true ]]; then
    BUILD_TESTS=ON
    BUILD_DOCS=ON
    BUILD_EXAMPLES=ON
    BUILD_COVERAGE=ON
fi

echo "Build Configuration:"
echo "  - Build Directory: $BUILD_DIR"
echo "  - Build Type: $BUILD_TYPE"
echo "  - Build Tests: $BUILD_TESTS"
echo "  - Build Docs: $BUILD_DOCS"
echo "  - Build Examples: $BUILD_EXAMPLES"
echo "  - Build Coverage: $BUILD_COVERAGE"

cmake -S . -B "$BUILD_DIR"                              \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"                    \
    -DCLOADER_BUILD_DOCS="$BUILD_DOCS"                  \
    -DCLOADER_BUILD_EXAMPLES="$BUILD_EXAMPLES"          \
    -DCLOADER_BUILD_TESTS="$BUILD_TESTS"                \
    -DCLOADER_BUILD_COVERAGE="$BUILD_COVERAGE"
if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

# Multi-config generators (Visual Studio, Ninja Multi-Config) ignore
# CMAKE_BUILD_TYPE and select the configuration at build time instead.
CONFIG_ARGS=()
if grep -q "^CMAKE_CONFIGURATION_TYPES:" "$BUILD_DIR/CMakeCache.txt" 2>/dev/null; then
    CONFIG_ARGS=(--config "$BUILD_TYPE")
fi

cmake --build "$BUILD_DIR" --parallel "${CONFIG_ARGS[@]}"
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

if [[ "$BUILD_DOCS" == "ON" ]]; then
    cmake --build "$BUILD_DIR" --target docs "${CONFIG_ARGS[@]}"
fi

if [[ "$BUILD_TESTS" == "ON" ]]; then
    cmake --build "$BUILD_DIR" --target tests "${CONFIG_ARGS[@]}"
fi

if [[ "$BUILD_COVERAGE" == "ON" ]]; then
    cmake --build "$BUILD_DIR" --target coverage "${CONFIG_ARGS[@]}"
fi
