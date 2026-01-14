# Linux toolchain configuration

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Use default system compilers
# CMAKE_C_COMPILER and CMAKE_CXX_COMPILER will be set by default

# Common Linux paths
set(CMAKE_INSTALL_PREFIX /usr/local CACHE PATH "Install path prefix, prepended onto install directories.")
