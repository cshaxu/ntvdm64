[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Require a fresh build-owned evidence directory'
}
$null=New-Item -ItemType Directory $root
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds){
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
    @{pid=$p.Id;startedUtc=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;deadlineUtc=$p.StartTime.AddSeconds($seconds).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name.runner.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$root/$name.timeout.txt";throw "Timeout $name"}
    $p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit"
    if($p.ExitCode -ne 0){throw "Failed $name actual=$($p.ExitCode)"}
}
try {
    & "$repo/tools/build-fixtures.cmd" x86 wow-hard-error-decoder > "$root/build.log" 2>&1
    if($LASTEXITCODE){throw 'Native hard-error build failed'}
    Copy-Item "$repo/build/.ninja/fixtures-x86/wow-hard-error-decoder.exe" "$root/fixture.exe"
    Run-Owned 'fixture' "$root/fixture.exe" '' 30
    $output=Get-Content "$root/fixture.out" -Raw
    if($output -notmatch 'PASS source-slot/default/cancel decoder contract' -or
       ([regex]::Matches($output,'^PASS ',[Text.RegularExpressions.RegexOptions]::Multiline)).Count -ne 6){
        throw 'Missing hard-error decoder assertions'
    }
    @("$repo/src/ntvdm-exe/win32/wow_hard_error_dialog.c","$repo/test/adapter-mvdm-host-out/wow/wow_hard_error_request_fixture.c","$repo/src/ntvdm-exe/win32/wow_hard_error_decoder.c","$repo/src/ntvdm-exe/win32/wow_hard_error_dialog.h","$repo/test/adapter-mvdm-host-out/wow/wow_hard_error_decoder_fixture.c")|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
    '0'|Set-Content "$root/exit.txt"
}catch { $_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw }
