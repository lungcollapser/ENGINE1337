@echo off
mkdir ..\build
pushd ..\build
cl -Zi ..\code\engine.cpp opengl32.lib 
popd
