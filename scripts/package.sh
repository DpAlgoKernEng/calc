#!/bin/bash
# package.sh - Package calc for distribution

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
VERSION=$(grep "^project" "$PROJECT_ROOT/CMakeLists.txt" | grep -oP "VERSION \K[0-9.]+")
DIST_DIR="$PROJECT_ROOT/dist"
BUILD_DIR="$PROJECT_ROOT/build"
PACKAGE_NAME="calc"
ARCH=$(uname -m)
OS=$(uname -s | tr '[:upper:]' '[:lower:]')
REBUILD=false
KEEP_BUILD=false

# Detect OS
case "$OS" in
    linux*)
        OS_NAME="linux"
        ;;
    darwin*)
        OS_NAME="macos"
        ;;
    msys*|mingw*)
        OS_NAME="windows"
        ;;
    *)
        echo -e "${RED}Error: Unsupported OS: $OS${NC}"
        exit 1
        ;;
esac

# Print usage
print_usage() {
    cat << EOF
Usage: $(basename "$0") [OPTIONS]

Package the calc project for distribution.

OPTIONS:
    -v, --version VERSION  Version number (default: from CMakeLists.txt)
    -d, --dir DIR         Distribution directory (default: ./dist)
    -b, --build           Rebuild before packaging
    -k, --keep-build      Keep build directory after packaging
    -h, --help            Show this help message

SUPPORTED FORMATS:
    Linux:   tar.gz, deb (requires dpkg-deb)
    macOS:   dmg (requires hdiutil), tar.gz
    Windows: zip (requires zip)

EXAMPLES:
    $(basename "$0")                      # Package with version from CMakeLists.txt
    $(basename "$0") --version 1.0.0      # Package with custom version
    $(basename "$0") --build              # Rebuild and package

EOF
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--version)
            VERSION="$2"
            shift 2
            ;;
        -d|--dir)
            DIST_DIR="$2"
            shift 2
            ;;
        -b|--build)
            REBUILD=true
            shift
            ;;
        -k|--keep-build)
            KEEP_BUILD=true
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

# Print packaging configuration
echo -e "${GREEN}=== Calc Packaging Configuration ===${NC}"
echo -e "Version:        ${YELLOW}$VERSION${NC}"
echo -e "Package Name:   ${YELLOW}$PACKAGE_NAME${NC}"
echo -e "Distribution:   ${YELLOW}$OS_NAME-$ARCH${NC}"
echo -e "Output Dir:     ${YELLOW}$DIST_DIR${NC}"
echo -e "Rebuild:        ${YELLOW}$REBUILD${NC}"
echo ""

# Build if requested
if [ "$REBUILD" = true ]; then
    echo -e "${GREEN}=== Building Project ===${NC}"
    "$SCRIPT_DIR/build.sh" --type Release
fi

# Check if build exists
if [ ! -f "$BUILD_DIR/Release/calc_cli" ]; then
    echo -e "${YELLOW}Warning: Release build not found, attempting to build...${NC}"
    "$SCRIPT_DIR/build.sh" --type Release
fi

BINARY="$BUILD_DIR/Release/calc_cli"
if [ ! -f "$BINARY" ]; then
    echo -e "${RED}Error: Binary not found at $BINARY${NC}"
    exit 1
fi

# Create distribution directory
echo -e "${GREEN}=== Creating Distribution ===${NC}"
mkdir -p "$DIST_DIR"

# Create package directory
PKG_DIR="$DIST_DIR/${PACKAGE_NAME}-${VERSION}-${OS_NAME}-${ARCH}"
PKG_INSTALL_DIR="$PKG_DIR/${PACKAGE_NAME}-${VERSION}"

rm -rf "$PKG_DIR"
mkdir -p "$PKG_INSTALL_DIR/bin"
mkdir -p "$PKG_INSTALL_DIR/lib"
mkdir -p "$PKG_INSTALL_DIR/include"
mkdir -p "$PKG_INSTALL_DIR/share/doc/calc"
mkdir -p "$PKG_INSTALL_DIR/share/licenses/calc"

# Copy files
echo -e "${YELLOW}Copying files...${NC}"
cp "$BINARY" "$PKG_INSTALL_DIR/bin/"
cp -r "$PROJECT_ROOT/include/calc" "$PKG_INSTALL_DIR/include/"
cp "$PROJECT_ROOT/LICENSE" "$PKG_INSTALL_DIR/share/licenses/calc/"
cp "$PROJECT_ROOT/README.md" "$PKG_INSTALL_DIR/share/doc/calc/"
cp "$PROJECT_ROOT/docs/USER_GUIDE.md" "$PKG_INSTALL_DIR/share/doc/calc/" 2>/dev/null || true

# Create tarball
echo -e "${GREEN}=== Creating Tarball ===${NC}"
cd "$DIST_DIR"
TAR_FILE="${PACKAGE_NAME}-${VERSION}-${OS_NAME}-${ARCH}.tar.gz"
tar -czf "$TAR_FILE" "$(basename "$PKG_DIR")"

# Generate sha256 checksum
echo -e "${GREEN}=== Generating Checksums ===${NC}"
if command -v sha256sum &> /dev/null; then
    sha256sum "$TAR_FILE" > "$TAR_FILE.sha256"
elif command -v shasum &> /dev/null; then
    shasum -a 256 "$TAR_FILE" > "$TAR_FILE.sha256"
else
    echo -e "${YELLOW}Warning: sha256sum not found, skipping checksum${NC}"
fi

# Create platform-specific packages
case "$OS_NAME" in
    linux)
        if command -v dpkg-deb &> /dev/null; then
            echo -e "${GREEN}=== Creating DEB Package ===${NC}"
            DEB_DIR="$PKG_DIR/deb"
            mkdir -p "$DEB_DIR/DEBIAN"
            mkdir -p "$DEB_DIR/usr/local/bin"
            mkdir -p "$DEB_DIR/usr/local/include/calc"
            mkdir -p "$DEB_DIR/usr/local/share/doc/calc"
            mkdir -p "$DEB_DIR/usr/local/share/licenses/calc"

            # Control file
            cat > "$DEB_DIR/DEBIAN/control" << EOF
Package: calc-cli
Version: ${VERSION}
Architecture: ${ARCH}
Maintainer: Calc Developers <calc@example.com>
Installed-Size: $(du -s "$PKG_INSTALL_DIR" | awk '{print $1}')
Depends: libc6 (>= 2.17), libstdc++6 (>= 5)
Section: math
Priority: optional
Homepage: https://github.com/yourusername/calc
Description: Cross-platform command-line calculator
 Calc is a powerful command-line calculator supporting three modes:
  - Standard mode for basic arithmetic
  - Scientific mode with mathematical functions
  - Programmer mode for base conversions and bitwise operations
EOF

            # Copy files
            cp "$BINARY" "$DEB_DIR/usr/local/bin/"
            cp -r "$PROJECT_ROOT/include/calc" "$DEB_DIR/usr/local/include/"
            cp "$PROJECT_ROOT/LICENSE" "$DEB_DIR/usr/local/share/licenses/calc/"
            cp "$PROJECT_ROOT/README.md" "$DEB_DIR/usr/local/share/doc/calc/"

            # Build deb
            dpkg-deb --build "$DEB_DIR" "${PACKAGE_NAME}_cli_${VERSION}_${ARCH}.deb"
            echo -e "${GREEN}DEB package created${NC}"
        fi
        ;;
    macos)
        if command -v hdiutil &> /dev/null; then
            echo -e "${GREEN}=== Creating DMG Package ===${NC}"
            DMG_NAME="${PACKAGE_NAME}-${VERSION}-${OS_NAME}-${ARCH}.dmg"

            # Create a temporary DMG
            TEMP_DMG="temp_${DMG_NAME}"
            hdiutil create -size 100m -volname "calc ${VERSION}" -fs HFS+ "$TEMP_DMG"

            # Mount the DMG
            MOUNT_DIR="/tmp/calc_dmg_mount"
            mkdir -p "$MOUNT_DIR"
            hdiutil attach "$TEMP_DMG" -mountpoint "$MOUNT_DIR"

            # Copy files
            cp -R "$PKG_INSTALL_DIR/"* "$MOUNT_DIR/"

            # Unmount and create final DMG
            hdiutil detach "$MOUNT_DIR"
            hdiutil convert "$TEMP_DMG" -format UDZO -o "$DMG_NAME"
            rm -f "$TEMP_DMG"
            rmdir "$MOUNT_DIR" 2>/dev/null || true

            echo -e "${GREEN}DMG package created${NC}"
        fi
        ;;
    windows)
        if command -v zip &> /dev/null; then
            echo -e "${GREEN}=== Creating ZIP Package ===${NC}"
            ZIP_FILE="${PACKAGE_NAME}-${VERSION}-windows-${ARCH}.zip"
            cd "$PKG_DIR"
            zip -r "../$ZIP_FILE" "${PACKAGE_NAME}-${VERSION}"
            cd "$DIST_DIR"
            echo -e "${GREEN}ZIP package created${NC}"
        fi
        ;;
esac

# Summary
echo ""
echo -e "${GREEN}=== Packaging Complete ===${NC}"
echo -e "${BLUE}Distribution directory:${NC} $DIST_DIR"
echo ""

# List created packages
echo -e "${GREEN}Created packages:${NC}"
ls -lh "$DIST_DIR"/*.tar.gz 2>/dev/null || true
ls -lh "$DIST_DIR"/*.deb 2>/dev/null || true
ls -lh "$DIST_DIR"/*.dmg 2>/dev/null || true
ls -lh "$DIST_DIR"/*.zip 2>/dev/null || true

# Cleanup
if [ "$KEEP_BUILD" = false ]; then
    echo -e "${YELLOW}Cleaning package directory...${NC}"
    rm -rf "$PKG_DIR"
fi
