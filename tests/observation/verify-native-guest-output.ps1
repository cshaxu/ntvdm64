[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [switch]$TraceHandoff,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$LogRoot='O:\winnt\Logs2'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
$physical=(Resolve-Path $ProcessPackageRoot).Path
$allowed=(Join-Path $repo 'build')+'\'
if(!$build.StartsWith($allowed,[StringComparison]::OrdinalIgnoreCase) -or
   !$physical.StartsWith($allowed,[StringComparison]::OrdinalIgnoreCase)){throw 'Use build-only fixture/candidate'}
if(Test-Path $build){throw 'Use fresh evidence root'}
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid prefix'}
$report=Join-Path (Resolve-Path $LogRoot).Path ($LogPrefix+'.txt')
if(Test-Path $report){throw 'Use fresh runtime report'}
$paths=@()
foreach($name in @('run16.exe','ntkvm.exe','ntsrv.exe','ntvdm.exe')){
    $launch=Join-Path $PackageRoot $name;$actual=Join-Path $physical $name
    if((Get-FileHash $launch).Hash -ne (Get-FileHash $actual).Hash){throw 'Package identity mismatch'}
    $paths+=@($launch,$actual)
}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Broker already active'}
foreach($name in @('CGREADY','CGDONE')){
    if(Test-Path (Join-Path $physical "tests\$name")){throw 'Existing fixture gate'}
}
$null=New-Item -ItemType Directory -Path $build
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$source=Join-Path $PSScriptRoot 'native_guest_output_test.c'
# Compiler environment must not leak into original COMMAND's finite startup
# environment. Keep VsDevCmd in a separate child, not the runtime test shell.
$compile='call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe /nologo /W4 /MT "'+$source+'" /Fo"'+$build+'\\" /Fe"'+$build+'\CG.EXE" /link /incremental:no user32.lib'
& cmd.exe /d /s /c $compile *> (Join-Path $build 'build.log')
if($LASTEXITCODE){throw 'Fixture compile failed'}
Copy-Item (Join-Path $build 'CG.EXE') (Join-Path $physical 'tests\CG.EXE')
$batch=([IO.File]::ReadAllText((Join-Path $PSScriptRoot 'native_guest_output.bat')) -replace '\r?\n',"`r`n")
$batch=$batch.Replace('@PACKAGE@',$PackageRoot.TrimEnd('\')+'\')
[IO.File]::WriteAllText((Join-Path $physical 'tests\CG.BAT'),$batch,[Text.Encoding]::ASCII)
$old=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$oldTrace=$env:MVDM_S34_TRACE_PATH
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    if($TraceHandoff){$env:MVDM_S34_TRACE_PATH=$report+'.handoff.txt'}
    else {Remove-Item Env:MVDM_S34_TRACE_PATH -ErrorAction SilentlyContinue}
    & $Observer (Join-Path $PackageRoot 'run16.exe') $PackageRoot $report --observation-timeout-ms 60000 tests\CG.EXE ($report+'.native.txt')
    if($LASTEXITCODE){throw 'Observer failed'}
    $record=Get-Content $report -Raw
    $screen=Get-Content ($report+'.console.txt') -Raw
    if($record -notmatch '(?m)^result=exited\r?$' -or $record -notmatch '(?m)^exit=0x00000025\r?$'){
        throw 'Native/guest completion failed'
    }
    $text=($screen -replace '(?m)^\[\d+\] ','') -replace '\s',''
    foreach($marker in @('NATIVE-BASE','DOS-INTERVAL','NATIVE-DURING-DOS','DOS-RESUMED','NATIVE-AFTER-DOS','PASSREAL-GUEST-CONCURRENTresult=0')){
        if(!$text.Contains($marker)){throw "Missing actual visible output: $marker"}
    }
    'PASS real COMMAND batch plus concurrent native ConPTY output; direct results DOS=0 native=37'
} finally {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$old
    $env:MVDM_S34_TRACE_PATH=$oldTrace
    # Close only exact candidate processes after recording the verdict.
    Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths} | ForEach-Object {
        $process=Get-Process -Id $_.ProcessId -ErrorAction SilentlyContinue
        if($process){try {
            $null=$process.Handle
            if($process.Path -notin $paths){throw 'Pinned process identity changed'}
            $process.Kill();$null=$process.WaitForExit(5000)
        } finally {$process.Dispose()}}
    }
    foreach($name in @('CGREADY','CGDONE')){
        $source=Join-Path $physical "tests\$name"
        if(Test-Path $source){Move-Item -LiteralPath $source -Destination (Join-Path $build $name)}
    }
}
