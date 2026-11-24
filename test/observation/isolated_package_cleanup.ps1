# Test-only ownership boundary. Never use this for publication or O:\winnt.
# Caller creates the scope before starting any case; exact isolated paths are
# the only cleanup authority, not image names or normal idle deadlines.
$script:IsolatedPackageCleanupRoot=$PSScriptRoot
function Get-IsolatedFileSha256([string]$LiteralPath) {
    $path=(Resolve-Path -LiteralPath $LiteralPath -ErrorAction Stop).Path
    $algorithm=[Security.Cryptography.SHA256]::Create()
    $stream=[IO.File]::OpenRead($path)
    try {return ([BitConverter]::ToString($algorithm.ComputeHash($stream))).Replace('-','')}
    finally {$stream.Dispose();$algorithm.Dispose()}
}
function Save-IsolatedPackageManifest([string]$Runtime, [string]$Destination) {
    $manifest = Join-Path $Runtime 'product-system32-manifest.json'
    $rows = Get-Content -LiteralPath $manifest -Raw | ConvertFrom-Json
    $rows = @($rows)
    $expected = @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe',
        'ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll','VCDEX.DLL')
    if (!$rows.Count -or $rows.Count -gt $expected.Count -or
            @($rows | Select-Object -ExpandProperty Name -Unique).Count -ne $rows.Count) {
        throw 'Expected unique staged component records'
    }
    foreach ($row in $rows) {
        if ($row.Name -notin $expected -or $row.RelativePath -ne ('system32\' + $row.Name) -or
                [IO.Path]::GetFileName($row.Name) -ne $row.Name) {
            throw 'Invalid staged component path'
        }
        if ((Get-IsolatedFileSha256 (Join-Path $Runtime $row.RelativePath)) -ne $row.Sha256) {
            throw "Staged component hash mismatch: $($row.Name)"
        }
    }
    Copy-Item -LiteralPath $manifest -Destination $Destination
}
function Get-IsolatedPackageIdentity([string]$Image) {
    $reader = Join-Path $script:IsolatedPackageCleanupRoot '../../build/.ninja/build-tools/ntvdm-package-id.exe'
    if (!(Test-Path -LiteralPath $reader -PathType Leaf)) {
        throw 'Build native tools with tools/build/bootstrap.cmd first'
    }
    $identity = & $reader --read $Image
    if ($LASTEXITCODE -ne 0) { return $null }
    if ($identity -cnotmatch '^[0-9A-F]{32}$') { throw 'Invalid package identity reader output' }
    return $identity
}
function Get-PhysicalRuntimeStageRoot {
    return 'O:\tmp'
}
function Test-PhysicalRuntimeStage([string]$Path) {
    if (!(Test-Path -LiteralPath $Path -PathType Container)) { return $false }
    $root = (Resolve-Path -LiteralPath $Path).Path
    $stageRoot = (Get-PhysicalRuntimeStageRoot).TrimEnd('\') + '\'
    if (!$root.StartsWith($stageRoot, [StringComparison]::OrdinalIgnoreCase)) { return $false }
    return Test-Path -LiteralPath (Join-Path $root '.ntvdm64-test-stage.json') -PathType Leaf
}
function New-PhysicalRuntimeStage([string]$PackageRoot,
    [ValidateSet('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')]
    [string[]]$RuntimeImages=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe'),
    [string[]]$TopLevelNames=@(),
    [switch]$FreshPackageIdentity,
    [string[]]$TestFiles=@(),
    [string]$TestSubdirectory,
    [switch]$TestInBinary) {
    if(!$RuntimeImages.Count){throw 'At least one runtime image is required'}
    $runner=Join-Path $script:IsolatedPackageCleanupRoot '../../build/.ninja/build-tools/stage-runtime.cmd'
    $arguments=@('-PackageRoot',(Resolve-Path -LiteralPath $PackageRoot).Path)
    foreach($name in $RuntimeImages){$arguments+=@('-RuntimeImage',$name)}
    foreach($name in $TopLevelNames){$arguments+=@('-Include',$name)}
    foreach($file in $TestFiles){$arguments+=@('-TestFile',$file)}
    if($TestSubdirectory){$arguments+=@('-TestSubdirectory',$TestSubdirectory)}
    if($TestInBinary){$arguments+='-TestInBinary'}
    if($FreshPackageIdentity){$arguments+='-FreshPackageIdentity'}
    $stage=@(& $runner @arguments)
    if($LASTEXITCODE -or $stage.Count -ne 1){throw "CMD physical staging failed ($LASTEXITCODE)"}
    return $stage[0]
}
function Remove-PhysicalRuntimeStage([string]$Stage) {
    if (!(Test-PhysicalRuntimeStage $Stage)) { throw 'Refuse to remove a non-stage path' }
    $root = (Resolve-Path -LiteralPath $Stage).Path
    $marker = Get-Content -LiteralPath (Join-Path $root '.ntvdm64-test-stage.json') -Raw | ConvertFrom-Json
    foreach ($row in @($marker.binaries)) {
        $path = Join-Path (Get-PackageBinaryRoot $root) $row.name
        if (!(Test-Path -LiteralPath $path) -or
            (Get-IsolatedFileSha256 $path) -ne $row.sha256) {
            throw 'Test stage binary identity changed; preserve it for diagnosis'
        }
    }
    # A surviving broker keeps its image open; reject before recursive
    # removal so an active stage is not left partially deleted.
    $prefix=$root.TrimEnd('\')+'\'
    $active=@(Get-CimInstance Win32_Process | Where-Object {
        $_.ExecutablePath -and $_.ExecutablePath.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)
    })
    if($active.Count){throw 'Test stage still has running processes; stop the owned scope before removal'}
    Remove-Item -LiteralPath $root -Recurse -Force
}
function Remove-OwnedGuestProbeCopy([string]$Destination,[string]$Task,[string]$Source) {
    if(!(Test-Path -LiteralPath $Destination)){return}
    $root=(Resolve-Path -LiteralPath $Destination).Path
    $taskRoot=(Resolve-Path -LiteralPath $Task).Path
    $buildRoot=(Resolve-Path "${script:IsolatedPackageCleanupRoot}/../../build").Path.TrimEnd('\')+'\'
    $tempRoot=(Resolve-Path -LiteralPath 'O:\tmp').Path.TrimEnd('\')
    if($tempRoot -ine 'O:\tmp' -or !$root.StartsWith($tempRoot+'\',[StringComparison]::OrdinalIgnoreCase) -or
       !$taskRoot.StartsWith($buildRoot,[StringComparison]::OrdinalIgnoreCase)) {
        throw 'Refuse guest probe cleanup: invalid target or task root'
    }
    $cursor=Get-Item -LiteralPath $root -Force
    while($cursor.FullName.Length -ge $tempRoot.Length){
        if($cursor.Attributes -band [IO.FileAttributes]::ReparsePoint){throw 'Refuse guest probe cleanup: linked ancestor'}
        $cursor=$cursor.Parent
        if(!$cursor){break}
    }
    $cursor=Get-Item -LiteralPath $taskRoot -Force
    while($cursor.FullName.Length -ge $buildRoot.TrimEnd('\').Length){
        if($cursor.Attributes -band [IO.FileAttributes]::ReparsePoint){throw 'Refuse guest probe cleanup: linked evidence path'}
        $cursor=$cursor.Parent
        if(!$cursor){break}
    }
    $owner=Join-Path $root '.t479-owner'
    if(!(Test-Path -LiteralPath $owner -PathType Leaf) -or
       (Get-Content -LiteralPath $owner -Raw).Trim() -ine $taskRoot) {
        throw 'Refuse guest probe cleanup: owner mismatch'
    }
    $prefix=$root+'\'
    $self=Get-CimInstance Win32_Process -Filter "ProcessId=$PID"
    $supervisor=if($self){$self.ParentProcessId}else{0}
    $active=@(Get-CimInstance Win32_Process | Where-Object {
        $_.ProcessId -ne $PID -and $_.ProcessId -ne $supervisor -and
        (($_.ExecutablePath -and $_.ExecutablePath.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)) -or
         ($_.CommandLine -and $_.CommandLine.IndexOf($prefix,[StringComparison]::OrdinalIgnoreCase) -ge 0))
    })
    if($active.Count){throw 'Refuse guest probe cleanup: directory still in use'}
    $contents=@(Get-ChildItem -LiteralPath $root -Force -Recurse -ErrorAction Stop)
    if(@($contents|Where-Object {$_.Name -ieq '.git' -or ($_.Attributes -band [IO.FileAttributes]::ReparsePoint)}).Count){
        throw 'Refuse guest probe cleanup: Git data or link'
    }
    $patch=Join-Path $root 'PATCH'
    if(Test-Path -LiteralPath $patch){
        $witnesses=@(Get-ChildItem -LiteralPath $patch -File | Where-Object {
            $_.Name -match '^(?i:(?:A|B|BOOT|READY|VMLOAD|DONE|REUSE|READERS|EDITORS|MENUSTG|MOUSE\d+)\.(?:BIN|TXT))$'
        })
        foreach($witness in $witnesses){
            $original=Join-Path (Join-Path $Source 'PATCH') $witness.Name
            if((Test-Path -LiteralPath $original) -and
               (Get-IsolatedFileSha256 $original) -eq (Get-IsolatedFileSha256 $witness.FullName)){continue}
            if($witness.Length -gt 1MB){throw 'Refuse guest probe cleanup: unusually large witness'}
            $evidence=Join-Path $taskRoot 'guest-witnesses'
            if(!(Test-Path -LiteralPath $evidence)){New-Item -ItemType Directory -Path $evidence|Out-Null}
            Copy-Item -LiteralPath $witness.FullName -Destination (Join-Path $evidence $witness.Name)
        }
    }
    if(((Get-Item -LiteralPath $root -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) -or
       (Get-Content -LiteralPath $owner -Raw).Trim() -ine $taskRoot){
        throw 'Refuse guest probe cleanup: target changed'
    }
    Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction Stop
}
function Get-PackageBinaryRoot([string]$PackageRoot) {
    # Test-only compatibility for sealed flat baselines and build-link caches.
    # Production has one layout: <root>\system32. Never mix the two layouts.
    $system=Join-Path $PackageRoot 'system32'
    if(Test-Path -LiteralPath (Join-Path $system 'run16.exe')){
        foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL')){
            if(Test-Path -LiteralPath (Join-Path $PackageRoot $name)){
                throw 'Mixed product binary layout'
            }
        }
        return $system
    }
    return $PackageRoot
}
function Assert-NoConflictingPackageBroker([string]$BrokerImage) {
    # A different verified UUID has a different endpoint. Unknown or equal
    # identity remains a conflict; never adopt or stop another test's broker.
    $ownIdentity=Get-IsolatedPackageIdentity $BrokerImage
    foreach($brokerRow in @(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'")){
        $brokerProcess=$null
        try {
            try {$brokerProcess=[Diagnostics.Process]::GetProcessById($brokerRow.ProcessId)}catch [ArgumentException]{continue}
            $null=$brokerProcess.Handle
            if(!$brokerProcess.HasExited){
                $otherIdentity=if($brokerRow.ExecutablePath) {Get-IsolatedPackageIdentity $brokerRow.ExecutablePath}else{$null}
                if(!$ownIdentity -or !$otherIdentity -or $ownIdentity -eq $otherIdentity){
                    throw "Existing live broker $($brokerRow.ProcessId) $($brokerRow.ExecutablePath); no test ownership"
                }
            }
        }finally{if($brokerProcess){$brokerProcess.Dispose()}}
    }
}
function New-IsolatedPackageScope([string]$PackageRoot,[string]$LaunchRoot='') {
    $physical=(Resolve-Path $PackageRoot).Path
    $build=(Resolve-Path "${script:IsolatedPackageCleanupRoot}/../../build").Path+'\'
    if(!$physical.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -and
       !(Test-PhysicalRuntimeStage $physical)){
        throw 'Test cleanup requires a repository-build package'
    }
    $binary=Get-PackageBinaryRoot $physical
    Assert-NoConflictingPackageBroker (Join-Path $binary 'ntsrv.exe')
    $paths=@()
    $launchBinary=if($LaunchRoot){Get-PackageBinaryRoot $LaunchRoot}else{''}
    foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')){
        $file=Join-Path $binary $name
        $paths+=$file
        if($LaunchRoot){
            $alias=Join-Path $launchBinary $name
            if((Get-IsolatedFileSha256 $alias) -ne (Get-IsolatedFileSha256 $file)){throw 'Test alias identity mismatch'}
            $paths+=$alias
        }
    }
    $prior=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths})
    if($prior.Count){throw 'Existing package user; no test ownership'}
    [pscustomobject]@{Paths=$paths;Root=$physical}
}
function Stop-IsolatedPackageScope($Scope) {
    if(!$Scope -or !$Scope.Paths){throw 'Missing isolated test ownership scope'}
    $owned=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $Scope.Paths})
    Stop-IdentityCheckedProcesses $owned $Scope.Paths
    if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $Scope.Paths}).Count){
        throw 'Owned package resource remains after explicit cleanup'
    }
}
# The caller supplies only rows already selected by its ownership boundary.
# Pin every process before killing any; validate image AND creation identity.
function Stop-IdentityCheckedProcesses($Rows,[string[]]$Paths) {
    $processes=@()
    try {
        foreach($row in $Rows){
            if($row.ExecutablePath -notin $Paths){throw 'Unowned cleanup image'}
            try {$process=[Diagnostics.Process]::GetProcessById($row.ProcessId)}catch{continue}
            try {
                $null=$process.Handle
                if($process.HasExited){$process.Dispose();continue}
                if($process.MainModule.FileName -ne $row.ExecutablePath -or
                    $process.MainModule.FileName -notin $Paths -or
                    !$row.CreationDate -or
                    [Math]::Abs(($process.StartTime.ToUniversalTime()-$row.CreationDate.ToUniversalTime()).Ticks) -gt 10){
                    throw 'Cleanup PID no longer identifies the owned package image'
                }
            }catch{$process.Dispose();throw}
            $processes+=$process
        }
        foreach($process in $processes){if(!$process.HasExited){
            try {$process.Kill()}
            catch [ComponentModel.Win32Exception] {
                # Stopping an owned broker can concurrently end its workers.
                # Accept only a signaled pinned process handle, never an
                # access-denied error by itself or a disappeared WMI row.
                if(!$process.WaitForExit(1000)){throw}
            }
        }}
        foreach($process in $processes){
            if(!$process.WaitForExit(5000)){throw 'Owned test process did not stop'}
        }
    }finally{foreach($process in $processes){$process.Dispose()}}
}
