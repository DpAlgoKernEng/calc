#!/bin/bash
# run-cppcheck.sh - Run cppcheck static analysis on the project

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

echo -e "${GREEN}=== Running cppcheck static analysis ===${NC}"

# Check if cppcheck is available
if ! command -v cppcheck &> /dev/null; then
    echo -e "${RED}Error: cppcheck not found${NC}"
    echo ""
    echo "Please install cppcheck:"
    echo "  Ubuntu/Debian: sudo apt-get install cppcheck"
    echo "  macOS: brew install cppcheck"
    echo "  Windows: Download from http://cppcheck.sourceforge.net/"
    exit 1
fi

# Display cppcheck version
echo -e "${YELLOW}cppcheck version:${NC}"
cppcheck --version

# Parse arguments
ENABLE_ALL=${ENABLE_ALL:-no}
ENABLE_WARNING_AS_ERROR=${ENABLE_WARNING_AS_ERROR:-no}
JOBS=${JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}
XML_OUTPUT=""
INLINE_SUPPRESSIONS=${INLINE_SUPPRESSIONS:-no}
VERBOSE=${VERBOSE:-0}

while [[ $# -gt 0 ]]; do
    case $1 in
        --enable-all)
            ENABLE_ALL="yes"
            shift
            ;;
        --warning-as-error)
            ENABLE_WARNING_AS_ERROR="yes"
            shift
            ;;
        --jobs=*)
            JOBS="${1#*=}"
            shift
            ;;
        --output=*)
            XML_OUTPUT="${1#*=}"
            shift
            ;;
        --inline-suppressions)
            INLINE_SUPPRESSIONS="yes"
            shift
            ;;
        --verbose|-v)
            VERBOSE=1
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Run cppcheck static analysis on the calc project."
            echo ""
            echo "Options:"
            echo "  --enable-all         Enable all checks (default: basic checks only)"
            echo "  --warning-as-error   Treat warnings as errors"
            echo "  --jobs=N            Number of parallel jobs (default: $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4))"
            echo "  --output=FILE        Output XML report to file"
            echo "  --inline-suppressions Show inline suppressed warnings"
            echo "  --verbose, -v        Verbose output"
            echo "  -h, --help          Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Build cppcheck command
CPPCHECK_CMD="cppcheck"
CPPCHECK_CMD="${CPPCHECK_CMD} -j${JOBS}"
CPPCHECK_CMD="${CPPCHECK_CMD} --std=c++17"
CPPCHECK_CMD="${CPPCHECK_CMD} --enable=unusedFunction"
CPPCHECK_CMD="${CPPCHECK_CMD} --enable=missingInclude"

# Enable additional checks if requested
if [ "$ENABLE_ALL" = "yes" ]; then
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=all"
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=warning"
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=performance"
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=portability"
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=style"
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=information"
else
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=warning"
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=performance"
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=portability"
    CPPCHECK_CMD="${CPPCHECK_CMD} --enable=style"
fi

# Error handling
CPPCHECK_CMD="${CPPCHECK_CMD} --error-exitcode=1"

# Suppressions
SUPPRESSIONS_FILE="${PROJECT_ROOT}/.cppcheck-suppressions.txt"
if [ -f "$SUPPRESSIONS_FILE" ]; then
    CPPCHECK_CMD="${CPPCHECK_CMD} --suppressions-list=${SUPPRESSIONS_FILE}"
fi

# Inline suppressions
if [ "$INLINE_SUPPRESSIONS" = "yes" ]; then
    CPPCHECK_CMD="${CPPCHECK_CMD} --inline-suppr"
fi

# Output format
if [ -n "$XML_OUTPUT" ]; then
    CPPCHECK_CMD="${CPPCHECK_CMD} --xml --xml-version=2 2> ${XML_OUTPUT}"
else
    CPPCHECK_CMD="${CPPCHECK_CMD} --template='{file}:{line}: {severity}: {id}: {message}'"
fi

# Verbose
if [ "$VERBOSE" -eq 1 ]; then
    CPPCHECK_CMD="${CPPCHECK_CMD} --verbose"
fi

# Include directories
CPPCHECK_CMD="${CPPCHECK_CMD} -I ${PROJECT_ROOT}/include"

# Paths to check
CPPCHECK_CMD="${CPPCHECK_CMD} ${PROJECT_ROOT}/src"

echo ""
echo -e "${YELLOW}cppcheck command:${NC}"
echo "  ${CPPCHECK_CMD}"
echo ""

# Run cppcheck
echo -e "${BLUE}Analyzing source files...${NC}"
echo ""

if [ "$VERBOSE" -eq 1 ]; then
    echo "  Scanning: ${PROJECT_ROOT}/src"
    echo ""
fi

# Run the analysis
if eval $CPPCHECK_CMD; then
    echo ""
    echo -e "${GREEN}=== Summary ===${NC}"
    echo -e "${GREEN}✓ No issues found!${NC}"
    exit 0
else
    # Count errors and warnings
    if [ -n "$XML_OUTPUT" ]; then
        ERRORS=$(grep -c "<error" "$XML_OUTPUT" 2>/dev/null || echo "0")
        WARNINGS=$(grep -c "<error severity=\"warning\"" "$XML_OUTPUT" 2>/dev/null || echo "0")

        echo ""
        echo -e "${GREEN}=== Summary ===${NC}"
        echo -e "Errors:   ${ERRORS}"
        echo -e "Warnings: ${WARNINGS}"
    else
        echo ""
        echo -e "${GREEN}=== Summary ===${NC}"
        echo -e "${RED}Issues found by cppcheck${NC}"
    fi

    if [ "$ENABLE_WARNING_AS_ERROR" = "yes" ]; then
        exit 1
    fi
fi
