#!/bin/bash
# valgrind.sh - Run tests with Valgrind memory leak detection

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

echo -e "${GREEN}=== Running Valgrind Memory Leak Detection ===${NC}"

# Check if valgrind is available
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}Error: valgrind not found${NC}"
    echo ""
    echo "Please install valgrind:"
    echo "  Ubuntu/Debian: sudo apt-get install valgrind"
    echo "  macOS: brew install valgrind"
    exit 1
fi

# Display valgrind version
echo -e "${YELLOW}valgrind version:${NC}"
valgrind --version

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${RED}Error: Build directory '$BUILD_DIR' does not exist${NC}"
    echo ""
    echo "Please build the project first:"
    echo "  mkdir -p ${BUILD_DIR}"
    echo "  cmake -B ${BUILD_DIR} -S . -DCMAKE_BUILD_TYPE=Debug"
    echo "  cmake --build ${BUILD_DIR}"
    exit 1
fi

# Check if tests exist
TESTS_DIR="${BUILD_DIR}/tests/unit"
if [ ! -d "$TESTS_DIR" ]; then
    echo -e "${RED}Error: Test executables not found in ${TESTS_DIR}${NC}"
    echo ""
    echo "Please build with tests enabled:"
    echo "  cmake -B ${BUILD_DIR} -S . -DBUILD_TESTING=ON"
    exit 1
fi

echo ""
echo -e "${YELLOW}Build directory:${NC} ${BUILD_DIR}"
echo ""

# Parse arguments
LEAK_CHECK=${LEAK_CHECK:-full}
SHOW_REACHABLE=${SHOW_REACHABLE:-no}
TRACK_ORIGINS=${TRACK_ORIGINS:-yes}
VERBOSE=${VERBOSE:-0}

while [[ $# -gt 0 ]]; do
    case $1 in
        --leak-check=*)
            LEAK_CHECK="${1#*=}"
            shift
            ;;
        --show-reachable)
            SHOW_REACHABLE="yes"
            shift
            ;;
        --no-track-origins)
            TRACK_ORIGINS="no"
            shift
            ;;
        --verbose|-v)
            VERBOSE=1
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Run tests with Valgrind memory leak detection."
            echo ""
            echo "Options:"
            echo "  --leak-check=LEVEL    Leak check level (no|summary|yes|full, default: full)"
            echo "  --show-reachable      Show reachable leaks (default: no)"
            echo "  --no-track-origins    Disable origin tracking (faster)"
            echo "  --verbose, -v         Verbose output"
            echo "  -h, --help            Show this help message"
            echo ""
            echo "Environment variables:"
            echo "  BUILD_DIR             Build directory (default: build)"
            echo "  LEAK_CHECK            Leak check level (default: full)"
            echo "  SHOW_REACHABLE        Show reachable leaks (default: no)"
            echo "  TRACK_ORIGINS         Track origin of uninitialized values (default: yes)"
            echo "  VERBOSE               Verbose output (default: 0)"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Build valgrind command
VALGRIND_CMD="valgrind"
VALGRIND_CMD="${VALGRIND_CMD} --tool=memcheck"
VALGRIND_CMD="${VALGRIND_CMD} --leak-check=${LEAK_CHECK}"
VALGRIND_CMD="${VALGRIND_CMD} --show-reachable=${SHOW_REACHABLE}"
VALGRIND_CMD="${VALGRIND_CMD} --track-origins=${TRACK_ORIGINS}"
VALGRIND_CMD="${VALGRIND_CMD} --num-callers=50"
VALGRIND_CMD="${VALGRIND_CMD} --error-exitcode=1"

if [ "$VERBOSE" -eq 1 ]; then
    VALGRIND_CMD="${VALGRIND_CMD} --verbose"
else
    VALGRIND_CMD="${VALGRIND_CMD} -q"
fi

# Valgrind suppression file
SUPPRESSIONS_FILE="${PROJECT_ROOT}/scripts/valgrind.supp"
if [ -f "$SUPPRESSIONS_FILE" ]; then
    VALGRIND_CMD="${VALGRIND_CMD} --suppressions=${SUPPRESSIONS_FILE}"
fi

echo -e "${YELLOW}Valgrind command:${NC}"
echo "  ${VALGRIND_CMD}"
echo ""

# Find and run all test executables
TEST_COUNT=0
FAIL_COUNT=0

echo -e "${BLUE}=== Running tests with Valgrind ===${NC}"
echo ""

while IFS= read -r -d '' test_file; do
    TEST_NAME=$(basename "$test_file")
    TEST_COUNT=$((TEST_COUNT + 1))

    echo -e "${BLUE}[${TEST_COUNT}]${NC} Running: ${TEST_NAME}"

    if [ "$VERBOSE" -eq 1 ]; then
        echo "  Executing: ${VALGRIND_CMD} ${test_file}"
    fi

    # Run the test with valgrind
    if ${VALGRIND_CMD} "${test_file}" 2>&1 | tee /tmp/valgrind_output.log; then
        echo -e "  ${GREEN}✓${NC} ${TEST_NAME} passed"
    else
        echo -e "  ${RED}✗${NC} ${TEST_NAME} failed"
        FAIL_COUNT=$((FAIL_COUNT + 1))

        # Show summary if available
        if grep -q "LEAK SUMMARY" /tmp/valgrind_output.log; then
            echo -e "${YELLOW}  Memory leak summary:${NC}"
            grep -A 5 "LEAK SUMMARY" /tmp/valgrind_output.log | sed 's/^/    /'
        fi
    fi

    echo ""
done < <(find "${TESTS_DIR}" \
    -type f \
    -executable \
    -name "*_test" \
    -print0)

# Also check integration tests
INTEGRATION_TESTS_DIR="${BUILD_DIR}/tests/integration"
if [ -d "$INTEGRATION_TESTS_DIR" ]; then
    echo -e "${BLUE}=== Running integration tests with Valgrind ===${NC}"
    echo ""

    while IFS= read -r -d '' test_file; do
        TEST_NAME=$(basename "$test_file")
        TEST_COUNT=$((TEST_COUNT + 1))

        echo -e "${BLUE}[${TEST_COUNT}]${NC} Running: ${TEST_NAME} (integration)"

        if ${VALGRIND_CMD} "${test_file}" 2>&1 | tee /tmp/valgrind_output.log; then
            echo -e "  ${GREEN}✓${NC} ${TEST_NAME} passed"
        else
            echo -e "  ${RED}✗${NC} ${TEST_NAME} failed"
            FAIL_COUNT=$((FAIL_COUNT + 1))
        fi

        echo ""
    done < <(find "${INTEGRATION_TESTS_DIR}" \
        -type f \
        -executable \
        -name "*_test" \
        -print0)
fi

# Summary
echo -e "${GREEN}=== Valgrind Summary ===${NC}"
echo -e "Total tests: ${TEST_COUNT}"
echo -e "Failed:      ${FAIL_COUNT}"
echo -e "Passed:      $((TEST_COUNT - FAIL_COUNT))"
echo ""

if [ $FAIL_COUNT -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed with no memory leaks!${NC}"
    exit 0
else
    echo -e "${RED}✗ ${FAIL_COUNT} test(s) failed or have memory leaks${NC}"
    exit 1
fi
