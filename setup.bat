@echo off
:: Script for building the project
set BATCH_DIR=%~dp0
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" -B ./build/ && ^
make -j --no-print-directory -C %BATCH_DIR%build
