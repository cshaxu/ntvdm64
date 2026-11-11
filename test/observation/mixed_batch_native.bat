@echo off
echo ADVBEGIN:@CASE@>@TRACE@
echo S7-INPUT>@INPUT@
@N32@ @TRACE@ IN32:@CASE@ stdin <@INPUT@ >@N32OUT@ 2>@N32ERR@
if errorlevel 1 goto fail
echo STDIN32:@CASE@>>@TRACE@
@N64@ @TRACE@ IN64:@CASE@ stdin <@INPUT@
if errorlevel 1 goto fail
echo STDIN64:@CASE@>>@TRACE@
start "" /wait @GUI32@ @TRACE@ GUIWAIT32:@CASE@ 37
if errorlevel 38 goto fail
if not errorlevel 37 goto fail
echo GUIWAITED32:@CASE@>>@TRACE@
start "" /wait @GUI64@ @TRACE@ GUIWAIT64:@CASE@ 37
if errorlevel 38 goto fail
if not errorlevel 37 goto fail
echo GUIWAITED64:@CASE@>>@TRACE@
start "" @GUI32@ @TRACE@ GUISTART:@CASE@ hold @EVENT@
@N64@ @TRACE@ RELEASE:@CASE@ release @EVENT@
if errorlevel 1 goto fail
echo STARTRETURN:@CASE@>>@TRACE@
@RUN16@ @GUI64@ @TRACE@ GUIROOT:@CASE@ hold @EVENT2@
@N32@ @TRACE@ RELEASE2:@CASE@ release @EVENT2@
if errorlevel 1 goto fail
echo GUIROOTRETURN:@CASE@>>@TRACE@
@RUN16@ --wait @GUI32@ @TRACE@ GUIEXPLICIT:@CASE@ 37
if errorlevel 38 goto fail
if not errorlevel 37 goto fail
echo GUIEXPLICIT37:@CASE@>>@TRACE@
@RUN16@ @MISSING@
if errorlevel 2 goto fail
if not errorlevel 1 goto fail
echo MISSINGFAILED:@CASE@>>@TRACE@
@N64@ @TRACE@ FINAL:@CASE@ 19
goto end
:fail
echo FAILED:@CASE@>>@TRACE@
:end
