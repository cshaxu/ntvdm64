[CmdletBinding()]
param([Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$GuestProbes,
    [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
$probes=(Resolve-Path $GuestProbes).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){
    throw 'Require a fresh build evidence directory'
}
if($env:COMPUTERNAME -ne 'NEKOGP4'){throw 'Existing guest pipe witness requires its recorded local machine name'}
if(Test-Path Z:\){throw 'Z: already in use'}
$scope=New-IsolatedPackageScope $runtime
New-Item -ItemType Directory $out | Out-Null
foreach($name in @('VDMPASY.COM','VDMPASW.COM','VDMNETAP.COM')){
    Copy-Item -LiteralPath (Join-Path $probes $name) -Destination (Join-Path $runtime "tests/$name")
}
$saved=[Environment]::GetEnvironmentVariable('TEST_RUNTIME_ROOT')
$savedInitial=[Environment]::GetEnvironmentVariable('MVDM_TEST_INITIAL_COMMAND')
$savedDirect=[Environment]::GetEnvironmentVariable('MVDM_TEST_DIRECT_CMD')
$mapped=$false
$results=@()
try{
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST failed'}
    $mapped=$true
    $scope=New-IsolatedPackageScope $runtime 'Z:\'
    $env:TEST_RUNTIME_ROOT='Z:\'
    $env:MVDM_TEST_INITIAL_COMMAND='Z:\system32\COMMAND.COM'
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
}finally{
    try{Stop-IsolatedPackageScope $scope}finally{
        if($mapped){& subst.exe Z: /d}
        [Environment]::SetEnvironmentVariable('TEST_RUNTIME_ROOT',$saved)
        [Environment]::SetEnvironmentVariable('MVDM_TEST_INITIAL_COMMAND',$savedInitial)
        [Environment]::SetEnvironmentVariable('MVDM_TEST_DIRECT_CMD',$savedDirect)
        $results | ConvertTo-Json | Set-Content (Join-Path $out 'results.json')
    }
}
