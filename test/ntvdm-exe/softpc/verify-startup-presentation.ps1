[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith("$repo\build\",[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
New-Item -ItemType Directory -Force $out | Out-Null
$source=Get-Content "$repo/src/ntvdm-exe/softpc/mvdm_standalone_worker.c" -Raw
# Compile the real callback without unrelated worker lifetime dependencies.
$body=[regex]::Match($source,'(?ms)^static DWORD begin_character_io\(.*?^\}').Value
if(!$body){throw 'Startup callback not found'}
$fixture=@'
#include <windows.h>
#include <stdio.h>
#include "common/protocol/console_io.h"
static BOOL StreamIoSwitchOn, presentation_only, window_mode;
static DWORD begin_error, query_error;
static int query_result=1, queries;
static DWORD ntvdm_console_client_begin(void *owner) {
    (void)owner; return begin_error;
}
static int ntvdm_console_window_query(DWORD mode,LONG values[4]) {
    ++queries;
    if(mode==CONSOLE_WINDOW_PRESENTATION_ONLY)values[0]=presentation_only;
    else if(mode==CONSOLE_WINDOW_TEXT_FRAME_REQUIRED)values[0]=presentation_only || window_mode;
    else return -1;
    SetLastError(query_error);return query_result;
}
@CALLBACK@
#define CHECK(x) do {if(!(x)){fprintf(stderr,"failed line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
    /* Window selection must preserve native Console exit/restore semantics. */
    for(int window=0;window!=2;++window) {
        window_mode=window;presentation_only=FALSE;StreamIoSwitchOn=TRUE;
        CHECK(begin_character_io(NULL,NULL)==0);CHECK(StreamIoSwitchOn);
    }
    /* A frame-only VM needs stream disabled before the first video tick. */
    presentation_only=TRUE;StreamIoSwitchOn=TRUE;
    CHECK(begin_character_io(NULL,NULL)==0);CHECK(!StreamIoSwitchOn);
    /* Never re-enable an option disabled by another owner. */
    presentation_only=FALSE;
    CHECK(begin_character_io(NULL,NULL)==0);CHECK(!StreamIoSwitchOn);
    StreamIoSwitchOn=TRUE;query_result=0;query_error=ERROR_NOT_READY;
    CHECK(begin_character_io(NULL,NULL)==ERROR_NOT_READY);CHECK(StreamIoSwitchOn);
    query_result=-1;
    CHECK(begin_character_io(NULL,NULL)==ERROR_INVALID_STATE);CHECK(StreamIoSwitchOn);
    queries=0;begin_error=ERROR_ACCESS_DENIED;
    CHECK(begin_character_io(NULL,NULL)==ERROR_ACCESS_DENIED);CHECK(queries==0);
    puts("PASS native Window, frame-only VM, disabled option and startup errors");
    return 0;
}
'@
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio\Installer\vswhere.exe'
$vs=& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(!$vs){throw 'MSVC development environment required'}
$dev=Join-Path $vs 'Common7\Tools\VsDevCmd.bat'
foreach($case in @('fixed','regression-control')) {
    $callback=$body
    if($case -eq 'regression-control'){
        $callback=$body.Replace('CONSOLE_WINDOW_PRESENTATION_ONLY','CONSOLE_WINDOW_TEXT_FRAME_REQUIRED')
        if($callback -eq $body){throw 'Expected capability selection missing: regression present'}
    }
    [IO.File]::WriteAllText("$out/$case.c",$fixture.Replace('@CALLBACK@',$callback))
    @('@echo off',('call "{0}" -arch=x86 -host_arch=x64 >nul' -f $dev),
      'if errorlevel 1 exit /b 1',
      ('cl /nologo /W4 /WX /I"{0}/src" /Fo"{1}/{2}.obj" /Fe"{1}/{2}.exe" "{1}/{2}.c"' -f $repo,$out,$case),
      'exit /b %errorlevel%') | Set-Content "$out/$case.cmd" -Encoding ascii
    $p=Start-Process cmd.exe -ArgumentList ('/d /c "{0}/{1}.cmd"' -f $out,$case) -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$case.build.log" -RedirectStandardError "$out/$case.build.err"
    if(!$p.WaitForExit(60000)){$p.Kill();throw 'Compiler timeout'}
    if($p.ExitCode){throw "Compile failed: $case"}
    $p=Start-Process "$out/$case.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$case.log" -RedirectStandardError "$out/$case.err"
    if(!$p.WaitForExit(10000)){$p.Kill();throw 'Fixture timeout'}
    if(($case -eq 'fixed' -and $p.ExitCode -ne 0) -or ($case -ne 'fixed' -and $p.ExitCode -ne 1)){throw "Unexpected result: $case exit=$($p.ExitCode)"}
}
'PASS current callback; regression control fails native Window preservation'
