@echo off
echo CHILD:@CASE@>>@TRACE@
@N64@ @TRACE@ CHILD64:@CASE@ 23
if errorlevel 24 goto fail
if not errorlevel 23 goto fail
echo CHILD64RC:@CASE@>>@TRACE@
@DOS@
if errorlevel 8 goto fail
if not errorlevel 7 goto fail
echo CHILDDOS7:@CASE@>>@TRACE@
@N32@ @TRACE@ CHILD32:@CASE@ 0
@N64@ @TRACE@ RETURN64:@CASE@ 0
echo CHILDEND:@CASE@>>@TRACE@
goto end
:fail
echo FAILEDCHILD:@CASE@>>@TRACE@
:end
