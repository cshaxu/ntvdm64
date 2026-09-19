@echo off
setlocal EnableExtensions
cd /d O:\winnt || exit /b 64
set "RUN=O:\winnt\run16.exe COMMAND.COM /c"
set "MARK=S34_NATIVE_CHILD_STDIN=S34_NATIVE_CHILD_STDOUT_OK"

rem Host-provided stdout and stdin stay outside the guest parser.
%RUN% tests\S34N.EXE | findstr /c:"S34_NATIVE_CHILD_STDOUT_OK" >nul || exit /b 1
del /q tests\HF.OUT 2>nul
%RUN% tests\S34N.EXE > tests\HF.OUT || exit /b 1
findstr /c:"S34_NATIVE_CHILD_STDOUT_OK" tests\HF.OUT >nul || exit /b 1
echo HOST-PIPE-IN|%RUN% tests\S34E.EXE | findstr /c:"S34_NATIVE_CHILD_STDIN=HOST-PIPE-IN" >nul || exit /b 1

rem Guest COMMAND owns >, >>, < and |.  Guest paths are legal 8.3 names;
rem checking a long host spelling here would test DOS filename canonicalization,
rem not the stream lifecycle.
del /q tests\A1.OUT tests\I1.OUT tests\G1.OUT tests\G2.OUT tests\G3.OUT 2>nul
> tests\IN1.TXT echo PIPE-IN-OK
%RUN% "tests\S34N.EXE > tests\A1.OUT" || exit /b 1
%RUN% "tests\S34N.EXE >> tests\A1.OUT" || exit /b 1
for /f %%C in ('findstr /c:"S34_NATIVE_CHILD_STDOUT_OK" tests\A1.OUT ^| find /c /v ""') do if not "%%C"=="2" exit /b 1
%RUN% "tests\S34E.EXE < tests\IN1.TXT > tests\I1.OUT" || exit /b 1
findstr /c:"S34_NATIVE_CHILD_STDIN=PIPE-IN-OK" tests\I1.OUT >nul || exit /b 1
%RUN% "tests\S34N.EXE | tests\S34E.EXE > tests\G1.OUT" || exit /b 1
findstr /c:"%MARK%" tests\G1.OUT >nul || exit /b 1
%RUN% "COMMAND.COM /c tests\S34N.EXE | tests\S34E.EXE > tests\G2.OUT" || exit /b 1
findstr /c:"%MARK%" tests\G2.OUT >nul || exit /b 1
%RUN% "COMMAND.COM /c COMMAND.COM /c tests\S34N.EXE | tests\S34E.EXE > tests\G3.OUT" || exit /b 1
findstr /c:"%MARK%" tests\G3.OUT >nul || exit /b 1
exit /b 0
