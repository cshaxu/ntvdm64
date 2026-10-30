param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$ExpectedWow32Sha256,
    [Parameter(Mandatory)][string]$BuildRoot
)
# Only an isolated, deliberately failing diagnostic WOW32 is accepted.
# The production package and guest files are never modified.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$package=(Resolve-Path -LiteralPath $PackageRoot).Path
$observer=(Resolve-Path -LiteralPath $Observer).Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Fresh build-owned output required'
}
if($ExpectedWow32Sha256 -notmatch '^[0-9a-fA-F]{64}$' -or
        (Get-FileHash "$package/system32/WOW32.DLL").Hash -ne $ExpectedWow32Sha256) {
    throw 'Diagnostic provider identity mismatch'
}
New-Item -ItemType Directory $out | Out-Null
. "$PSScriptRoot/isolated_package_cleanup.ps1"
foreach($mode in @('default','wait')) {
    $runtime=$null;$scope=$null;$process=$null
    try {
        $runtime=New-PhysicalRuntimeStage $package
        $scope=New-IsolatedPackageScope $runtime
        Save-IsolatedPackageManifest $runtime "$out/$mode-manifest.json"
        $start=[Diagnostics.ProcessStartInfo]::new($observer)
        $start.UseShellExecute=$false;$start.CreateNoWindow=$true
        foreach($arg in @("$runtime/system32/run16.exe",$runtime,"$out/$mode-observer.txt")) {
            $start.ArgumentList.Add($arg)
        }
        if($mode -eq 'wait') { $start.ArgumentList.Add('--wait') }
        foreach($arg in @("$runtime/SOL.EXE",'--observation-timeout-ms','40000')) {
            $start.ArgumentList.Add($arg)
        }
        $start.Environment['TEMP']=$runtime;$start.Environment['TMP']=$runtime
        $start.Environment['PATH']="$runtime/system32;"+$env:SystemRoot+'\System32'
        $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
        $start.Environment['MVDM_S42_WOW_RETURN_REPORT_PATH']="$out/$mode-stage.txt"
        $start.Environment['MVDM_TEST_WOW_ATTACH_MARKER']="$out/$mode-attach.txt"
        $process=[Diagnostics.Process]::Start($start)
        if(!$process.WaitForExit(45000)){throw "$mode observer timeout"}
        $report=Get-Content "$out/$mode-observer.txt" -Raw
        $stages=Get-Content "$out/$mode-stage.txt" -Raw
        if(!(Test-Path "$out/$mode-attach.txt") -or
                (Get-Content "$out/$mode-attach.txt" -Raw) -ne 'attach-failure' -or
                $process.ExitCode -ne 0 -or $report -notmatch '(?m)^result=exited\r?$' -or
                $report -notmatch '(?m)^exit=0x0000045[aA]\r?$' -or
                $stages -notmatch 'wow-stage stage=load-failed') {
            throw "$mode did not report real WOW32 attach failure"
        }
    } finally {
        if($process){$process.Dispose()}
        try { if($scope){Stop-IsolatedPackageScope $scope} }
        finally { if($runtime){Remove-PhysicalRuntimeStage $runtime} }
    }
}
'PASS real WOW32 attach failure: default and --wait return ERROR_DLL_INIT_FAILED' |
    Set-Content "$out/result.txt"
