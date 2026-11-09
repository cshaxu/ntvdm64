[CmdletBinding()]
param([Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$GuestProbes,
    [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$package=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
$probes=(Resolve-Path $GuestProbes).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){
    throw 'Require a fresh build evidence directory'
}
if($env:COMPUTERNAME -ne 'NEKOGP4'){throw 'Existing guest pipe witness requires its recorded local machine name'}
New-Item -ItemType Directory $out | Out-Null
$saved=[Environment]::GetEnvironmentVariable('TEST_RUNTIME_ROOT')
$savedInitial=[Environment]::GetEnvironmentVariable('MVDM_TEST_INITIAL_COMMAND')
$savedDirect=[Environment]::GetEnvironmentVariable('MVDM_TEST_DIRECT_CMD')
$savedPath=[Environment]::GetEnvironmentVariable('PATH')
$runtime=$null;$scope=$null;$passed=$false
$results=@()
try{
    $runtime=New-PhysicalRuntimeStage $package -FreshPackageIdentity
    $scope=New-IsolatedPackageScope $runtime
    New-Item -ItemType Directory -Force (Join-Path $runtime 'tests') | Out-Null
    foreach($name in @('VDMPASY.COM','VDMPASW.COM','VDMNETAP.COM')){
        Copy-Item -LiteralPath (Join-Path $probes $name) -Destination (Join-Path $runtime "tests/$name")
    }
    $env:TEST_RUNTIME_ROOT=$runtime
    $env:MVDM_TEST_INITIAL_COMMAND=Join-Path $runtime 'system32\COMMAND.COM'
    # Match the established bounded guest environment; do not change product
    # environment handling or allow the inherited long host PATH into COMMAND.
    $env:PATH="$env:SystemRoot\system32;$env:SystemRoot"
    [Environment]::SetEnvironmentVariable('MVDM_TEST_DIRECT_CMD',$null)
    foreach($mode in @('async','async-write','netapi')){
        $start=[Diagnostics.ProcessStartInfo]::new()
        $start.FileName=$observerPath; $start.UseShellExecute=$false; $start.CreateNoWindow=$true
        $null=$start.Environment.Remove('MVDM_TEST_DIRECT_CMD')
        $start.RedirectStandardOutput=$true; $start.RedirectStandardError=$true
        foreach($argument in @('80','30',(Join-Path $out "$mode.raw"),"--vdmredir-$mode")){
            $start.ArgumentList.Add($argument)
        }
        $p=[Diagnostics.Process]::new(); $p.StartInfo=$start
        $clock=[Diagnostics.Stopwatch]::StartNew()
        try{
            if(!$p.Start()){throw 'Observer did not start'}
            $stdout=$p.StandardOutput.ReadToEndAsync(); $stderr=$p.StandardError.ReadToEndAsync()
            if(!$p.WaitForExit(120000)){$p.Kill(); $p.WaitForExit(); throw 'Observer timeout, not a pass'}
            $stdout.Result | Set-Content (Join-Path $out "$mode.runner.txt")
            $stderr.Result | Set-Content (Join-Path $out "$mode.stderr.txt")
            $results += [pscustomobject]@{mode=$mode;exit_code=$p.ExitCode;elapsed_ms=$clock.ElapsedMilliseconds}
            Write-Output $stdout.Result
            if($p.ExitCode){throw "Guest $mode failed: $($p.ExitCode)"}
        }finally{$p.Dispose(); Stop-IsolatedPackageScope $scope}
    }
    $passed=$true
}finally{
    try{if($scope){Stop-IsolatedPackageScope $scope}}finally{
        [Environment]::SetEnvironmentVariable('TEST_RUNTIME_ROOT',$saved)
        [Environment]::SetEnvironmentVariable('MVDM_TEST_INITIAL_COMMAND',$savedInitial)
        [Environment]::SetEnvironmentVariable('MVDM_TEST_DIRECT_CMD',$savedDirect)
        [Environment]::SetEnvironmentVariable('PATH',$savedPath)
        $results | ConvertTo-Json | Set-Content (Join-Path $out 'results.json')
        if($runtime){Remove-PhysicalRuntimeStage $runtime}
    }
}
