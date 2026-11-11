@echo off
echo BEGIN:@CASE@>@TRACE@
@DOS@
if errorlevel 8 goto fail
if not errorlevel 7 goto fail
echo DOS7:@CASE@>>@TRACE@
@N32@ @TRACE@ N32:@CASE@ 37 "two words" >@N32OUT@ 2>@N32ERR@
if errorlevel 38 goto fail
if not errorlevel 37 goto fail
echo RC32:@CASE@>>@TRACE@
call @CHILD@
echo CALLRETURN:@CASE@>>@TRACE@
@DOS@
if errorlevel 8 goto fail
if not errorlevel 7 goto fail
echo DOSAGAIN:@CASE@>>@TRACE@
@N64@ @TRACE@ N64:@CASE@ 19
if errorlevel 20 goto fail
if not errorlevel 19 goto fail
echo RC64:@CASE@>>@TRACE@
echo DONE:@CASE@>>@TRACE@
goto end
:fail
echo FAILED:@CASE@>>@TRACE@
:end
