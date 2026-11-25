@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "witness=%~dp0PATCH"
ver >"%witness%\SCHED.RUN" || exit /b 1
:wait
if not exist "%witness%\SCHED.GO" goto wait
ver >"%witness%\EXEC.TXT.tmp" || exit /b 1
move /y "%witness%\EXEC.TXT.tmp" "%witness%\EXEC.TXT" >nul || exit /b 1
echo done>"%witness%\SCHED.END"
exit /b %errorlevel%
