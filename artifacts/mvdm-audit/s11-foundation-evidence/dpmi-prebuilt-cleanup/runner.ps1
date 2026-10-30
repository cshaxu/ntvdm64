param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][ValidateSet('interrupt16','interrupt32','code32','task-cleanup','ems-call','xms-failure')][string]$Case
)
$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$sourceRoot=Join-Path $repo 'tests/observation'
. "$sourceRoot/isolated_package_cleanup.ps1"
$baseline=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $out)){throw 'Require a fresh build run root'}
if(Test-Path Z:\){throw 'Z: already in use'}
# A fresh test-only package preserves the published inputs and sealed baseline.
New-Item -ItemType Directory $out | Out-Null
$runtime=Join-Path $out 'runtime'
Copy-Item -LiteralPath $baseline -Destination $runtime -Recurse
$binary=Get-PackageBinaryRoot $runtime
$names='run16.exe','ntsrv.exe','ntvdm.exe','ntvwm.exe','ntcon.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL'
$identity=@($names | ForEach-Object {
    [pscustomobject]@{name=$_;sha256=(Get-FileHash (Join-Path $binary $_)).Hash}
})
$identity | ConvertTo-Json | Set-Content (Join-Path $out 'package.json')
$definitions=@(); $target='COMMAND.COM'; $expected=@()
switch($Case){
    'interrupt16' {$source='dpmi_interrupt_return.asm';$expected=@('S38_INT16_RETURN_OK','S38_FAULT16_RETURN_NEGATIVE_OK','S38_HARDWARE_IRQ_RETURN_OK')}
    'interrupt32' {$source='dpmi_interrupt_return.asm';$definitions=@('-DCLIENT32');$expected=@('S38_INT32_RETURN_OK','S38_FAULT32_RETURN_NEGATIVE_OK','S38_HARDWARE_IRQ_RETURN_OK')}
    'code32' {$source='dpmi_interrupt_return.asm';$definitions=@('-DCLIENT32','-DCODE32');$expected=@('S38_INT32_RETURN_OK','S38_FAULT32_RETURN_NEGATIVE_OK','S38_HARDWARE_IRQ_RETURN_OK')}
    'task-cleanup' {$source='dpmi_task_cleanup.asm';$expected=@('S36_DPMI_TASK_EXIT_CAPACITY_RESTORED_OK')}
    'ems-call' {$source='ems_call_return_probe.asm';$expected=@('T430_EMS_CALL_RETURN_STACK_MAP_OK');$target='E30.PIF'}
    'xms-failure' {$source='xms_capability.asm';$definitions=@('-DFORCED_RELOCATION');$expected=@('S36_XMS_FORCED_RELOCATION_DATA_RELEASE_OK')}
}
if ($Case -ne 'task-cleanup') { throw 'Prebuilt runner supports task-cleanup only' }
$prebuilt=Join-Path $repo 'build/t456-s2/package-r2/tests'
$expectedHashes=@{'D36T.COM'='E7C6398A4CE40639E6491B44B0FE47CD733A2E9CCFB24EBC155FCF060A9FE7A1';'D36N.COM'='9A1574AD8A537D901C6A0C390682D2F60872BD678D6893013B1F934DD2AD4DAD';'D36L.COM'='698DA97B3C254472F4C74CA17258FCED21AE04FC66657B66D9CBD0A131E2C5BD'}
foreach($name in $expectedHashes.Keys) { if((Get-FileHash (Join-Path $prebuilt $name)).Hash -ne $expectedHashes[$name]){throw "Prebuilt identity mismatch: $name"} }
$probe=Join-Path $runtime 'tests/S6PROBE.COM'
Copy-Item (Join-Path $prebuilt 'D36T.COM') $probe
foreach($name in @('D36N.COM','D36L.COM')){Copy-Item (Join-Path $prebuilt $name) (Join-Path $runtime "tests/$name")}
Get-FileHash (Join-Path $sourceRoot $source),$probe,$PSCommandPath,$observerPath |
    ConvertTo-Json | Set-Content (Join-Path $out 'guest-identity.json')
if($Case -eq 'task-cleanup'){
    Get-FileHash "$sourceRoot/dpmi_suballoc.asm",(Join-Path $runtime 'tests/D36N.COM'),(Join-Path $runtime 'tests/D36L.COM') |
        ConvertTo-Json | Set-Content (Join-Path $out 'child-identity.json')
}
Get-FileHash (Join-Path $binary 'DOSX.EXE'),(Join-Path $binary 'HIMEM.SYS') |
    ConvertTo-Json | Set-Content (Join-Path $out 'original-media.json')
$scope=New-IsolatedPackageScope $runtime
$environmentNames='PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_MILESTONE_INPUT','MVDM_OBSERVER_SHORT_HISTORY'
$saved=@{}; foreach($name in $environmentNames){$saved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')}
$mapped=$false
try{
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST failed'}
    $mapped=$true
    $scope=New-IsolatedPackageScope $runtime 'Z:\'
    $env:PATH='Z:\system32;'+$env:SystemRoot+'\System32'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_MILESTONE_INPUT='1'
    $env:MVDM_OBSERVER_SHORT_HISTORY='1'
    if($Case -eq 'ems-call'){
        & (Join-Path $out 'pif-builder.exe') 'Z:\system32' --close-on-exit
        if($LASTEXITCODE){throw 'EMS profile generation failed'}
    }
    $report=Join-Path $out 'guest.txt'
    $start=[Diagnostics.ProcessStartInfo]::new()
    $start.FileName=$observerPath; $start.UseShellExecute=$false; $start.CreateNoWindow=$true
    $probeArgs=@('Z:\system32\run16.exe','Z:\',$report,$target)
    # /c propagates the actual probe completion instead of treating bare EXIT's
    # established outer-shell status as the guest's result.
    if($Case -ne 'ems-call'){$probeArgs+=@('/c','Z:\tests\S6PROBE.COM')}
    $probeArgs+=@('--observation-timeout-ms','25000')
    foreach($arg in $probeArgs){$start.ArgumentList.Add($arg)}
    $start.RedirectStandardOutput=$true; $start.RedirectStandardError=$true
    $p=[Diagnostics.Process]::new(); $p.StartInfo=$start
    try{
        if(!$p.Start()){throw 'Observer did not start'}
        $stdout=$p.StandardOutput.ReadToEndAsync(); $stderr=$p.StandardError.ReadToEndAsync()
        if(!$p.WaitForExit(35000)){$p.Kill();$p.WaitForExit();throw 'Observer timeout, not pass'}
        ($stdout.Result+$stderr.Result) | Set-Content (Join-Path $out 'observer.txt')
        if($p.ExitCode -or !(Test-Path $report)){throw 'Missing/failed observer report'}
        $text=Get-Content $report -Raw
        if($text -notmatch '(?m)^result=exited\r?$' -or $text -notmatch '(?m)^exit=0x00000000\r?$'){
            throw 'Guest/outer COMMAND did not exit zero'
        }
        $screenPath="$report.console.txt"
        $screen=Get-Content $screenPath -Raw
        foreach($marker in $expected){if($screen -notmatch [regex]::Escape($marker)){throw "Missing actual guest marker: $marker"}}
        if($screen -match 'S38_FAIL|S36_DPMI_FAIL|S36_DPMI_TASK_EXIT_FAIL|T430_EMS_CALL_RETURN_FAIL|S35_XMS_FAIL'){throw 'Guest failure marker'}
        foreach($row in $identity){if((Get-FileHash (Join-Path $binary $row.name)).Hash -ne $row.sha256){throw 'Product changed during test'}}
        [pscustomobject]@{case=$Case;result='PASS';markers=$expected;exit=0;definitions=$definitions} |
            ConvertTo-Json | Set-Content (Join-Path $out 'result.json')
        "PASS actual guest $Case"
    }finally{$p.Dispose()}
}finally{
    try{Stop-IsolatedPackageScope $scope}finally{
        if($mapped){& subst.exe Z: /d; if($LASTEXITCODE){throw 'Owned Z: cleanup failed'}}
        foreach($name in $environmentNames){
            if($null -eq $saved[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
            else{[Environment]::SetEnvironmentVariable($name,$saved[$name],'Process')}
        }
    }
}
