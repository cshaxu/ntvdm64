@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~1"=="" exit /b 64
for %%I in ("%~1") do set "OUT=%%~fI"
if exist "%OUT%" (echo Fresh test output required.& exit /b 2)
mkdir "%OUT%\win31\SYSTEM" "%OUT%\win31\PATCH" "%OUT%\win95\SYSTEM" "%OUT%\win95\PATCH" || exit /b 1
for %%S in (win31 win95) do (
  >"%OUT%\%%S\WIN.COM" echo fixture
  >"%OUT%\%%S\SYSTEM.INI" (
    echo [boot]
    echo mouse.drv=mouse.drv
    echo.
    echo [386Enh]
    echo mouse=*vmd
    echo.
    echo [other]
    echo value=yes
  )
  copy /b "%OUT%\%%S\SYSTEM.INI" "%OUT%\%%S.original" >nul || exit /b 1
)
>"%OUT%\win31\SYSTEM\WIN386.EXE" echo fixture
>"%OUT%\win95\SYSTEM\VMM32.VXD" echo fixture
for %%S in (win31 win95) do call :test "%%S" || exit /b 1
>"%OUT%\win31\SYSTEM.INI" (
  echo [386Enh]
  echo device=*vmd
  echo mouse=*vmd
)
copy /b /y "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.preexisting" >nul || exit /b 1
call tools\win-multitask\APPLY.CMD "%OUT%\win31" || exit /b 1
for /f %%N in ('findstr /i /b /x /c:"device=*vmd" "%OUT%\win31\SYSTEM.INI" ^| find /c /v ""') do if not "%%N"=="1" exit /b 1
call tools\win-multitask\UNAPPLY.CMD "%OUT%\win31" || exit /b 1
fc /b "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.preexisting" >nul || exit /b 1
copy /b /y "%OUT%\win31.original" "%OUT%\win31\SYSTEM.INI" >nul || exit /b 1
>"%OUT%\win31\SYSTEM.INI" (
  echo [386Enh]
  echo   device = *VMD ; existing
  echo mouse=*vmd
)
copy /b /y "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.commented" >nul || exit /b 1
call tools\win-multitask\APPLY.CMD "%OUT%\win31" || exit /b 1
findstr /i /b /x /c:"device=*vmd" "%OUT%\win31\SYSTEM.INI" >nul && exit /b 1
call tools\win-multitask\UNAPPLY.CMD "%OUT%\win31" || exit /b 1
fc /b "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.commented" >nul || exit /b 1
copy /b /y "%OUT%\win31.original" "%OUT%\win31\SYSTEM.INI" >nul || exit /b 1
>"%OUT%\win31\SYSTEM.INI" (
  echo [386Enh]
  echo 	device	=	*VMD	; existing
  echo mouse=*vmd
)
copy /b /y "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.tabs" >nul || exit /b 1
call tools\win-multitask\APPLY.CMD "%OUT%\win31" || exit /b 1
findstr /i /b /x /c:"device=*vmd" "%OUT%\win31\SYSTEM.INI" >nul && exit /b 1
call tools\win-multitask\UNAPPLY.CMD "%OUT%\win31" || exit /b 1
fc /b "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.tabs" >nul || exit /b 1
copy /b /y "%OUT%\win31.original" "%OUT%\win31\SYSTEM.INI" >nul || exit /b 1
>"%OUT%\win31\SYSTEM.INI" (
  echo [other]
  echo device=*vmd
  echo [386Enh]
  echo mouse=*vmd
)
copy /b /y "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.other-section" >nul || exit /b 1
call tools\win-multitask\APPLY.CMD "%OUT%\win31" || exit /b 1
for /f %%N in ('findstr /i /b /x /c:"device=*vmd" "%OUT%\win31\SYSTEM.INI" ^| find /c /v ""') do if not "%%N"=="2" exit /b 1
call tools\win-multitask\UNAPPLY.CMD "%OUT%\win31" || exit /b 1
fc /b "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.other-section" >nul || exit /b 1
copy /b /y "%OUT%\win31.original" "%OUT%\win31\SYSTEM.INI" >nul || exit /b 1
>"%OUT%\win31\SYSTEM.INI" (
  echo [other]
  echo mouse=*vmd
  echo [386Enh]
  echo   mouse = *VMD ; redundant binding
  echo device=*vmd
)
copy /b /y "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.mouse-sections" >nul || exit /b 1
call tools\win-multitask\APPLY.CMD "%OUT%\win31" || exit /b 1
for /f %%N in ('findstr /i /b /x /c:"mouse=*vmd" "%OUT%\win31\SYSTEM.INI" ^| find /c /v ""') do if not "%%N"=="1" exit /b 1
findstr /i /l /c:"redundant binding" "%OUT%\win31\SYSTEM.INI" >nul && exit /b 1
call tools\win-multitask\UNAPPLY.CMD "%OUT%\win31" || exit /b 1
fc /b "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.mouse-sections" >nul || exit /b 1
copy /b /y "%OUT%\win31.original" "%OUT%\win31\SYSTEM.INI" >nul || exit /b 1
>"%OUT%\win31\PATCH\NTVDM64.386" echo owner file
call tools\win-multitask\APPLY.CMD "%OUT%\win31" >nul 2>&1 && exit /b 1
fc /b "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.original" >nul || exit /b 1
del /q "%OUT%\win31\PATCH\NTVDM64.386"
>>"%OUT%\win31\SYSTEM.INI" echo value=hello!world
copy /b /y "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.unsafe" >nul || exit /b 1
call tools\win-multitask\APPLY.CMD "%OUT%\win31" >nul 2>&1 && exit /b 1
fc /b "%OUT%\win31\SYSTEM.INI" "%OUT%\win31.unsafe" >nul || exit /b 1
copy /b /y "%OUT%\win31.original" "%OUT%\win31\SYSTEM.INI" >nul || exit /b 1
call tools\win-multitask\APPLY.CMD "%OUT%\win31" || exit /b 1
>>"%OUT%\win31\PATCH\MULTITASK.INI.BAK" echo changed
call tools\win-multitask\UNAPPLY.CMD "%OUT%\win31" >nul 2>&1 && exit /b 1
fc /b "%OUT%\win31\SYSTEM.INI" "%OUT%\win31\PATCH\MULTITASK.INI.NEW" >nul || exit /b 1
echo PASS both systems: detection, install, duplicate refusal, edit refusal, exact restore
exit /b 0
:test
set "ROOT=%OUT%\%~1"
call tools\win-multitask\APPLY.CMD "%ROOT%" || exit /b 1
if /i "%~1"=="win31" findstr /i /b /x /c:"device=*vmd" "%ROOT%\SYSTEM.INI" >nul || if /i "%~1"=="win31" exit /b 1
if /i "%~1"=="win31" findstr /i /b /x /c:"mouse=*vmd" "%ROOT%\SYSTEM.INI" >nul && if /i "%~1"=="win31" exit /b 1
if /i "%~1"=="win95" findstr /i /b /x /c:"mouse=*vmd" "%ROOT%\SYSTEM.INI" >nul || if /i "%~1"=="win95" exit /b 1
findstr /i /l /c:"device=%ROOT%\PATCH\INSIGNIA.386" "%ROOT%\SYSTEM.INI" >nul || exit /b 1
findstr /i /l /c:"device=%ROOT%\PATCH\NTVDM64.386" "%ROOT%\SYSTEM.INI" >nul || exit /b 1
call tools\win-multitask\APPLY.CMD "%ROOT%" >nul 2>&1 && exit /b 1
>>"%ROOT%\SYSTEM.INI" echo changed=yes
call tools\win-multitask\UNAPPLY.CMD "%ROOT%" >nul 2>&1 && exit /b 1
copy /b /y "%ROOT%\PATCH\MULTITASK.INI.NEW" "%ROOT%\SYSTEM.INI" >nul || exit /b 1
call tools\win-multitask\UNAPPLY.CMD "%ROOT%" || exit /b 1
fc /b "%ROOT%\SYSTEM.INI" "%OUT%\%~1.original" >nul || exit /b 1
if exist "%ROOT%\PATCH\INSIGNIA.386" exit /b 1
if exist "%ROOT%\PATCH\NTVDM64.386" exit /b 1
exit /b 0
