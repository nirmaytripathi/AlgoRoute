@echo off
echo ======================================================
echo  Building AlgoRoute Qt 6 C++ Desktop Application...
echo ======================================================

set PATH=C:/Qt/6.11.1/mingw_64/bin;C:/Qt/Tools/mingw1310_64/bin;%PATH%

cd /d D:\AlgoRoute\src

echo Running qmake...
qmake.exe -spec win32-g++ AlgoRoute.pro

echo Compiling C++ Qt executable...
mingw32-make.exe -j4

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ======================================================
    echo  Qt Application Build SUCCESSFUL!
    echo  Executable: D:\AlgoRoute\AlgoRoute.exe
    echo ======================================================
) else (
    echo.
    echo Build FAILED! Please check error output.
)
