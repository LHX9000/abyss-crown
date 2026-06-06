@echo off
setlocal

set "PROJECT_DIR=%~dp0.."
set "TOOLS_DIR=%PROJECT_DIR%\.tools"
set "VCPKG_DIR=%TOOLS_DIR%\vcpkg"

where git >nul 2>nul
if errorlevel 1 (
    echo [ERROR] Git is not installed or not available in PATH.
    exit /b 1
)

if not exist "%TOOLS_DIR%" mkdir "%TOOLS_DIR%"

if not exist "%VCPKG_DIR%\.git" (
    echo [INFO] Cloning vcpkg...
    git clone https://github.com/microsoft/vcpkg.git "%VCPKG_DIR%"
    if errorlevel 1 exit /b 1
)

echo [INFO] Bootstrapping vcpkg...
call "%VCPKG_DIR%\bootstrap-vcpkg.bat" -disableMetrics
if errorlevel 1 exit /b 1

setx VCPKG_ROOT "%VCPKG_DIR%" >nul

echo.
echo [OK] vcpkg is ready at:
echo      %VCPKG_DIR%
echo.
echo Restart CLion, open the repository root, and select:
echo   CMake preset: Windows MinGW Debug ^(vcpkg^)
echo   Toolchain:    MinGW
echo.
echo CLion will use vcpkg.json to install SDL2 and SDL2_ttf.
endlocal
