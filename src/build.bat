@echo off

if not exist ..\build mkdir ..\build

pushd ..\build
    cl -Od ..\src\*.cpp /Zi /link User32.lib Gdi32.lib Winmm.lib
popd
