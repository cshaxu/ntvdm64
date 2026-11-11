@echo off
tests\D38I.COM
if errorlevel 1 goto fail
tests\D38I.COM
if errorlevel 1 goto fail
tests\D38I.COM
if errorlevel 1 goto fail
tests\D38I.COM
if errorlevel 1 goto fail
tests\D38I.COM
if errorlevel 1 goto fail
echo S38_PM_FIVE_CLIENTS_OK
goto end
:fail
echo S38_FAIL_PM_CLIENT_LIFETIME
:end
