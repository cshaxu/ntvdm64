@echo off
system32\REDIR.EXE
if errorlevel 1 goto fail
tests\D38N16.COM
if errorlevel 1 goto fail
echo S38_NETWORK_SPOOL_REDIR_LOADED_OK
goto end
:fail
echo S38_FAIL_NETWORK_SPOOL_REDIR
:end
