@echo off
REM package.bat - Package calc for distribution (Windows)

setlocal enabledelayedexpansion

REM Get script directory
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..

REM Default values
set VERSION=1.0.0
set DIST_DIR=%PROJECT_ROOT%\dist
set BUILD_DIR=%PROJECT_ROOT%\build
set PACKAGE_NAME=calc
set OS_NAME=windows
set REBUILD=0
set KEEP_BUILD=0

REM Read version from CMakeLists.txt if not specified
for /f "tokens=2 delims= " %%i in ('findstr "^project" "%PROJECT_ROOT%\CMakeLists.txt" 2^>nul ^| findstr "VERSION"') do (
    for /f "tokens=2 delims=)" %%j in ("%%i") do (
        set VERSION=%%j
    )
)

REM Print usage
:print_usage
echo Usage: %~nx0 [OPTIONS]
echo.
echo Package the calc project for distribution.
echo.
echo OPTIONS:
echo     -v, --version VERSION  Version number (default: from CMakeLists.txt)
echo     -d, --dir DIR         Distribution directory (default: .\dist)
echo     -b, --build           Rebuild before packaging
echo     -k, --keep-build      Keep build directory after packaging
echo     -h, --help            Show this help message
echo.
echo SUPPORTED FORMATS:
echo     Windows: zip (requires 7-Zip or PowerShell Compress-Archive)
echo.
echo EXAMPLES:
echo     %~nx0                      Package with version from CMakeLists.txt
echo     %~nx0 --version 1.0.0      Package with custom version
echo     %~nx0 --build              Rebuild and package
echo.
goto :eof

REM Parse arguments
:parse_args
if "%~1"=="" goto :package
if /i "%~1"=="-v" goto :set_version
if /i "%~1"=="--version" goto :set_version
if /i "%~1"=="-d" goto :set_dir
if /i "%~1"=="--dir" goto :set_dir
if /i "%~1"=="-b" goto :set_build
if /i "%~1"=="--build" goto :set_build
if /i "%~1"=="-k" goto :set_keep
if /i "%~1"=="--keep-build" goto :set_keep
if /i "%~1"=="-h" goto :help
if /i "%~1"=="--help" goto :help

echo Error: Unknown option: %~1
call :print_usage
exit /b 1

:set_version
set VERSION=%~2
shift
shift
goto :parse_args

:set_dir
set DIST_DIR=%~2
shift
shift
goto :parse_args

:set_build
set REBUILD=1
shift
goto :parse_args

:set_keep
set KEEP_BUILD=1
shift
goto :parse_args

:help
call :print_usage
exit /b 0

:package
REM Print packaging configuration
echo === Calc Packaging Configuration ===
echo Version:        %VERSION%
echo Package Name:   %PACKAGE_NAME%
echo Distribution:   %OS_NAME%
echo Output Dir:     %DIST_DIR%
echo Rebuild:        %REBUILD%
echo.

REM Build if requested
if %REBUILD%==1 (
    echo === Building Project ===
    call "%SCRIPT_DIR%\build.bat" --type Release
    if errorlevel 1 exit /b 1
)

REM Check if build exists
set BINARY=%BUILD_DIR%\Release\calc_cli.exe
if not exist "%BINARY%" (
    echo Warning: Release build not found, attempting to build...
    call "%SCRIPT_DIR%\build.bat" --type Release
    if errorlevel 1 (
        echo Error: Failed to build project
        exit /b 1
    )
)

REM Create distribution directory
echo === Creating Distribution ===
if not exist "%DIST_DIR%" mkdir "%DIST_DIR%"

REM Create package directory
set PKG_DIR=%DIST_DIR%\%PACKAGE_NAME%-%VERSION%-%OS_NAME%
set PKG_INSTALL_DIR=%PKG_DIR%\%PACKAGE_NAME%-%VERSION%

if exist "%PKG_DIR%" rmdir /s /q "%PKG_DIR%"
mkdir "%PKG_INSTALL_DIR%"
mkdir "%PKG_INSTALL_DIR%\bin"
mkdir "%PKG_INSTALL_DIR%\lib"
mkdir "%PKG_INSTALL_DIR%\include"
mkdir "%PKG_INSTALL_DIR%\share\doc\calc"
mkdir "%PKG_INSTALL_DIR%\share\licenses\calc"

REM Copy files
echo Copying files...
copy "%BINARY%" "%PKG_INSTALL_DIR%\bin\" >nul
xcopy "%PROJECT_ROOT%\include\calc" "%PKG_INSTALL_DIR%\include\calc\" /E /I /Q >nul
copy "%PROJECT_ROOT%\LICENSE" "%PKG_INSTALL_DIR%\share\licenses\calc\" >nul
copy "%PROJECT_ROOT%\README.md" "%PKG_INSTALL_DIR%\share\doc\calc\" >nul
if exist "%PROJECT_ROOT%\docs\USER_GUIDE.md" (
    copy "%PROJECT_ROOT%\docs\USER_GUIDE.md" "%PKG_INSTALL_DIR%\share\doc\calc\" >nul
)

REM Create ZIP package
echo === Creating ZIP Package ===
set ZIP_FILE=%DIST_DIR%\%PACKAGE_NAME%-%VERSION%-%OS_NAME%.zip

REM Check for 7-Zip
where 7z >nul 2>&1
if %errorlevel%==0 (
    7z a -tzip "%ZIP_FILE%" "%PKG_INSTALL_DIR%" >nul
) else (
    REM Use PowerShell Compress-Archive
    powershell -Command "Compress-Archive -Path '%PKG_INSTALL_DIR%' -DestinationPath '%ZIP_FILE%' -Force"
)

if errorlevel 1 (
    echo Error: Failed to create ZIP package
    exit /b 1
)

echo ZIP package created

REM Generate sha256 checksum (requires CertUtil)
echo === Generating Checksums ===
where certutil >nul 2>&1
if %errorlevel%==0 (
    cd /d "%DIST_DIR%"
    for %%f in (*.zip) do (
        certutil -hashfile "%%f" SHA256 > "%%f.sha256"
    )
    cd /d "%PROJECT_ROOT%"
) else (
    echo Warning: certutil not found, skipping checksum
)

REM Summary
echo.
echo === Packaging Complete ===
echo Distribution directory: %DIST_DIR%
echo.

REM List created packages
echo Created packages:
dir /b "%DIST_DIR%\*.zip" 2>nul
dir /b "%DIST_DIR%\*.sha256" 2>nul

REM Cleanup
if %KEEP_BUILD%==0 (
    echo Cleaning package directory...
    rmdir /s /q "%PKG_DIR%"
)

endlocal
