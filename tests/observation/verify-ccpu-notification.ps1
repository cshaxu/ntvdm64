param([string]$BuildRoot = 'build/M0-T430/S6/notification')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Require fresh build evidence directory'
}
New-Item -ItemType Directory $out | Out-Null
$source=Join-Path $repo 'src/mvdm/softpc.new/base/ccpu386/c_main.c'
$body=[IO.File]::ReadAllText($source)
$parts=@()
foreach($name in @('CPU_HW_INT_MASK','CPU_SIGIO_EXCEPTION_MASK','CPU_SAD_EXCEPTION_MASK','CPU_RESET_EXCEPTION_MASK','CPU_SIGALRM_EXCEPTION_MASK')) {
    $match=[regex]::Match($body,'(?m)^#define\s+'+$name+'\s+\(1 << \d+\)\s*$')
    if(!$match.Success){throw "Missing production mask: $name"}
    $parts+=$match.Value
}
foreach($name in @('clear_any_thingies','c_cpu_event_snapshot','c_cpu_raise_event','c_cpu_take_event')) {
    $match=[regex]::Match($body,'(?ms)^(?:LOCAL|GLOBAL) (?:VOID|IUM32|IBOOL) '+$name+' IFN[01]\([^\r\n]*\)\r?\n\{.*?^\}')
    if(!$match.Success){throw "Missing complete production function: $name"}
    $parts+=$match.Value
}
[IO.File]::WriteAllText((Join-Path $out 'notification_body.inc'),($parts -join "`r`n"))
$fixture=Join-Path $repo 'tests/mvdm-host/ccpu_notification_fixture.c'
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl /nologo /W4 /MT /TC /I"'+$out+'" /Fo"'+$out+'\fixture.obj" /Fe"'+$out+'\fixture.exe" "'+$fixture+'"') 2>&1 | Tee-Object (Join-Path $out 'build.txt')
if($LASTEXITCODE){throw 'Notification fixture build failed'}
& (Join-Path $out 'fixture.exe') 2>&1 | Tee-Object (Join-Path $out 'result.txt')
if($LASTEXITCODE){throw 'Notification fixture failed'}
Get-FileHash $source,$fixture,(Join-Path $out 'notification_body.inc'),(Join-Path $out 'fixture.exe') |
    ConvertTo-Json | Set-Content (Join-Path $out 'identity.json')
