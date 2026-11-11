@echo off
tests\D38T16.COM
if errorlevel 1 goto fail
echo S38_TSR_PARENT_RETURN_OK
tests\D38I.COM
if errorlevel 1 goto fail
tests\D38I.COM
if errorlevel 1 goto fail
echo S38_TSR_FOLLOWING_DPMI_CLIENTS_OK
tests\D38R.COM
if errorlevel 1 goto fail
goto end
:fail
echo S38_FAIL_TSR_LIFETIME
:end
