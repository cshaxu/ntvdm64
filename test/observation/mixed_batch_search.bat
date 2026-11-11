@echo off
echo SEARCHBEGIN:@CASE@>@TRACE@
set PATH=Z:\tests\S7;Z:\system32;%PATH%
cd @DIR@
DUAL
if errorlevel 8 goto fail
if not errorlevel 7 goto fail
echo COMFIRST:@CASE@>>@TRACE@
PICK @TRACE@ CWD64:@CASE@ 0
@RUN16@ PICK @TRACE@ LAUNCH64:@CASE@ 0
call FIND
echo BAREBAT:@CASE@>>@TRACE@
@RUN16@ @PIF@
if errorlevel 8 goto fail
if not errorlevel 7 goto fail
echo PIF7:@CASE@>>@TRACE@
cd Z:\system32
PICK @TRACE@ PATH32:@CASE@ 0
@RUN16@ Z:\tests\S7\NOMISS.EXE
if errorlevel 2 goto fail
if not errorlevel 1 goto fail
echo SEARCHMISSING:@CASE@>>@TRACE@
@N64@ @TRACE@ FINAL:@CASE@ 19
goto end
:fail
echo FAILED:@CASE@>>@TRACE@
:end
