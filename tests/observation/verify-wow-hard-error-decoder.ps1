[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Require a fresh build-owned evidence directory'
}
$null=New-Item -ItemType Directory $root
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds){
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
    @{pid=$p.Id;startedUtc=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;deadlineUtc=$p.StartTime.AddSeconds($seconds).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name.runner.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$root/$name.timeout.txt";throw "Timeout $name"}
    $p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit"
    if($p.ExitCode -ne 0){throw "Failed $name actual=$($p.ExitCode)"}
}
try {
    $flags='/nologo /TC /W4 /WX /MT /DWIN32 /I"'+$repo+'/src/opennt-abi/host-compat/include" /I"'+$repo+'/src/ntvdm-exe/win32"'
    $sources='"'+$repo+'/src/ntvdm-exe/win32/wow_hard_error_decoder.c" "'+$repo+'/tests/adapter-mvdm-host-out/wow/wow_hard_error_decoder_fixture.c"'
    $command='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe '+$flags+' /Fe"'+$root+'/fixture.exe" '+$sources+'"'
    Run-Owned 'compile' 'cmd.exe' $command 120
    Run-Owned 'fixture' "$root/fixture.exe" '' 30
    $output=Get-Content "$root/fixture.out" -Raw
    if($output -notmatch 'PASS source-slot/default/cancel decoder contract' -or
       ([regex]::Matches($output,'^PASS ',[Text.RegularExpressions.RegexOptions]::Multiline)).Count -ne 5){
        throw 'Missing hard-error decoder assertions'
    }
    @("$repo/src/ntvdm-exe/win32/wow_hard_error_decoder.c","$repo/src/ntvdm-exe/win32/wow_hard_error_dialog.h","$repo/tests/adapter-mvdm-host-out/wow/wow_hard_error_decoder_fixture.c")|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
    '0'|Set-Content "$root/exit.txt"
}catch { $_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw }
