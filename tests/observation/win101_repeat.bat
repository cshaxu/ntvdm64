@echo off
cd \WIN101
win.com
if errorlevel 1 exit /b %errorlevel%
\system32\mem.exe
if errorlevel 1 exit /b %errorlevel%
echo MOUSE101-FIRST-RETURNED>\TMP\FIRST.DONE
win.com
if errorlevel 1 exit /b %errorlevel%
echo MOUSE101-WINDOWS-RETURNED
\system32\mem.exe
if errorlevel 1 exit /b %errorlevel%
echo MOUSE101-DOS-RETURN-COMPLETE
