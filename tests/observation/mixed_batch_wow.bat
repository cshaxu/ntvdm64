@echo off
echo WOWBEGIN:@CASE@>@TRACE@
@RUN16@ Z:\system32\WINMINE.EXE
if errorlevel 1 goto fail
echo WOWSTARTED:@CASE@>>@TRACE@
@N64@ @TRACE@ FINAL:@CASE@ 19
goto end
:fail
echo FAILED:@CASE@>>@TRACE@
:end
