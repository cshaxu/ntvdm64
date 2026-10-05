[CmdletBinding()]
param([Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $out)){throw 'Require a fresh build directory'}
if(Test-Path Z:\){throw 'Z: already in use'}
$scope=New-IsolatedPackageScope $runtime
$owned=Join-Path $runtime 'tests/DFCB01'
if(Test-Path $owned){throw 'Guest directory already exists; cannot claim ownership'}
New-Item -ItemType Directory $out | Out-Null
New-Item -ItemType Directory $owned | Out-Null
$probe=Join-Path $out 'FCBTEST.COM'
& nasm -f bin "$PSScriptRoot/dem_fcb_probe.asm" -o $probe
if($LASTEXITCODE){throw 'Guest probe assembly failed'}
Copy-Item $probe (Join-Path $owned 'FCBTEST.COM')
foreach($name in @('AAA.TST','BBB.TST','CCC.TST')){
    [IO.File]::WriteAllText((Join-Path $owned $name),'AB',[Text.Encoding]::ASCII)
}
$mapped=$false
$savedPath=$env:PATH
$savedDesktop=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$savedMilestone=$env:MVDM_OBSERVER_MILESTONE_INPUT
try{
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST failed'}
    $mapped=$true
    $scope=New-IsolatedPackageScope $runtime 'Z:\'
    $env:PATH='Z:\system32;'+$savedPath
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_MILESTONE_INPUT='1'
    $report=Join-Path $out 'guest.txt'
    $start=[Diagnostics.ProcessStartInfo]::new()
    $start.FileName=$observerPath; $start.UseShellExecute=$false; $start.CreateNoWindow=$true
    foreach($arg in @('Z:\system32\run16.exe','Z:\',$report,'COMMAND.COM',
        '--observe-console-input-text',"Z:\tests\DFCB01\FCBTEST.COM`rexit`r",
        '--observation-timeout-ms','20000')){
        $start.ArgumentList.Add($arg)
    }
    $start.RedirectStandardOutput=$true; $start.RedirectStandardError=$true
    $p=[Diagnostics.Process]::new(); $p.StartInfo=$start
    $clock=[Diagnostics.Stopwatch]::StartNew()
    try{
        if(!$p.Start()){throw 'Observer did not start'}
        $stdout=$p.StandardOutput.ReadToEndAsync(); $stderr=$p.StandardError.ReadToEndAsync()
        if(!$p.WaitForExit(30000)){$p.Kill(); $p.WaitForExit(); throw 'Timeout, not a pass'}
        ($stdout.Result+$stderr.Result) | Set-Content (Join-Path $out 'observer.txt')
        if($p.ExitCode -or !(Test-Path $report)){throw 'Observer/report failure'}
        $text=Get-Content $report -Raw
        if($text -notmatch '(?m)^result=exited\r?$' -or $text -notmatch '(?m)^exit=0x00000001\r?$'){
            throw 'Outer COMMAND did not complete normally'
        }
        $trace=Join-Path $owned 'TRACE.BIN'
        if(!(Test-Path $trace)){throw 'No guest-owned witness'}
        $data=[IO.File]::ReadAllBytes($trace)
        if($data.Length -ne 40 -or [Text.Encoding]::ASCII.GetString($data,0,4) -ne 'FCB1' -or
            $data[4] -ne 3 -or $data[5] -ne 255 -or $data[6] -ne 255){throw 'FCB count/end/missing failure'}
        $names=0..2 | ForEach-Object {[Text.Encoding]::ASCII.GetString($data,7+11*$_,11)}
        if((($names | Sort-Object) -join '|') -ne 'AAA     TST|BBB     TST|CCC     TST'){
            throw 'Wrong/duplicate FCB filename cells'
        }
        Copy-Item $trace (Join-Path $out 'TRACE.BIN')
        [pscustomobject]@{result='PASS';elapsed_ms=$clock.ElapsedMilliseconds;
            names=$names;command_exit=1;probe_sha256=(Get-FileHash $probe).Hash;
            scope='Actual INT21 FCB first/next/end/missing + outer COMMAND completion; forced idle/mutation restart covered by provider fixture only'
        } | ConvertTo-Json | Set-Content (Join-Path $out 'result.json')
        Write-Output 'DEM-FCB-GUEST-PASS: 3 unique names, EOF and missing FF, outer COMMAND completion'
    }finally{$p.Dispose()}
}finally{
    try{Stop-IsolatedPackageScope $scope}finally{
        if($mapped){& subst.exe Z: /d}
        $env:PATH=$savedPath
        [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_PRIVATE_DESKTOP',$savedDesktop)
        [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_MILESTONE_INPUT',$savedMilestone)
    }
    # Only the five exact fixture-created paths, never recursive/glob cleanup.
    foreach($name in @('AAA.TST','BBB.TST','CCC.TST','FCBTEST.COM','TRACE.BIN')){
        $file=Join-Path $owned $name
        if(Test-Path $file){Remove-Item -LiteralPath $file}
    }
    Remove-Item -LiteralPath $owned
}
