@echo off
echo Building 32-bit version...

REM Compile source files
g++ -o bin\main.exe src\main.cpp src\inout.cpp src\quadtree.cpp src\errorcounter.cpp -Ilibrary\include -Llibrary\x32 -lfreeimage

REM Copy DLL to bin
copy /Y library\x32\FreeImage.dll bin\

echo Build selesai! File output: bin\main.exe
pause
