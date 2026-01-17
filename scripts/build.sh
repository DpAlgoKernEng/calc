#!/bin/bash
# build.sh - Build script for calc project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Default values
BUILD_TYPE="Release"
BUILD_DIR="$PROJECT_ROOT/build"
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
CLEAN_BUILD=false
RUN_TESTS=false
COVERAGE=false
VERBOSE=false

# Print usage
print_usage() {
    cat << EOF
Usage: $(basename "$0") [OPTIONS]

Build the calc project.

OPTIONS:
    -t, --type TYPE        Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)
    -d, --dir DIR          Build directory (default: ./build)
    -j, --jobs N           Number of parallel jobs (default: auto-detect)
    -c, --clean            Clean build directory before building
    -t, --test             Run tests after building
    --coverage             Enable code coverage
    -v, --verbose          Verbose output
    -h, --help             Show this help message

EXAMPLES:
    $(basename "$0")                      # Release build
    $(basename "$0") --type Debug         # Debug build
    $(basename "$0") --clean --test       # Clean build and run tests
    $(basename "$0") --coverage           # Build with coverage support

EOF
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -d|--dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        --test)
            RUN_TESTS=true
            shift
            ;;
        --coverage)
            COVERAGE=true
            BUILD_TYPE="Debug"
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option: $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# Validate build type
case "$BUILD_TYPE" in
    Debug|Release|RelWithDebInfo|MinSizeRel)
        ;;
    *)
        echo -e "${RED}Error: Invalid build type: $BUILD_TYPE${NC}"
        print_usage
        exit 1
        ;;
esac

# Print build configuration
echo -e "${GREEN}=== Calc Build Configuration ===${NC}"
echo -e "Build Type:    ${YELLOW}$BUILD_TYPE${NC}"
echo -e "Build Directory: ${YELLOW}$BUILD_DIR${NC}"
echo -e "Parallel Jobs:  ${YELLOW}$JOBS${NC}"
echo -e "Clean Build:    ${YELLOW}$CLEAN_BUILD${NC}"
echo -e "Run Tests:      ${YELLOW}$RUN_TESTS${NC}"
echo -e "Coverage:       ${YELLOW}$COVERAGE${NC}"
echo ""

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo -e "${GREEN}=== Configuring with CMake ===${NC}"
CMAKE_ARGS=()
CMAKE_ARGS+=("-DCMAKE_BUILD_TYPE=$BUILD_TYPE")
CMAKE_ARGS+=("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")

if [ "$COVERAGE" = true ]; then
    CMAKE_ARGS+=("-DENABLE_COVERAGE=ON")
fi

if [ "$VERBOSE" = true ]; then
    CMAKE_ARGS+=("-DCMAKE_VERBOSE_MAKEFILE=ON")
fi

echo -e "CMake arguments: ${CMAKE_ARGS[*]}"
cmake "$PROJECT_ROOT" "${CMAKE_ARGS[@]}"

# Build
echo ""
echo -e "${GREEN}=== Building ===${NC}"
BUILD_ARGS=()
BUILD_ARGS+=("--config" "$BUILD_TYPE")
BUILD_ARGS+=("-j" "$JOBS")

if [ "$VERBOSE" = true ]; then
    BUILD_ARGS+=("--verbose")
fi

cmake --build . "${BUILD_ARGS[@]}"

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
    echo ""
    echo -e "${GREEN}=== Running Tests ===${NC}"
    ctest --output-on-failure
fi

# Generate coverage report if enabled
if [ "$COVERAGE" = true ]; then
    echo ""
    echo -e "${GREEN}=== Generating Coverage Report ===${NC}"
    "$SCRIPT_DIR/coverage.sh"
fi

echo ""
echo -e "${GREEN}=== Build Complete ===${NC}"
echo -e "Binary location: ${BLUE}$BUILD_DIR/${BUILD_TYPE}/calc_cli${NC}"
