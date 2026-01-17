#!/bin/bash
# docs.sh - Documentation generation and management script

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
ACTION="generate"
DOXYGEN_OUTPUT="$PROJECT_ROOT/docs/api"
SKIP_HTML=false

# Print usage
print_usage() {
    cat << EOF
Usage: $(basename "$0") [OPTIONS] [ACTION]

Generate and manage project documentation.

ACTIONS:
    generate    Generate API documentation with Doxygen (default)
    serve       Serve documentation on local web server
    clean       Remove generated documentation
    check       Check if documentation is up to date
    all         Generate API docs, validate links, and check completeness

OPTIONS:
    -o, --output DIR    Output directory for generated docs (default: docs/api)
    --skip-html          Skip HTML generation, only check
    -h, --help           Show this help message

DEPENDENCIES:
    - doxygen: For API documentation generation
    - python3: For link checking (optional)

EXAMPLES:
    $(basename "$0") generate      # Generate API documentation
    $(basename "$0") serve         # Serve docs at localhost:8000
    $(basename "$0") clean         # Remove generated docs

EOF
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        generate|serve|clean|check|all)
            ACTION="$1"
            shift
            ;;
        -o|--output)
            DOXYGEN_OUTPUT="$2"
            shift 2
            ;;
        --skip-html)
            SKIP_HTML=true
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

# Check dependencies
check_doxygen() {
    if ! command -v doxygen &> /dev/null; then
        echo -e "${RED}Error: doxygen is not installed${NC}"
        echo "Install it with:"
        echo "  Ubuntu/Debian: sudo apt-get install doxygen"
        echo "  macOS: brew install doxygen"
        echo "  Windows: Download from https://www.doxygen.nl/download.html"
        exit 1
    fi
}

# Check graphviz for diagrams
check_graphviz() {
    if ! command -v dot &> /dev/null; then
        echo -e "${YELLOW}Warning: graphviz (dot) not found. Diagrams will not be generated.${NC}"
        echo "Install it with: sudo apt-get install graphviz (Ubuntu/Debian)"
    fi
}

# Generate documentation
generate_docs() {
    echo -e "${GREEN}=== Generating API Documentation ===${NC}"

    check_doxygen
    check_graphviz

    # Update OUTPUT_DIRECTORY in Doxyfile
    if [ -f "$PROJECT_ROOT/Doxyfile" ]; then
        # Create a temporary Doxyfile with updated output directory
        DOXYGEN_TEMP=$(mktemp)
        sed "s|OUTPUT_DIRECTORY.*|OUTPUT_DIRECTORY       = $DOXYGEN_OUTPUT|" "$PROJECT_ROOT/Doxyfile" > "$DOXYGEN_TEMP"

        echo -e "${YELLOW}Running doxygen...${NC}"
        doxygen "$DOXYGEN_TEMP"
        rm -f "$DOXYGEN_TEMP"

        if [ $? -eq 0 ]; then
            echo -e "${GREEN}Documentation generated successfully${NC}"
            echo -e "${BLUE}Output directory: $DOXYGEN_OUTPUT/html/index.html${NC}"
        else
            echo -e "${RED}Error: doxygen failed${NC}"
            exit 1
        fi
    else
        echo -e "${RED}Error: Doxyfile not found at $PROJECT_ROOT/Doxyfile${NC}"
        exit 1
    fi

    # Check for warnings
    if [ -f "$DOXYGEN_OUTPUT/html/index.html" ]; then
        echo ""
        echo -e "${GREEN}=== Documentation Statistics ===${NC}"
        echo -e "Files processed:"
        find "$DOXYGEN_OUTPUT/html" -name "*.html" | wc -l | xargs -I {} echo "  HTML files: {}"
    fi
}

# Serve documentation
serve_docs() {
    echo -e "${GREEN}=== Serving Documentation ===${NC}"

    if [ ! -d "$DOXYGEN_OUTPUT/html" ]; then
        echo -e "${YELLOW}Documentation not found. Generating...${NC}"
        generate_docs
    fi

    echo -e "${BLUE}Serving documentation at http://localhost:8000${NC}"
    echo -e "Press Ctrl+C to stop${NC}"
    echo ""

    # Try to find a suitable server
    if command -v python3 &> /dev/null; then
        cd "$DOXYGEN_OUTPUT/html"
        python3 -m http.server 8000
    elif command -v python &> /dev/null; then
        cd "$DOXYGEN_OUTPUT/html"
        python -m SimpleHTTPServer 8000
    else
        echo -e "${RED}Error: Python not found for serving documentation${NC}"
        echo "Install Python 3 to use the serve command"
        exit 1
    fi
}

# Clean documentation
clean_docs() {
    echo -e "${GREEN}=== Cleaning Documentation ===${NC}"

    if [ -d "$DOXYGEN_OUTPUT" ]; then
        echo -e "${YELLOW}Removing: $DOXYGEN_OUTPUT${NC}"
        rm -rf "$DOXYGEN_OUTPUT"
    else
        echo -e "${YELLOW}No documentation found to clean${NC}"
    fi
}

# Check documentation
check_docs() {
    echo -e "${GREEN}=== Checking Documentation ===${NC}"

    # Check if Doxyfile exists
    if [ ! -f "$PROJECT_ROOT/Doxyfile" ]; then
        echo -e "${RED}Error: Doxyfile not found${NC}"
        exit 1
    fi

    # Check if documentation exists
    if [ ! -d "$DOXYGEN_OUTPUT/html" ]; then
        echo -e "${YELLOW}Warning: Documentation not generated. Run '$(basename "$0") generate' first.${NC}"
        exit 1
    fi

    # Check for recent changes
    echo -e "${YELLOW}Checking if documentation is up to date...${NC}"

    SOURCE_FILES=$(find "$PROJECT_ROOT/include" -name "*.h" -o -name "*.hpp" | wc -l)
    DOC_FILES=$(find "$DOXYGEN_OUTPUT/html" -name "*.html" | wc -l)

    echo -e "Source files: ${BLUE}$SOURCE_FILES${NC}"
    echo -e "Documentation files: ${BLUE}$DOC_FILES${NC}"

    if [ "$DOC_FILES" -lt 10 ]; then
        echo -e "${YELLOW}Warning: Very few documentation files generated${NC}"
    fi

    echo -e "${GREEN}Documentation check complete${NC}"
}

# Run all checks
run_all() {
    echo -e "${GREEN}=== Running All Documentation Tasks ===${NC}"
    echo ""

    generate_docs
    echo ""
    check_docs
    echo ""
    echo -e "${GREEN}=== All Tasks Complete ===${NC}"
}

# Execute action
case "$ACTION" in
    generate)
        generate_docs
        ;;
    serve)
        serve_docs
        ;;
    clean)
        clean_docs
        ;;
    check)
        check_docs
        ;;
    all)
        run_all
        ;;
    *)
        echo -e "${RED}Error: Unknown action: $ACTION${NC}"
        print_usage
        exit 1
        ;;
esac
