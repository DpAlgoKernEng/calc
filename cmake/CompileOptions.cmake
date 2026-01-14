# Compiler warning options for calc project

# MSVC specific options
if(MSVC)
    # Enable high warning level
    add_compile_options(/W4 /permissive-)

    # Treat warnings as errors (can be commented out during development)
    # add_compile_options(/WX)

    # Disable some specific warnings that are too noisy
    # C4996: Deprecated functions
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

    # Enable parallel compilation
    add_compile_options(/MP)
else()
    # GCC/Clang options
    add_compile_options(-Wall -Wextra -Wpedantic)

    # Additional useful warnings (common to GCC and Clang)
    add_compile_options(
        -Wshadow
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
    )

    # GCC-specific warnings (only available in GCC)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(
            -Wmisleading-indentation
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wuseless-cast
        )
    endif()

    # Treat warnings as errors (can be commented out during development)
    # add_compile_options(-Werror)

    # Debug options
    add_compile_options(-g)

    # Add address sanitizer (can be enabled with -DENABLE_ASAN=ON)
    if(ENABLE_ASAN)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()

    # Add thread sanitizer (can be enabled with -DENABLE_TSAN=ON)
    if(ENABLE_TSAN)
        add_compile_options(-fsanitize=thread)
        add_link_options(-fsanitize=thread)
    endif()

    # Add undefined behavior sanitizer (can be enabled with -DENABLE_UBSAN=ON)
    if(ENABLE_UBSAN)
        add_compile_options(-fsanitize=undefined -fno-sanitize-recover=all)
        add_link_options(-fsanitize=undefined -fno-sanitize-recover=all)
    endif()
endif()

# Position Independent Code (required for shared libraries)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
