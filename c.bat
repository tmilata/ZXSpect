@echo off
rem Compilation...

set TARGET=ZXSpec
set GRPDIR=ZXSpec

if "%1"=="" goto default
..\..\..\_c1.bat %1

:default
rem ..\..\..\_c1.bat picopad08
..\..\..\_c1.bat picopad10
