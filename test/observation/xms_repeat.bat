@echo off
tests\X35.COM
if errorlevel 1 goto FAILED
tests\X35.COM
if errorlevel 1 goto FAILED
echo S35_XMS_REPEAT_OK
goto END
:FAILED
echo S35_XMS_REPEAT_FAIL
:END
