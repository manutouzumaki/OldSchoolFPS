@echo off

if not exist ..\build mkdir ..\build

pushd ..\build
    cl ..\src\main.cpp /Zi /link User32.lib Gdi32.lib Winmm.lib
popd
