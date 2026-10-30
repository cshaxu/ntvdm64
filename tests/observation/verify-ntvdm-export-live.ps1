param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$VddBinary,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith(($repo + '\build\'), [StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Require a fresh repository-build evidence directory'
}
$VddBinary = (Resolve-Path -LiteralPath $VddBinary).Path
$Observer = (Resolve-Path -LiteralPath $Observer).Path
New-Item -ItemType Directory -Path $out | Out-Null
. "$repo\tests\observation\isolated_package_cleanup.ps1"
$runtime = $null; $scope = $null; $process = $null
try {
    $runtime = New-PhysicalRuntimeStage (Resolve-Path -LiteralPath $PackageRoot).Path -FreshPackageIdentity
    $scope = New-IsolatedPackageScope $runtime
    $vdd = "$runtime\system32\export-test.dll"
    Copy-Item -LiteralPath $VddBinary -Destination $vdd
    $bytes = [Text.Encoding]::ASCII.GetBytes($vdd) + [byte[]](0,0)
    $hex = ($bytes | ForEach-Object { $_.ToString('x2') }) -join ','
    $registry = "Windows Registry Editor Version 5.00`r`n`r`n[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\VirtualDeviceDrivers]`r`n`"VDD`"=hex(7):$hex`r`n"
    [IO.File]::WriteAllText("$runtime\system32\NTVDM.REG", $registry, [Text.Encoding]::ASCII)
    Copy-Item "$runtime\product-system32-manifest.json" "$out\manifest.json"
    Get-FileHash -LiteralPath $VddBinary, $vdd | Select-Object Path,Hash |
        ConvertTo-Json | Set-Content "$out\vdd-hashes.json"
    $start = [Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute = $false; $start.CreateNoWindow = $true
    $start.EnvironmentVariables['TEMP'] = $runtime; $start.EnvironmentVariables['TMP'] = $runtime
    $start.EnvironmentVariables['PATH'] = "$runtime\system32;$env:SystemRoot\System32"
    $start.EnvironmentVariables['NTVDM_EXPORT_TEST_TRACE'] = "$out\trace.txt"
    foreach ($arg in @("$runtime\system32\run16.exe", $runtime, "$out\observer.txt", '--wait', "$runtime\system32\MEM.EXE", '--observation-timeout-ms', '40000')) { $start.ArgumentList.Add($arg) }
    $process = [Diagnostics.Process]::Start($start)
    if (!$process.WaitForExit(45000)) { throw 'Observer did not complete' }
    if ($process.ExitCode -ne 0) { throw "Observer failed: $($process.ExitCode)" }
    if ((Get-Content "$out\observer.txt" -Raw) -notmatch 'exit=0x00000000') { throw 'MEM did not report a successful target exit' }
    if (!(Test-Path "$out\trace.txt") -or (Get-Content "$out\trace.txt" -Raw) -notmatch 'PASS NTVDM export name/ordinal query/allocate/reallocate/free') { throw 'Live VDD export witness missing' }
    if ((Get-Content "$out\trace.txt" -Raw) -notmatch 'PASS failed reallocation preserves allocation and free space') { throw 'Live VDD failed-reallocation witness missing' }
    'PASS live NTVDM VDD export witness and MEM completion' | Set-Content "$out\result.txt"
} finally {
    if ($scope) { Stop-IsolatedPackageScope $scope }
    if ($runtime) { Remove-PhysicalRuntimeStage $runtime }
    if ($process) { $process.Dispose() }
}
