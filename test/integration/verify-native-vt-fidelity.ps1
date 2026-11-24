param(
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [string]$FrontendBuild,
    [Parameter(Mandatory=$true)][string]$Observer,
    [string]$RepositoryRoot=(Split-Path (Split-Path $PSScriptRoot)),
    [string]$VsDevCmd='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
# Invoke this script only in an owned background process. Dependencies are
# current frontend objects from the shared native build graph.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path $RepositoryRoot).Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith("$repo\build\",[StringComparison]::OrdinalIgnoreCase)){throw 'BuildRoot must be below repository build'}
New-Item -ItemType Directory $root -Force|Out-Null
$env:TEMP="$root\tmp";$env:TMP=$env:TEMP
New-Item -ItemType Directory $env:TEMP -Force|Out-Null
$timings=[Collections.Generic.List[object]]::new()
function Run([string]$name,[string]$exe,[string]$arguments,[int]$seconds) {
    $watch=[Diagnostics.Stopwatch]::StartNew()
    $p=Start-Process $exe -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root\$name.stdout.txt" -RedirectStandardError "$root\$name.stderr.txt"
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime();command="$exe $arguments";root=$root;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root\$name.owner.json"
    if(!$p.WaitForExit($seconds*1000)){$p.Kill($true);$p.WaitForExit();throw "$name timeout"}
    $p.Refresh();$timings.Add(@{name=$name;elapsedMs=$watch.ElapsedMilliseconds;exitCode=$p.ExitCode})
    $timings|ConvertTo-Json|Set-Content "$root\timings.json"
    if($p.ExitCode){throw "$name exit $($p.ExitCode)"}
}
try {
    & "$repo/tools/build.cmd" x64 *> "$root/build-frontend.txt"
    if($LASTEXITCODE){throw 'Current frontend build failed'}
    $frontend="$repo/build/.ninja/msvc-x64-release/frontend"
    & "$frontend/run-ninja.cmd" native-vt-fidelity.exe *> "$root/build-verifier.txt"
    if($LASTEXITCODE){throw 'VT verifier build failed'}
    Copy-Item "$frontend/native-vt-fidelity.exe" "$root/verify.exe"
    foreach($arch in @('x64','x86')) {
        & "$repo/tools/build-fixtures.cmd" $arch native-vt-producer *> "$root/build-$arch.txt"
        if($LASTEXITCODE){throw "$arch VT producer build failed"}
        Copy-Item "$repo/build/.ninja/fixtures-$arch/native-vt-producer.exe" "$root/producer-$arch.exe"
    }
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($arch in @('x64','x86')) {
        $args='"'+$root+'\verify.exe" "'+$root+'" "'+$root+'\observer-'+$arch+'.txt" "'+$root+'\result-'+$arch+'.txt" "'+$root+'\producer-'+$arch+'.exe" --observation-timeout-ms 60000'
        Run "verify-$arch" $Observer $args 90
        if((Get-Content "$root\observer-$arch.txt" -Raw) -notmatch 'exit=0x00000000'){throw "$arch verifier failed"}
        if((Get-Content "$root\result-$arch.txt" -Raw) -notmatch 'RESULT checks=\d+ failures=0'){throw "$arch assertions failed"}
    }
    Get-ChildItem $root -Filter '*.exe'|Get-FileHash|ConvertTo-Json|Set-Content "$root\artifacts.json"
    'PASS x86/x64 writers, real Console conversion, production capture/frame/projection/pixels and raw file/pipe bytes'|Set-Content "$root\result.txt"
    0|Set-Content "$root\exitcode.txt"
}catch{$_|Out-String|Set-Content "$root\failure.txt";1|Set-Content "$root\exitcode.txt";exit 1}
