#!/bin/bash
# install.sh - Installation script for calc

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
PREFIX="${PREFIX:-/usr/local}"
UNINSTALL=false
SKIP_SYMLINK=false

# Check if running as root
check_sudo() {
    if [ "$EUID" -ne 0 ] && [ "$PREFIX" = "/usr/local" ] || [ "$PREFIX" = "/usr" ]; then
        echo -e "${YELLOW}This script requires root privileges to install to $PREFIX${NC}"
        echo -e "Run with sudo or use --prefix to install to a different location"
        exit 1
    fi
}

# Print usage
print_usage() {
    cat << EOF
Usage: $(basename "$0") [OPTIONS] [--]

Install or uninstall calc.

OPTIONS:
    -p, --prefix PREFIX   Installation prefix (default: /usr/local)
    -u, --uninstall       Uninstall calc instead of installing
    -s, --skip-symlink   Skip creating symlinks in bin/
    -h, --help           Show this help message

INSTALLATION LOCATIONS:
    Binary:    \$PREFIX/bin/calc_cli
    Headers:   \$PREFIX/include/calc/
    Docs:      \$PREFIX/share/doc/calc/
    License:   \$PREFIX/share/licenses/calc/

EXAMPLES:
    sudo $(basename "$0")                        # Install to /usr/local
    sudo $(basename "$0") --prefix /usr          # Install to /usr
    $(basename "$0") --prefix \$HOME/.local      # Install to home directory
    sudo $(basename "$0") --uninstall             # Uninstall from /usr/local

EOF
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -p|--prefix)
            PREFIX="$2"
            shift 2
            ;;
        -u|--uninstall)
            UNINSTALL=true
            shift
            ;;
        -s|--skip-symlink)
            SKIP_SYMLINK=true
            shift
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        --)
            shift
            break
            ;;
        *)
            echo -e "${RED}Error: Unknown option: $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# Validate prefix
if [ ! -d "$PREFIX" ] && ! mkdir -p "$PREFIX"; then
    echo -e "${RED}Error: Cannot create prefix directory: $PREFIX${NC}"
    exit 1
fi

# Check sudo if needed
if [ "$UNINSTALL" = false ]; then
    check_sudo
fi

# Print configuration
echo -e "${GREEN}=== Calc Installation ===${NC}"
echo -e "Action:    ${YELLOW}$([ "$UNINSTALL" = true ] && echo "Uninstall" || echo "Install")${NC}"
echo -e "Prefix:    ${YELLOW}$PREFIX${NC}"
echo -e "Skip Symlink: ${YELLOW}$SKIP_SYMLINK${NC}"
echo ""

# Uninstall
if [ "$UNINSTALL" = true ]; then
    echo -e "${GREEN}=== Uninstalling Calc ===${NC}"

    # Remove files
    if [ -f "$PREFIX/bin/calc_cli" ]; then
        rm -f "$PREFIX/bin/calc_cli"
        echo -e "Removed: ${YELLOW}$PREFIX/bin/calc_cli${NC}"
    fi

    if [ -L "$PREFIX/bin/calc" ]; then
        rm -f "$PREFIX/bin/calc"
        echo -e "Removed: ${YELLOW}$PREFIX/bin/calc${NC} (symlink)"
    fi

    if [ -d "$PREFIX/include/calc" ]; then
        rm -rf "$PREFIX/include/calc"
        echo -e "Removed: ${YELLOW}$PREFIX/include/calc/${NC}"
    fi

    if [ -d "$PREFIX/share/doc/calc" ]; then
        rm -rf "$PREFIX/share/doc/calc"
        echo -e "Removed: ${YELLOW}$PREFIX/share/doc/calc/${NC}"
    fi

    if [ -d "$PREFIX/share/licenses/calc" ]; then
        rm -rf "$PREFIX/share/licenses/calc"
        echo -e "Removed: ${YELLOW}$PREFIX/share/licenses/calc/${NC}"
    fi

    echo ""
    echo -e "${GREEN}=== Uninstall Complete ===${NC}"
    exit 0
fi

# Install
echo -e "${GREEN}=== Installing Calc ===${NC}"

# Find binary
if [ -f "$PROJECT_ROOT/build/Release/calc_cli" ]; then
    BINARY="$PROJECT_ROOT/build/Release/calc_cli"
elif [ -f "$PROJECT_ROOT/build/calc_cli" ]; then
    BINARY="$PROJECT_ROOT/build/calc_cli"
else
    echo -e "${RED}Error: Binary not found. Please build the project first.${NC}"
    echo "Run: $SCRIPT_DIR/build.sh"
    exit 1
fi

# Create directories
mkdir -p "$PREFIX/bin"
mkdir -p "$PREFIX/include"
mkdir -p "$PREFIX/share/doc/calc"
mkdir -p "$PREFIX/share/licenses/calc"

# Install binary
echo -e "${YELLOW}Installing binary...${NC}"
install -m 755 "$BINARY" "$PREFIX/bin/calc_cli"
echo -e "Installed: ${GREEN}$PREFIX/bin/calc_cli${NC}"

# Create symlink
if [ "$SKIP_SYMLINK" = false ]; then
    if [ -L "$PREFIX/bin/calc" ] || [ -e "$PREFIX/bin/calc" ]; then
        rm -f "$PREFIX/bin/calc"
    fi
    ln -sf calc_cli "$PREFIX/bin/calc"
    echo -e "Created: ${GREEN}$PREFIX/bin/calc${NC} (symlink to calc_cli)"
fi

# Install headers
echo -e "${YELLOW}Installing headers...${NC}"
if [ -d "$PROJECT_ROOT/include/calc" ]; then
    cp -r "$PROJECT_ROOT/include/calc" "$PREFIX/include/"
    echo -e "Installed: ${GREEN}$PREFIX/include/calc/${NC}"
fi

# Install documentation
echo -e "${YELLOW}Installing documentation...${NC}"
if [ -f "$PROJECT_ROOT/README.md" ]; then
    install -m 644 "$PROJECT_ROOT/README.md" "$PREFIX/share/doc/calc/"
    echo -e "Installed: ${GREEN}$PREFIX/share/doc/calc/README.md${NC}"
fi
if [ -f "$PROJECT_ROOT/docs/USER_GUIDE.md" ]; then
    install -m 644 "$PROJECT_ROOT/docs/USER_GUIDE.md" "$PREFIX/share/doc/calc/"
    echo -e "Installed: ${GREEN}$PREFIX/share/doc/calc/USER_GUIDE.md${NC}"
fi
if [ -f "$PROJECT_ROOT/docs/API.md" ]; then
    install -m 644 "$PROJECT_ROOT/docs/API.md" "$PREFIX/share/doc/calc/"
    echo -e "Installed: ${GREEN}$PREFIX/share/doc/calc/API.md${NC}"
fi

# Install license
echo -e "${YELLOW}Installing license...${NC}"
if [ -f "$PROJECT_ROOT/LICENSE" ]; then
    install -m 644 "$PROJECT_ROOT/LICENSE" "$PREFIX/share/licenses/calc/"
    echo -e "Installed: ${GREEN}$PREFIX/share/licenses/calc/LICENSE${NC}"
fi

# Summary
echo ""
echo -e "${GREEN}=== Installation Complete ===${NC}"
echo ""
echo -e "Calc has been installed to: ${BLUE}$PREFIX${NC}"
echo ""
echo "Files installed:"
echo -e "  ${YELLOW}$PREFIX/bin/calc_cli${NC} (executable)"
echo -e "  ${YELLOW}$PREFIX/include/calc/${NC} (headers)"
echo -e "  ${YELLOW}$PREFIX/share/doc/calc/${NC} (documentation)"
echo -e "  ${YELLOW}$PREFIX/share/licenses/calc/LICENSE${NC} (license)"
echo ""
echo "To run calc:"
echo -e "  ${GREEN}calc_cli${NC}  or  ${GREEN}calc${NC}"
echo ""
echo "To uninstall:"
echo -e "  ${GREEN}sudo $(basename "$0") --uninstall --prefix $PREFIX${NC}"
echo ""

# Verify installation
if command -v calc_cli &> /dev/null; then
    echo -e "${GREEN}Verifying installation...${NC}"
    calc_cli --version
fi
