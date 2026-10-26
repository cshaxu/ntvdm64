param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Nasm = 'nasm.exe'
)
# This is a negative contract witness: no host profile/printer state is changed.
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root = [IO.Path]::GetFullPath($BuildRoot)
if (!$root.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $root)) { throw 'Require a fresh build-owned evidence root' }
$null = New-Item -ItemType Directory -Path $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime = $null; $scope = $null; $process = $null
$saved = @{}; foreach ($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')) { $saved[$name] = [Environment]::GetEnvironmentVariable($name) }
try {
    $source = Join-Path $PSScriptRoot 'win16_raw_spool_probe.asm'
    & (Get-Command $Nasm -ErrorAction Stop).Source -f bin -o "$root/RAWPROBE.EXE" -l "$root/probe.lst" $source *> "$root/assemble.txt"
    if ($LASTEXITCODE) { throw 'Independent raw spool probe assembly failed' }
    $runtime = New-PhysicalRuntimeStage (Resolve-Path -LiteralPath $PackageRoot).Path
    $scope = New-IsolatedPackageScope $runtime
    $binary = Get-PackageBinaryRoot $runtime; $tests = Join-Path $runtime 'tests'
    $null = New-Item -ItemType Directory -Path $tests -Force
    Copy-Item "$root/RAWPROBE.EXE" "$tests/RAWPROBE.EXE"
    @($source,"$root/RAWPROBE.EXE",(Join-Path $binary 'WOW32.DLL')) | Get-FileHash |
        Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
    $env:TEMP=$runtime; $env:TMP=$runtime; $env:PATH="$binary;"+$env:SystemRoot+'\System32'; $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path -LiteralPath $Observer).Path)
    $start.UseShellExecute=$false; $start.CreateNoWindow=$true
    foreach($arg in @((Join-Path $binary 'run16.exe'),$tests,"$root/observer.txt",'--wait',"$tests/RAWPROBE.EXE",'--observation-timeout-ms','30000')){$start.ArgumentList.Add($arg)}
    $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
    if(!$process.WaitForExit(35000)){throw 'Owned raw spool observer exceeded deadline'};$process.Refresh()
    if($process.ExitCode -or (Get-Content "$root/observer.txt" -Raw) -notmatch 'result=exited'){throw 'Raw spool probe did not complete normally'}
    $reportPath = @("$tests/RAWPROBE.BIN", "$runtime/RAWPROBE.BIN") |
        Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
    if (!$reportPath) { throw 'Raw spool probe did not write its report' }
    Copy-Item $reportPath "$root/guest-report.bin"
    $bytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
    if($bytes.Length -ne 8 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'RAW1'){throw 'Malformed raw spool report'}
    $row=[ordered]@{openJob=[BitConverter]::ToUInt16($bytes,4);closeJob=[BitConverter]::ToUInt16($bytes,6)}
    $row|ConvertTo-Json|Set-Content "$root/guest-report.json"
    if($row.openJob -ne 0 -or $row.closeJob -ne 0){throw 'Raw spool profile witness unexpectedly found a legacy mapping'}
    'PASS raw OpenJob reaches original explicit no-profile failure without host configuration'|Set-Content "$root/result.txt";'0'|Set-Content "$root/result.exit"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/result.exit";throw}
finally{
    if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
    try{if($scope){Stop-IsolatedPackageScope $scope}}finally{try{if($runtime){Remove-PhysicalRuntimeStage $runtime}}finally{foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}}}
}
