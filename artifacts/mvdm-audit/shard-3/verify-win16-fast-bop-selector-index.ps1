[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Nasm='nasm.exe'
)
# Disposable independently authored NE; original guest files stay unchanged.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $root)){throw 'Require fresh build-owned evidence root'}
$null=New-Item -ItemType Directory -Path $root
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
$names=@('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')}
try {
    $package=(Resolve-Path -LiteralPath $PackageRoot).Path
    $observer=(Resolve-Path -LiteralPath $Observer).Path
    $source=Join-Path $repo 'artifacts/mvdm-audit/shard-3/win16-fast-bop-selector-index-probe.asm'
    $assembler=(Get-Command $Nasm -ErrorAction Stop).Source
    & $assembler -f bin -o "$root\DCPROBE.EXE" -l "$root\probe.lst" $source *> "$root\assemble.txt"
    if($LASTEXITCODE){throw 'Independent DC probe assembly failed'}
    Get-FileHash $source,"$root\DCPROBE.EXE",$observer,"$package\system32\WOW32.DLL" |
        Export-Csv -NoTypeInformation "$root\inputs.csv"
    $runtime=New-PhysicalRuntimeStage $package -FreshPackageIdentity
    $stageRows=@(Get-Content "$runtime/product-system32-manifest.json" -Raw | ConvertFrom-Json)
    [pscustomobject]@{ SourcePackageId=(Get-Content "$package/product-system32-manifest.json" -Raw | ConvertFrom-Json | Select-Object -First 1 -ExpandProperty PackageId); StagePackageId=($stageRows | Select-Object -First 1 -ExpandProperty PackageId); KernelSha256=(Get-FileHash "$runtime/system32/KRNL386.EXE").Hash; WowSha256=(Get-FileHash "$runtime/system32/WOW32.DLL").Hash } | ConvertTo-Json | Set-Content "$root/stage-identity.json"
    $scope=New-IsolatedPackageScope $runtime
    $null=New-Item -ItemType Directory -Force -Path "$runtime\tests"
    Copy-Item "$root\DCPROBE.EXE" "$runtime\tests\DCPROBE.EXE"
    $env:TEMP=$runtime;$env:TMP=$runtime
    $env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $start=[Diagnostics.ProcessStartInfo]::new($observer)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    foreach($argument in @("$runtime\system32\run16.exe","$runtime\tests",
            "$root\observer.txt",'--wait',"$runtime\tests\DCPROBE.EXE",
            '--observation-timeout-ms','60000')){$start.ArgumentList.Add($argument)}
    $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
    @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');
      arguments=@($start.ArgumentList)}|ConvertTo-Json|Set-Content "$root\observer-process.json"
    if(!$process.WaitForExit(65000)){throw 'Owned DC observer deadline'}
    $process.Refresh();$process.ExitCode|Set-Content "$root\observer-exit.txt"
    if($process.ExitCode){throw "DC observer exit $($process.ExitCode)"}
    $report=Get-Content "$root\observer.txt" -Raw
    if($report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){
        throw 'DC guest/launcher did not complete normally'
    }
    $guest="$runtime\tests\DCPROBE.BIN"
    if(!(Test-Path -LiteralPath $guest)){throw 'DC probe did not emit its report'}
    Copy-Item $guest "$root\guest-report.bin"
    $bytes=[IO.File]::ReadAllBytes("$root\guest-report.bin")
    if($bytes.Length -ne 18 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'FB01') {throw 'Invalid guest report'}
    $endpoint=[BitConverter]::ToUInt16($bytes,4); $offset=[BitConverter]::ToUInt16($bytes,6)
    $offsetHigh=[BitConverter]::ToUInt16($bytes,8); $hit=[BitConverter]::ToUInt16($bytes,10)
    $status=[BitConverter]::ToUInt16($bytes,12); $remaining=[BitConverter]::ToUInt16($bytes,14); $peak=[BitConverter]::ToUInt16($bytes,16)
    if(!$endpoint -or $status -ne 0 -or $remaining -ne 0 -or $hit -gt 1) {throw 'Invalid endpoint or incomplete release'}
    "PASS endpoint=$endpoint offset=$offset offsetHigh=$offsetHigh peak=$peak indexHit=$hit releaseAttemptsRemaining=$remaining"

} finally {
    if($process){if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(10000)};$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $names){[Environment]::SetEnvironmentVariable($name,$saved[$name],'Process')}
}
