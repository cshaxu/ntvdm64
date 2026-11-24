[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Require fresh build-owned evidence directory'
}
$null=New-Item -ItemType Directory $root
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds,[int]$expected=0){
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;executable=$file;deadline=$p.StartTime.AddSeconds($seconds).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name-process.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$root/$name-timeout.txt";throw "Timeout $name"}
    $p.WaitForExit();$p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit"
    if($p.ExitCode -ne $expected){throw "Failed $name actual=$($p.ExitCode) expected=$expected"}
}

try {
    & "$repo/tools/build-fixtures.cmd" x86 wow-dispatch-exception > "$root/build.log" 2>&1
    if($LASTEXITCODE){throw 'Native dispatcher build failed'}
    $native="$repo/build/.ninja/msvc-x86-release/wow32"
    Copy-Item "$native/wow-dispatch-exception.exe" "$root/fixture.exe"
    Copy-Item "$native/wow-dispatch-exception.exe.map" "$root/fixture.map"
    $map=Get-Content "$root/fixture.map" -Raw
    if($map -match '\s_(W32Init@0|W32DllInitialize@12|LockWowSharedMemory@0|fixture_unused_original_invoke@8|wow_user_borrow_enter@4|wow_user_borrow_leave_required@4|fixture_unreachable_dependency|fixture_unreachable_data)\s'){
        throw 'Unrelated original service graph survived fixture link'
    }
    Run-Owned 'fixture' "$root/fixture.exe" '--verify' 30
    $output=Get-Content "$root/fixture.out" -Raw
    if($output -notmatch 'PASS selected W32Dispatch and W32Exception' -or
        $output -notmatch 'DISPATCH_SEARCH debugger-first-chance=1 child-exit=0' -or
        ([regex]::Matches($output,'DISPATCH_EXCEPTION phase=')).Count -ne 8 -or
        ([regex]::Matches($output,'HARD_ERROR_SCOPE mode=')).Count -ne 5){throw 'Missing boundary assertions'}
    Get-FileHash "$native/wow-dispatch-exception.exe","$native/dispatch/original-wndproc.inc" |
        Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
    '0'|Set-Content "$root/exit.txt"
}catch { $_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw }
