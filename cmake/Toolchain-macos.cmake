# macOS toolchain configuration

set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Set deployment target (adjust as needed)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")

# Use default system compilers (Clang)
# CMAKE_C_COMPILER and CMAKE_CXX_COMPILER will be set by default

# Enable universal binary support (optional)
# set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Architecture types")
