@echo off
rem Actual inner run16 and native target; preserve the immediate child's code.
"%~1" "%ComSpec%" /d /c exit 37
if not "%errorlevel%"=="37" exit /b 98
"%~1" "%ComSpec%" /d /c exit 41 <nul >nul 2>&1
if not "%errorlevel%"=="41" exit /b 97
echo INNER-NATIVE-RETURNED
exit /b 23
