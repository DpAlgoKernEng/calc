#!/bin/bash
# coverage.sh - Generate code coverage reports for the calc project

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
BUILD_DIR="${BUILD_DIR:-build}"

echo -e "${GREEN}=== Generating Code Coverage Report ===${NC}"

# Parse arguments
TARGET=${TARGET:-all}
THRESHOLD=${THRESHOLD:-90}
OPEN=${OPEN:-0}
CLEAN=${CLEAN:-0}

while [[ $# -gt 0 ]]; do
    case $1 in
        --target=*)
            TARGET="${1#*=}"
            shift
            ;;
        --threshold=*)
            THRESHOLD="${1#*=}"
            shift
            ;;
        --open|-o)
            OPEN=1
            shift
            ;;
        --clean|-c)
            CLEAN=1
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Generate code coverage reports for the calc project."
            echo ""
            echo "Options:"
            echo "  --target=TARGET     Coverage target to build (default: all)"
            echo "                      Available targets: unit, integration, all"
            echo "  --threshold=N       Minimum coverage percentage (default: 90)"
            echo "  --open, -o          Open HTML report in browser"
            echo "  --clean, -c         Clean build directory before coverage"
            echo "  -h, --help          Show this help message"
            echo ""
            echo "Environment variables:"
            echo "  BUILD_DIR           Build directory (default: build)"
            echo "  TARGET              Coverage target (default: all)"
            echo "  THRESHOLD           Minimum coverage (default: 90)"
            echo "  OPEN                Open report after generation (0 or 1)"
            echo "  CLEAN               Clean before build (0 or 1)"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Clean build if requested
if [ "$CLEAN" -eq 1 ]; then
    echo ""
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "${BUILD_DIR}"
fi

# Check if lcov is available
if ! command -v lcov &> /dev/null; then
    echo -e "${RED}Error: lcov not found${NC}"
    echo ""
    echo "Please install lcov:"
    echo "  Ubuntu/Debian: sudo apt-get install lcov"
    echo "  macOS: brew install lcov"
    exit 1
fi

# Check if genhtml is available
if ! command -v genhtml &> /dev/null; then
    echo -e "${RED}Error: genhtml not found${NC}"
    echo ""
    echo "Please install genhtml (usually included with lcov)"
    exit 1
fi

# Display versions
echo ""
echo -e "${YELLOW}Tools:${NC}"
echo "  lcov version: $(lcov --version | head -n 1)"
echo "  genhtml version: $(genhtml --version | head -n 1)"
echo ""

# Configure CMake with coverage
echo -e "${YELLOW}Configuring CMake with coverage enabled...${NC}"
cmake -B "${BUILD_DIR}" -S "${PROJECT_ROOT}" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    -DENABLE_COVERAGE=ON

# Build the project
echo ""
echo -e "${YELLOW}Building project...${NC}"
cmake --build "${BUILD_DIR}" --config Debug -- -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Run tests and generate coverage
echo ""
echo -e "${YELLOW}Running tests and capturing coverage data...${NC}"
cd "${BUILD_DIR}"

# Clean previous coverage data
lcov --directory . --zerocounters 2>/dev/null || true
lcov --directory . --capture --output-file coverage.info 2>/dev/null || true

# Run unit tests
if [ "$TARGET" = "all" ] || [ "$TARGET" = "unit" ]; then
    echo ""
    echo -e "${BLUE}[1/2]${NC} Running unit tests..."
    ctest -R "unit" --output-on-failure --build-config Debug || true
fi

# Run integration tests
if [ "$TARGET" = "all" ] || [ "$TARGET" = "integration" ]; then
    echo ""
    echo -e "${BLUE}[2/2]${NC} Running integration tests..."
    ctest -R "integration" --output-on-failure --build-config Debug || true
fi

# Capture coverage data
lcov --directory . --capture --output-file coverage.raw.info

# Filter out unwanted files
echo ""
echo -e "${YELLOW}Filtering coverage data...${NC}"
lcov --remove coverage.raw.info \
    "/usr/*" \
    "${BUILD_DIR}/*" \
    "*/_deps/*" \
    "*/tests/*" \
    "*/benchmark/*" \
    "*/test_*" \
    "*/include/gtest/*" \
    "*/include/gmock/*" \
    --output-file coverage.info

# Generate HTML report
echo ""
echo -e "${YELLOW}Generating HTML report...${NC}"
genhtml coverage.info \
    --output-directory coverage_html \
    --title "calc - Code Coverage Report" \
    --legend \
    --show-details

# Extract coverage summary
echo ""
echo -e "${GREEN}=== Coverage Summary ===${NC}"
lcov --summary coverage.info

# Extract line coverage percentage
LINE_COV=$(lcov --summary coverage.info 2>&1 | grep "lines" | awk '{print $2}' | tr -d '%')

# Check if threshold is met
echo ""
if [ -n "$LINE_COV" ]; then
    COV_NUM=$(echo "$LINE_COV" | cut -d. -f1)
    if [ "$COV_NUM" -ge "$THRESHOLD" ]; then
        echo -e "${GREEN}✓ Coverage ${LINE_COV}% meets threshold ${THRESHOLD}%${NC}"
    else
        echo -e "${YELLOW}⚠ Coverage ${LINE_COV}% is below threshold ${THRESHOLD}%${NC}"
    fi
fi

# Open report if requested
if [ "$OPEN" -eq 1 ]; then
    echo ""
    echo -e "${YELLOW}Opening coverage report...${NC}"
    if command -v xdg-open &> /dev/null; then
        xdg-open "${BUILD_DIR}/coverage_html/index.html" 2>/dev/null || true
    elif command -v open &> /dev/null; then
        open "${BUILD_DIR}/coverage_html/index.html"
    elif command -v start &> /dev/null; then
        start "${BUILD_DIR}/coverage_html/index.html"
    else
        echo -e "${RED}Could not open browser. Open manually:${NC}"
        echo "  ${BUILD_DIR}/coverage_html/index.html"
    fi
else
    echo ""
    echo -e "${GREEN}Coverage report generated:${NC} ${BUILD_DIR}/coverage_html/index.html"
fi

cd "${PROJECT_ROOT}"
