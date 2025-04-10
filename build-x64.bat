@echo off
echo Building 64-bit version...

REM Compile source files
g++ -o bin\main.exe src\main.cpp src\inout.cpp src\quadtree.cpp src\errorcounter.cpp src\validation.cpp -Ilibrary\include -Llibrary\x64 -lfreeimage

REM Copy DLL to bin
copy /Y library\x64\FreeImage.dll bin\

echo Build selesai! File output: bin\main.exe
pause
