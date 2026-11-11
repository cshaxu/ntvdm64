@echo off
run16 tests\S2EXIT7.COM OK
if errorlevel 8 goto fail
if not errorlevel 7 goto fail
echo S2-DOS-EXIT-7-RETAINED
run16 tests\S2EXIT7.COM
if errorlevel 100 goto fail
if not errorlevel 99 goto fail
echo S2-DOS-EXIT-99-RETAINED
run16 mem
echo S2-DOS-AFTER-NONZERO
goto end
:fail
echo S2-DOS-NONZERO-FAILED
:end
