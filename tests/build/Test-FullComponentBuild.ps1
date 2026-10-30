param([string]$OutputRoot = 'build/td-component-build')
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$output = [IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if (!$output.StartsWith((Join-Path $repo 'build')+'\', [StringComparison]::OrdinalIgnoreCase) -or
    $OutputRoot -match '(^|[\\/])\.\.?([\\/]|$)' -or
    $output.StartsWith((Join-Path $repo 'build/.ninja'), [StringComparison]::OrdinalIgnoreCase) -or
    $output.StartsWith((Join-Path $repo 'build/.ccache'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Evidence must use a disposable repository build child, not a cache'
}
$null = New-Item -ItemType Directory -Force -Path $output
Set-Location $repo
$cmake = @(Get-Command cmake.exe -All -ErrorAction Stop |
    Where-Object { $_.Source -notmatch '[\\/]mingw(32|64)[\\/]' } |
    Select-Object -First 1).Source
if (!$cmake) {
    throw 'Full component build requires a non-MinGW cmake.exe; a MinGW CMake cannot configure the MSVC preset reliably.'
}
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$steps = [Collections.Generic.List[object]]::new()
$started = [DateTime]::UtcNow
function Get-Sha256([string]$Path) {
    $stream=[IO.File]::OpenRead($Path)
    $hash=[Security.Cryptography.SHA256]::Create()
    try {return ([BitConverter]::ToString($hash.ComputeHash($stream))).Replace('-','')}
    finally {$hash.Dispose();$stream.Dispose()}
}
function Run-Step([string]$Name, [string]$Architecture, [string]$Command) {
    $batch = Join-Path $output "$Name.cmd"
    [IO.File]::WriteAllText($batch, "@echo off`r`ncall `"$vs`" -arch=$Architecture -host_arch=x64 >nul`r`nif errorlevel 1 exit /b %errorlevel%`r`n$Command`r`nexit /b %errorlevel%`r`n")
    $stdout = Join-Path $output "$Name.stdout.log"
    $stderr = Join-Path $output "$Name.stderr.log"
    $process = Start-Process cmd.exe -ArgumentList @('/d','/c',"`"$batch`"") -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    $null = $process.Handle
    $step = [ordered]@{name=$Name; pid=$process.Id; startUtc=$process.StartTime.ToUniversalTime().ToString('o'); command=$Command; batch=$batch; stdout=$stdout; stderr=$stderr; deadlineSeconds=1500; exitCode=$null}
    $steps.Add($step)
    $steps | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $output 'steps.json')
    $lastProgress = [DateTime]::UtcNow; $lastSize = -1L
    while ($true) {
        $process.Refresh()
        if($process.HasExited){break}
        Start-Sleep -Milliseconds 1000
        $size = (Get-Item $stdout).Length + (Get-Item $stderr).Length
        if ($size -ne $lastSize) {$lastSize=$size; $lastProgress=[DateTime]::UtcNow}
        if (([DateTime]::UtcNow-$started).TotalMinutes -gt 40 -or
            ([DateTime]::UtcNow-$process.StartTime.ToUniversalTime()).TotalSeconds -gt 1500 -or
            ([DateTime]::UtcNow-$lastProgress).TotalMinutes -gt 15 -or $size -gt 64MB) {
            & taskkill.exe /PID $process.Id /T /F | Out-Null
            $process.WaitForExit()
            throw "Owned build exceeded time/progress/log budget: $Name"
        }
    }
    $step.exitCode = $process.ExitCode
    $step.endUtc = [DateTime]::UtcNow.ToString('o')
    $steps | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $output 'steps.json')
    if ($process.ExitCode -ne 0) {throw "$Name failed: $($process.ExitCode); see $stdout / $stderr"}
    Write-Output "PASS $Name"
}
function Build-Components([string]$Pass) {
    Run-Step "$Pass-x86" x86 "`"$cmake`" --build --preset msvc-x86-release-ccache --target wow32 --parallel 12"
    $runner = Join-Path $repo 'build/.ninja/msvc-x86-release/worker/run-ninja-parallel.cmd'
    Run-Step "$Pass-x86-hook-redir" x86 "call `"$runner`" nthook32.dll VDMREDIR.dll"
    Run-Step "$Pass-x64" x64 "`"$cmake`" --build --preset msvc-x64-release-ccache --target native-worker native-frontend native-monitor native-launcher native-service hook64 wow_message_observer --parallel 2"
}
function Intermediate-Snapshot {
    $snapshot = @{}
    foreach ($architecture in @('x86','x64')) {
        $tree = Join-Path $repo "build/.ninja/msvc-$architecture-release"
        foreach ($file in Get-ChildItem $tree -Recurse -File | Where-Object Extension -in @('.obj','.lib','.res')) {
            $snapshot[$file.FullName] = "$($file.Length):$($file.LastWriteTimeUtc.Ticks)"
        }
    }
    return $snapshot
}
$result = [ordered]@{startUtc=$started.ToString('o'); passed=$false; images=@(); intermediateCount=0; unchangedIntermediates=$false; error=$null}
try {
    foreach ($architecture in @('x86','x64')) {
        Run-Step "configure-$architecture" $architecture "`"$cmake`" --preset msvc-$architecture-release-ccache"
    }
    Build-Components 'first'
    $x86 = Join-Path $repo 'build/.ninja/msvc-x86-release'
    $x64 = Join-Path $repo 'build/.ninja/msvc-x64-release'
    # The DLL must borrow the worker's existing session/TLS, not link a second
    # session implementation merely to satisfy its terminal-thread helper.
    Run-Step 'redirector-worker-import' x86 "dumpbin.exe /nologo /imports:ntvdm.exe `"$x86/worker/VDMREDIR.dll`""
    $imports = Get-Content (Join-Path $output 'redirector-worker-import.stdout.log') -Raw
    $provider = Get-Content (Join-Path $x86 'worker/generated/ntvdm-wow32-provider.def') -Raw
    $finish = [regex]::Match($provider, '(?m)^\s*session_thread_finish\s+@(\d+)\b')
    if (!$finish.Success -or ($imports -notmatch '\bsession_thread_finish\b' -and
        $imports -notmatch ('(?m)^\s*Ordinal\s+' + $finish.Groups[1].Value + '\s*$'))) {
        throw 'Redirector terminal cleanup must import session_thread_finish from ntvdm.exe'
    }
    $images = @(
        @('ntvdm.exe',"$x86/worker/ntvdm.exe",0x14c),
        @('WOW32.DLL',"$x86/wow32/WOW32.DLL",0x14c),
        @('VDMREDIR.DLL',"$x86/worker/VDMREDIR.dll",0x14c),
        @('nthook32.dll',"$x86/worker/nthook32.dll",0x14c),
        @('ntvwm.exe',"$x64/worker/ntvwm.exe",0x8664),
        @('ntcon.exe',"$x64/frontend/ntcon.exe",0x8664),
        @('ntmon.exe',"$x64/monitor/ntmon.exe",0x8664),
        @('run16.exe',"$x64/launcher/run16.exe",0x8664),
        @('ntsrv.exe',"$x64/service/ntsrv.exe",0x8664),
        @('nthook64.dll',"$x64/hook/nthook64.dll",0x8664)
    )
    foreach ($image in $images) {
        $bytes = [IO.File]::ReadAllBytes($image[1]); $pe=[BitConverter]::ToInt32($bytes,0x3c)
        if ([BitConverter]::ToUInt32($bytes,$pe) -ne 0x4550 -or [BitConverter]::ToUInt16($bytes,$pe+4) -ne $image[2]) {throw "Wrong PE architecture: $($image[0])"}
        $file=Get-Item $image[1]
        $result.images += [pscustomobject]@{name=$image[0]; path=$file.FullName; machine=$image[2]; sha256=(Get-Sha256 $file.FullName); bytes=$file.Length; timestamp=$file.LastWriteTimeUtc.Ticks}
    }
    $before = Intermediate-Snapshot
    Build-Components 'second'
    $after = Intermediate-Snapshot
    if (!$before.Count -or $before.Count -ne $after.Count) {throw 'Intermediate inventory changed on second build'}
    foreach ($path in $before.Keys) {if ($before[$path] -ne $after[$path]) {throw "Intermediate rebuilt: $path"}}
    foreach ($image in $result.images) {
        if ((Get-Sha256 $image.path) -ne $image.sha256 -or (Get-Item $image.path).LastWriteTimeUtc.Ticks -ne $image.timestamp) {throw "Output rebuilt: $($image.name)"}
    }
    foreach ($name in @('second-x86','second-x86-hook-redir','second-x64')) {
        $log=Get-Content (Join-Path $output "$name.stdout.log") -Raw
        if ($log -notmatch 'ninja: no work to do' -or $log -match '(?m)^\[\d+/\d+\] (cl |CL |CC |CXX |Building |Linking |LIB |LINK )') {throw "Second build was not idle: $name"}
    }
    $result.intermediateCount=$before.Count
    $result.unchangedIntermediates=$true
    $result.passed=$true
} catch {$result.error=$_.ToString(); Write-Error $_ -ErrorAction Continue}
finally {
    $result.endUtc=[DateTime]::UtcNow.ToString('o')
    $result | ConvertTo-Json -Depth 6 | Set-Content (Join-Path $output 'result.json')
}
if (!$result.passed) {exit 1}
Write-Output "FULL_COMPONENT_BUILD_OK images=$($result.images.Count) intermediates=$($result.intermediateCount)"
