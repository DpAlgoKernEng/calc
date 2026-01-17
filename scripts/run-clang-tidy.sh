#!/bin/bash
# run-clang-tidy.sh - Run clang-tidy static analysis on the project

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

echo -e "${GREEN}=== Running clang-tidy static analysis ===${NC}"

# Check if clang-tidy is available
if ! command -v clang-tidy &> /dev/null; then
    echo -e "${RED}Error: clang-tidy not found${NC}"
    echo "Please install clang-tidy:"
    echo "  Ubuntu/Debian: sudo apt-get install clang-tidy"
    echo "  macOS: brew install llvm"
    echo "  Windows: Download from LLVM website"
    exit 1
fi

# Display clang-tidy version
echo -e "${YELLOW}clang-tidy version:${NC}"
clang-tidy --version | head -n 1

# Check if compile_commands.json exists
COMPILE_COMMANDS="${BUILD_DIR}/compile_commands.json"
if [ ! -f "$COMPILE_COMMANDS" ]; then
    echo -e "${RED}Error: compile_commands.json not found in ${BUILD_DIR}${NC}"
    echo ""
    echo "Please build the project first:"
    echo "  mkdir -p ${BUILD_DIR}"
    echo "  cmake -B ${BUILD_DIR} -S ."
    echo "  cmake --build ${BUILD_DIR}"
    exit 1
fi

echo ""
echo -e "${YELLOW}Using compile commands:${NC} $COMPILE_COMMANDS"

# Command options
FIX=${FIX:-0}
WARNINGS_AS_ERRORS=${WARNINGS_AS_ERRORS:-0}
FILTER=${FILTER:-}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --fix)
            FIX=1
            shift
            ;;
        --warnings-as-errors)
            WARNINGS_AS_ERRORS=1
            shift
            ;;
        --filter=*)
            FILTER="${1#*=}"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --fix               Apply automatic fixes"
            echo "  --warnings-as-errors Treat warnings as errors"
            echo "  --filter=PATTERN    Only check files matching pattern"
            echo "  -h, --help          Show this help message"
            echo ""
            echo "Environment variables:"
            echo "  BUILD_DIR           Build directory (default: build)"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Count files and errors
FILE_COUNT=0
WARNING_COUNT=0
ERROR_COUNT=0

# Build clang-tidy command
TIDY_CMD="clang-tidy"
if [ "$FIX" -eq 1 ]; then
    TIDY_CMD="${TIDY_CMD} --fix"
fi
if [ "$WARNINGS_AS_ERRORS" -eq 1 ]; then
    TIDY_CMD="${TIDY_CMD} --warnings-as-errors='*'"
fi
TIDY_CMD="${TIDY_CMD} -p ${BUILD_DIR}"

# Find and analyze source files
echo ""
echo -e "${YELLOW}Analyzing source files...${NC}"
echo ""

while IFS= read -r -d '' file; do
    # Skip test files if FILTER is set
    if [ -n "$FILTER" ] && [[ ! "$file" =~ $FILTER ]]; then
        continue
    fi

    ((FILE_COUNT++))
    REL_PATH="${file#$PROJECT_ROOT/}"

    echo -e "${BLUE}[${FILE_COUNT}]${NC} Analyzing: ${REL_PATH}"

    # Run clang-tidy and capture output
    OUTPUT=$($TIDY_CMD "$file" 2>&1 || true)

    if [ -n "$OUTPUT" ]; then
        # Count warnings and errors
        W=$(echo "$OUTPUT" | grep -c "warning:" || true)
        E=$(echo "$OUTPUT" | grep -c "error:" || true)
        ERROR_FILES=$(echo "$OUTPUT" | grep -c "error generated" || true)

        WARNING_COUNT=$((WARNING_COUNT + W))
        ERROR_COUNT=$((ERROR_COUNT + E + ERROR_FILES))

        # Display output
        echo "$OUTPUT" | while IFS= read -r line; do
            if [[ $line == *"error:"* ]]; then
                echo -e "  ${RED}${line}${NC}"
            elif [[ $line == *"warning:"* ]]; then
                echo -e "  ${YELLOW}${line}${NC}"
            else
                echo "  ${line}"
            fi
        done
    else
        echo -e "  ${GREEN}✓ No issues found${NC}"
    fi
    echo ""
done < <(find "$PROJECT_ROOT/src" \
    -type f \
    \( -name "*.cpp" -o -name "*.h" \) \
    -print0)

echo ""
echo -e "${GREEN}=== Summary ===${NC}"
echo -e "Total files analyzed: ${FILE_COUNT}"
echo -e "Warnings found: ${WARNING_COUNT}"
echo -e "Errors found: ${ERROR_COUNT}"

if [ $ERROR_COUNT -gt 0 ]; then
    echo ""
    echo -e "${RED}Static analysis found errors!${NC}"
    exit 1
elif [ $WARNING_COUNT -gt 0 ]; then
    echo ""
    echo -e "${YELLOW}Static analysis found warnings${NC}"
    exit 0
else
    echo ""
    echo -e "${GREEN}✓ No issues found!${NC}"
    exit 0
fi
