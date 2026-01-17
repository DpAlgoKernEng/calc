#!/bin/bash
# format.sh - Format source code using clang-format

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo -e "${GREEN}=== Formatting calc source code with clang-format ===${NC}"

# Check if clang-format is available
if ! command -v clang-format &> /dev/null; then
    echo -e "${RED}Error: clang-format not found${NC}"
    echo "Please install clang-format:"
    echo "  Ubuntu/Debian: sudo apt-get install clang-format"
    echo "  macOS: brew install clang-format"
    echo "  Windows: Download from LLVM website"
    exit 1
fi

# Display clang-format version
echo -e "${YELLOW}clang-format version:${NC}"
clang-format --version

# Count files
FILE_COUNT=0
CHANGED_COUNT=0

# Find and format all C++ source files
echo ""
echo -e "${YELLOW}Processing files...${NC}"

while IFS= read -r -d '' file; do
    ((FILE_COUNT++))
    FILENAME=$(basename "$file")

    # Format the file
    if ! clang-format -i "$file"; then
        echo -e "${RED}Failed to format: $file${NC}"
    fi

    # Check if file was changed
    if [ -n "$(git diff --name-only "$file" 2>/dev/null)" ]; then
        ((CHANGED_COUNT++))
        echo -e "  ${GREEN}✓${NC} Formatted: $FILENAME"
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
echo -e "Total files processed: ${FILE_COUNT}"
echo -e "Files changed: ${CHANGED_COUNT}"

if [ $CHANGED_COUNT -gt 0 ]; then
    echo ""
    echo -e "${YELLOW}Note: Files were modified. Review changes with git diff.${NC}"
else
    echo ""
    echo -e "${GREEN}All files are already formatted!${NC}"
fi
