@echo off
REM Batch file to convert a .pw file to .txt using PWDatConverter.exe

REM Usage: convert_pw_to_txt.bat input.pw

set INPUT=%1

if "%INPUT%"=="" (
    echo Usage: %0 input.pw
    exit /b 1
)

PWDatConverter.exe I %INPUT%

if errorlevel 1 (
    echo Conversion failed.
    exit /b %errorlevel%
) else (
    echo Conversion successful.
)