@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "PRESET=%~1"
if not defined PRESET call :choose_preset
if errorlevel 1 exit /b 1

if /I not "%PRESET%"=="debug" if /I not "%PRESET%"=="release" (
    echo Unsupported preset: %PRESET%
    echo Use: build.bat [release^|debug] [sfml-version]
    pause
    exit /b 1
)

set "VERSION=%~2"
if not defined VERSION set "VERSION=3.0.2"

set "REPO_ROOT=%~dp0"
if "%REPO_ROOT:~-1%"=="\" set "REPO_ROOT=%REPO_ROOT:~0,-1%"

set "DEPS_ROOT=%REPO_ROOT%\.deps"
set "SFML_DIR=%DEPS_ROOT%\SFML-%VERSION%"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

for %%T in (git cmake ninja) do (
    where %%T >nul 2>nul
    if errorlevel 1 (
        echo Missing required tool: %%T
        pause
        exit /b 1
    )
)

call :ensure_compiler
if errorlevel 1 exit /b 1

if not exist "%SFML_DIR%\CMakeLists.txt" (
    if not exist "%DEPS_ROOT%" mkdir "%DEPS_ROOT%"
    git clone --branch "%VERSION%" --depth 1 https://github.com/SFML/SFML.git "%SFML_DIR%"
    if errorlevel 1 (
        pause
        exit /b 1
    )
)

pushd "%REPO_ROOT%"
cmake --preset "%PRESET%"
if errorlevel 1 (
    popd
    pause
    exit /b 1
)

cmake --build --preset "%PRESET%"
if errorlevel 1 (
    popd
    pause
    exit /b 1
)

popd
pause
exit /b 0

:choose_preset
echo Select build preset:
echo   1. release
echo   2. debug
choice /c 12 /n /m "Choose [1/2]: "
if errorlevel 2 (
    set "PRESET=debug"
) else (
    set "PRESET=release"
)
echo Selected preset: !PRESET!
exit /b 0

:ensure_compiler
where cl >nul 2>nul
if not errorlevel 1 (
    echo Using compiler already available in PATH.
    exit /b 0
)

where clang++ >nul 2>nul
if not errorlevel 1 (
    echo Using clang++ already available in PATH.
    exit /b 0
)

where g++ >nul 2>nul
if not errorlevel 1 (
    echo Using g++ already available in PATH.
    exit /b 0
)

if not exist "!VSWHERE!" (
    echo No C++ compiler found in PATH.
    echo Also could not find vswhere at:
    echo   !VSWHERE!
    pause
    exit /b 1
)

set "VSINSTALL="
for /f "usebackq delims=" %%I in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"

if not defined VSINSTALL (
    echo No Visual Studio installation with C++ tools was found.
    echo Install the Desktop development with C++ workload or start from a shell where a compiler is already in PATH.
    pause
    exit /b 1
)

if exist "!VSINSTALL!\Common7\Tools\VsDevCmd.bat" (
    echo Bootstrapping MSVC toolchain from:
    echo   !VSINSTALL!
    call "!VSINSTALL!\Common7\Tools\VsDevCmd.bat" -host_arch=x64 -arch=x64
) else if exist "!VSINSTALL!\VC\Auxiliary\Build\vcvars64.bat" (
    echo Bootstrapping MSVC toolchain from:
    echo   !VSINSTALL!
    call "!VSINSTALL!\VC\Auxiliary\Build\vcvars64.bat"
) else (
    echo Visual Studio was found, but neither VsDevCmd.bat nor vcvars64.bat exists there:
    echo   !VSINSTALL!
    pause
    exit /b 1
)

where cl >nul 2>nul
if not errorlevel 1 (
    echo MSVC environment is ready.
    exit /b 0
)

echo MSVC environment bootstrap finished, but cl.exe is still not available.
echo Start the script from a working developer shell or fix the Visual Studio C++ workload.
pause
exit /b 1
