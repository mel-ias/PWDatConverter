@echo off
REM Batch file to convert a point cloud with RGB in txt format to pw format

REM Usage: convert_to_pw.bat input.txt
REM Output will be input.pw

set INPUT=%1

if "%INPUT%"=="" (
    echo Usage: %0 input.txt
    exit /b 1
)

PWDatConverter.exe I %INPUT% xyzRGB

if errorlevel 1 (
    echo Conversion failed.
    exit /b %errorlevel%
) else (
    echo Conversion successful.
)