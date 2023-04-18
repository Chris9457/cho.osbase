@echo off

where /q clang-format.exe
if ERRORLEVEL 1 (
    echo ERROR: clang-format was not found in the PATH
    echo Check for the one installed by Visual Studio Code C++ extension in "%USERPROFILE%\.vscode\extensions"
    pause
    exit /b 1
)

for %%d in (src tests) do call :Format %~dp0../%%d
goto End

:Format
set CODE_DIR=%1
for /r "%CODE_DIR%" %%f in ("*.cpp" "*.h" "*.inl") do (
	echo - %%f
	clang-format.exe -i %%f || pause
)
goto :eof

:End