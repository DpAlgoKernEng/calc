@echo off
REM build.bat - Build script for calc project (Windows)

setlocal enabledelayedexpansion

REM Default values
set BUILD_TYPE=Release
set BUILD_DIR=%CD%\build
set JOBS=%NUMBER_OF_PROCESSORS%
set CLEAN_BUILD=0
set RUN_TESTS=0
set VERBOSE=0

REM Get script directory
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..

REM Print usage
:print_usage
echo Usage: %~nx0 [OPTIONS]
echo.
echo Build the calc project.
echo.
echo OPTIONS:
echo     -t, --type TYPE        Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)
echo     -d, --dir DIR          Build directory (default: .\build)
echo     -j, --jobs N           Number of parallel jobs (default: auto-detect)
echo     -c, --clean            Clean build directory before building
echo     --test                 Run tests after building
echo     -v, --verbose           Verbose output
echo     -h, --help             Show this help message
echo.
echo EXAMPLES:
echo     %~nx0                      Release build
echo     %~nx0 --type Debug         Debug build
echo     %~nx0 --clean --test       Clean build and run tests
echo.
goto :eof

REM Parse arguments
:parse_args
if "%~1"=="" goto :build
if /i "%~1"=="-t" goto :set_type
if /i "%~1"=="--type" goto :set_type
if /i "%~1"=="-d" goto :set_dir
if /i "%~1"=="--dir" goto :set_dir
if /i "%~1"=="-j" goto :set_jobs
if /i "%~1"=="--jobs" goto :set_jobs
if /i "%~1"=="-c" goto :set_clean
if /i "%~1"=="--clean" goto :set_clean
if /i "%~1"=="--test" goto :set_test
if /i "%~1"=="-v" goto :set_verbose
if /i "%~1"=="--verbose" goto :set_verbose
if /i "%~1"=="-h" goto :help
if /i "%~1"=="--help" goto :help

echo Error: Unknown option: %~1
call :print_usage
exit /b 1

:set_type
set BUILD_TYPE=%~2
shift
shift
goto :parse_args

:set_dir
set BUILD_DIR=%~2
shift
shift
goto :parse_args

:set_jobs
set JOBS=%~2
shift
shift
goto :parse_args

:set_clean
set CLEAN_BUILD=1
shift
goto :parse_args

:set_test
set RUN_TESTS=1
shift
goto :parse_args

:set_verbose
set VERBOSE=1
shift
goto :parse_args

:help
call :print_usage
exit /b 0

:build
REM Print build configuration
echo === Calc Build Configuration ===
echo Build Type:    %BUILD_TYPE%
echo Build Directory: %BUILD_DIR%
echo Parallel Jobs:  %JOBS%
echo Clean Build:    %CLEAN_BUILD%
echo Run Tests:      %RUN_TESTS%
echo.

REM Clean build directory if requested
if %CLEAN_BUILD%==1 (
    echo Cleaning build directory...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM Configure with CMake
echo === Configuring with CMake ===
set CMAKE_ARGS=-DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

if %VERBOSE%==1 (
    set CMAKE_ARGS=!CMAKE_ARGS! -DCMAKE_VERBOSE_MAKEFILE=ON
)

echo CMake arguments: %CMAKE_ARGS%
cmake "%PROJECT_ROOT%" %CMAKE_ARGS%
if errorlevel 1 (
    echo CMake configuration failed!
    exit /b 1
)

REM Build
echo.
echo === Building ===
set BUILD_ARGS=--config %BUILD_TYPE% --parallel %JOBS%

if %VERBOSE%==1 (
    set BUILD_ARGS=!BUILD_ARGS! --verbose
)

cmake --build . %BUILD_ARGS%
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

REM Run tests if requested
if %RUN_TESTS%==1 (
    echo.
    echo === Running Tests ===
    ctest --output-on-failure
    if errorlevel 1 (
        echo Tests failed!
        exit /b 1
    )
)

echo.
echo === Build Complete ===
echo Binary location: %BUILD_DIR%\%BUILD_TYPE%\calc_cli.exe

endlocal
