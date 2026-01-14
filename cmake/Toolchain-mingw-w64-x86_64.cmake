# MinGW-w64 x86_64 toolchain for Windows cross-compilation from Linux/macOS

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Set compilers
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Set target root for sysroot
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# Set library and include search paths
set(CMAKE_INCLUDE_PATH /usr/x86_64-w64-mingw32/include)
set(CMAKE_LIBRARY_PATH /usr/x86_64-w64-mingw32/lib)

# Don't search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
