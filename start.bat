@echo off
echo Building the project...
cmake --build build
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)
echo Starting back_end.exe...
start back_end.exe
echo Starting front_end.exe...
start front_end.exe
echo Opening index.html...
start html_front_end/index.html
echo All components started!