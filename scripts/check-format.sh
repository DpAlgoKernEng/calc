#!/bin/bash
# check-format.sh - Check code formatting without modifying files

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo -e "${GREEN}=== Checking calc code formatting ===${NC}"

# Check if clang-format is available
if ! command -v clang-format &> /dev/null; then
    echo -e "${RED}Error: clang-format not found${NC}"
    exit 1
fi

# Display clang-format version
echo -e "${YELLOW}clang-format version:${NC}"
clang-format --version

# Count files and errors
FILE_COUNT=0
ERROR_COUNT=0

# Check all C++ source files
echo ""
echo -e "${YELLOW}Checking files...${NC}"

while IFS= read -r -d '' file; do
    ((FILE_COUNT++))
    FILENAME=$(basename "$file")
    REL_PATH="${file#$PROJECT_ROOT/}"

    # Check formatting
    if ! clang-format --dry-run --Werror "$file" 2>/dev/null; then
        ((ERROR_COUNT++))
        echo -e "  ${RED}✗${NC} $REL_PATH needs formatting"
    fi
done < <(find "$PROJECT_ROOT" \
    -type f \
    \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) \
    ! -path "*/build/*" \
    ! -path "*/.git/*" \
    ! -path "*/_deps/*" \
    -print0)

echo ""
echo -e "${GREEN}=== Summary ===${NC}"
echo -e "Total files checked: ${FILE_COUNT}"

if [ $ERROR_COUNT -eq 0 ]; then
    echo -e "${GREEN}✓ All files are properly formatted!${NC}"
    exit 0
else
    echo -e "${RED}✗ $ERROR_COUNT file(s) need formatting${NC}"
    echo ""
    echo "Run '${YELLOW}./scripts/format.sh${NC}' to fix formatting issues"
    echo "Or review specific file with: ${YELLOW}clang-format --dry-run <file>${NC}"
    exit 1
fi
