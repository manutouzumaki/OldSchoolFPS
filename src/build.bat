@echo off

if not exist ..\build mkdir ..\build

pushd ..\build
    cl -O2 ..\src\*.cpp /Zi /link User32.lib Gdi32.lib Winmm.lib
popd
