param(
    [Parameter(Mandatory)][string]$ProviderBuildRoot,
    [Parameter(Mandatory)][string]$WorkerBuildRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$StyleDll
)
# Supplemental native/actual-CCPU contracts, never real Win16 acceptance or
# authorization to publish. The caller supplies an already built coherent
# worker/provider/style set; each row refreshes its dependency-selected owners.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $root)){throw 'Fresh build-owned evidence root required'}
$provider=(Resolve-Path $ProviderBuildRoot).Path
$worker=(Resolve-Path $WorkerBuildRoot).Path
$style=(Resolve-Path $StyleDll).Path
$shell=(Get-Process -Id $PID).Path
$null=New-Item -ItemType Directory $root
try{
    $roots=@('src/wow32-dll','src/opennt-host','src/opennt-abi/host-compat',
        'src/mvdm/wow32','src/ntvdm-exe/wow','src/ntvdm-exe/softpc',
        'src/ntw16s-dll','tests/adapter-mvdm-host-out/wow','tests/mvdm-host',
        'tests/observation/verify-wow-task-lifecycle.ps1',
        'tests/observation/verify-wow-native-slot-group.ps1')
    $deleted=@(& git -C $repo ls-files -d -- @roots)
    if($LASTEXITCODE){throw 'Deleted input inventory failed'}
    $deleted|Set-Content "$root/deleted-inputs.txt"
    $paths=@(& git -C $repo ls-files -co --exclude-standard -- @roots)|Sort-Object -Unique
    if($LASTEXITCODE){throw 'Input inventory failed'}
    $paths|Where-Object {$_ -notin $deleted}|ForEach-Object {
        Get-FileHash -LiteralPath (Join-Path $repo $_)
    }|Export-Csv -NoTypeInformation "$root/source-sha256.csv"
    Get-FileHash "$provider/wow32.dll","$worker/ntvdm.exe",$style |
        Export-Csv -NoTypeInformation "$root/artifact-sha256.csv"
    $rows=@('window-borrow','native-dispatch-gate','window-procedure','class-client',
        'class-query','class-procedure','class-lifetime','native-creation',
        'standard-class','task-lifecycle','process-retirement','dialog-lifecycle',
        'native-send','native-nested','native-foreign')|ForEach-Object {
        @{name=$_;case=$_;arguments=''}
    }
    $rows+=@(
        @{name='creation-unwind';case='native-creation';arguments='-CreationUnwind'},
        @{name='init-rollback';case='task-lifecycle';arguments='-InitRollback'},
        @{name='styled-window-borrow';case='window-borrow';arguments='-StyleDll "'+$style+'"'},
        @{name='style-policy';case='style-policy';arguments='-StyleDll "'+$style+'"'},
        @{name='graph-negative';case='graph-snapshot';arguments=''}
    )
    $results=@()
    foreach($row in $rows){
        $line='-NoProfile -File "'+$repo+'/tests/observation/verify-wow-task-lifecycle.ps1" -ProviderBuildRoot "'+$provider+'" -WorkerBuildRoot "'+$worker+'" -BuildRoot "'+$root+'/'+$row.name+'" -Case '+$row.case+' '+$row.arguments
        $p=Start-Process $shell -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$($row.name).stdout.txt" -RedirectStandardError "$root/$($row.name).stderr.txt"
        $null=$p.Handle
        $start=$p.StartTime.ToUniversalTime()
        @{pid=$p.Id;start=$start.ToString('o');command=$line;timeoutSeconds=900} |
            ConvertTo-Json|Set-Content "$root/$($row.name).process.json"
        Write-Host "$($row.name) PID=$($p.Id)"
        if(!$p.WaitForExit(900000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned case deadline exceeded'}
        $p.WaitForExit();$code=$p.ExitCode;$p.Dispose()
        $code|Set-Content "$root/$($row.name).exit.txt"
        $results+=@{name=$row.name;exit=$code;elapsedSeconds=([DateTime]::UtcNow-$start).TotalSeconds}
        $results|ConvertTo-Json|Set-Content "$root/results.json"
        Write-Host "$($row.name) exit=$code"
    }
    foreach($input in Import-Csv "$root/source-sha256.csv"){
        if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Source changed during frozen group'}
    }
    foreach($input in Import-Csv "$root/artifact-sha256.csv"){
        if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Artifact changed during frozen group'}
    }
    $failed=@($results|Where-Object {$_.exit -ne 0}).Count
    @{cases=$results.Count;failed=$failed;scope='supplemental native/real CCPU; not real guest'} |
        ConvertTo-Json|Set-Content "$root/summary.json"
    $failed|Set-Content "$root/exit.txt"
    exit ([int]($failed -ne 0))
}catch{
    $_|Out-String|Set-Content "$root/failure.txt"
    '1'|Set-Content "$root/exit.txt";exit 1
}
