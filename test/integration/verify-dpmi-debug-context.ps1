[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Fresh build-owned evidence directory required'
}
$package=(Resolve-Path $PackageRoot).Path
$observer=(Resolve-Path $Observer).Path
New-Item -ItemType Directory $out | Out-Null
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
try {
    # These are project-owned probes, not modified original guest binaries.
    foreach($case in @(@('dpmi_watchpoint','WATCH'),@('dpmi_debug_lifetime','LIFE'))) {
        # Original DOSX status-query inversion is an immutable guest boundary
        # documented in m0-t420-s38-dpmi32-capability-review.md. Exercise actual
        # delivery/removal groups here; do not claim 0B02/0B03 status acceptance.
        [string[]]$defines=@(if($case[0] -eq 'dpmi_watchpoint'){'-DDELIVERY_GROUPS'})
        & nasm @defines -f bin "$PSScriptRoot/../observation/$($case[0]).asm" -o "$out/$($case[1]).COM"
        if($LASTEXITCODE){throw "Probe assembly failed: $($case[0])"}
    }
    $runtime=New-PhysicalRuntimeStage $package
    Save-IsolatedPackageManifest $runtime "$out/manifest.json"
    $scope=New-IsolatedPackageScope $runtime
    New-Item -ItemType Directory -Force "$runtime/tests" | Out-Null
    Copy-Item "$out/WATCH.COM","$out/LIFE.COM" "$runtime/tests"
    $cases=@(
        @{name='WATCH';marker='S38_DEBUG_SCALAR_RMW_STRING_STACK_CODE_OK'},
        @{name='LIFE';marker='S38_DEBUG_EXIT_HANDLE='},
        @{name='LIFE';marker='S38_DEBUG_EXIT_HANDLE='}
    )
    for($i=0;$i -lt $cases.Count;$i++) {
        $case=$cases[$i];$report="$out/case-$i.txt"
        $start=[Diagnostics.ProcessStartInfo]::new($observer)
        $start.UseShellExecute=$false;$start.CreateNoWindow=$true
        $start.EnvironmentVariables['PATH']="$runtime\system32;"+$env:SystemRoot+'\System32'
        $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
        $start.EnvironmentVariables['MVDM_OBSERVER_MILESTONE_INPUT']='1'
        foreach($arg in @("$runtime\system32\run16.exe",$runtime,$report,'COMMAND.COM','/c',"$runtime\tests\$($case.name).COM",'--observation-timeout-ms','25000')) {
            $start.ArgumentList.Add($arg)
        }
        $process=[Diagnostics.Process]::Start($start)
        if(!$process.WaitForExit(35000)){throw "Observer timeout: $i"}
        if($process.ExitCode -or (Get-Content $report -Raw) -notmatch 'result=exited[\s\S]*exit=0x00000000') {
            throw "Guest launcher failed: $i"
        }
        $screen=Get-Content "$report.console.txt" -Raw
        if($screen -notmatch [regex]::Escape($case.marker) -or $screen -match 'S38_FAIL|ALLOCATION_FAILED') {
            throw "Guest assertion failed: $i"
        }
        $process.Dispose();$process=$null
    }
    'PASS real DPMI watchpoint delivery/removal groups and two normal debug-handle exits; status-query contract not covered' | Set-Content "$out/result.txt"
} finally {
    if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit()};$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
}
