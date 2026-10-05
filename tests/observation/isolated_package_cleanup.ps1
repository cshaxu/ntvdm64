# Test-only ownership boundary. Never use this for publication or O:\winnt.
# Caller creates the scope before starting any case; exact isolated paths are
# the only cleanup authority, not image names or normal idle deadlines.
function Get-PackageBinaryRoot([string]$PackageRoot) {
    # Test-only compatibility for sealed flat baselines and build-link caches.
    # Production has one layout: <root>\system32. Never mix the two layouts.
    $system=Join-Path $PackageRoot 'system32'
    if(Test-Path -LiteralPath (Join-Path $system 'run16.exe')){
        foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntvwm32.exe','ntvwm64.exe','nthook32.dll','nthook64.dll','ntmon.exe','WOW32.DLL','VDMREDIR.DLL')){
            if(Test-Path -LiteralPath (Join-Path $PackageRoot $name)){
                throw 'Mixed product binary layout'
            }
        }
        return $system
    }
    return $PackageRoot
}
function Get-PackageNativeWorkerNames([string]$PackageRoot) {
    $binary=Get-PackageBinaryRoot $PackageRoot
    if(Test-Path -LiteralPath (Join-Path $binary 'ntvwm32.exe')){
        if(!(Test-Path -LiteralPath (Join-Path $binary 'ntvwm64.exe'))){throw 'Incomplete dual worker family'}
        return @('ntvwm32.exe','ntvwm64.exe')
    }
    if(!(Test-Path -LiteralPath (Join-Path $binary 'ntvwm.exe'))){throw 'Missing native worker family'}
    return @('ntvwm.exe')
}
function Get-PackageImageNames([string]$PackageRoot) {
    $binary=Get-PackageBinaryRoot $PackageRoot
    $workers=@(Get-PackageNativeWorkerNames $PackageRoot)
    $names=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL')+$workers
    if($workers.Count -eq 2){
        if(Test-Path -LiteralPath (Join-Path $binary 'ntvwm.exe')){throw 'Mixed legacy/dual product family'}
        foreach($hook in @('nthook32.dll','nthook64.dll')){
            if(!(Test-Path -LiteralPath (Join-Path $binary $hook))){throw 'Incomplete dual Hook family'}
            $names+=$hook
        }
    }elseif(Test-Path -LiteralPath (Join-Path $binary 'nthook32.dll')){$names+='nthook32.dll'}
    return $names
}
function New-IsolatedPackageScope([string]$PackageRoot,[string]$LaunchRoot='') {
    $physical=(Resolve-Path $PackageRoot).Path
    $build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
    if(!$physical.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){
        throw 'Test cleanup requires a repository-build package'
    }
    if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Existing broker; no test ownership'}
    $paths=@()
    $binary=Get-PackageBinaryRoot $physical
    $launchBinary=if($LaunchRoot){Get-PackageBinaryRoot $LaunchRoot}else{''}
    foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntvwm32.exe','ntvwm64.exe')){
        $file=Join-Path $binary $name
        if(!(Test-Path -LiteralPath $file)){continue}
        $paths+=$file
        if($LaunchRoot){
            $alias=Join-Path $launchBinary $name
            if((Get-FileHash $alias).Hash -ne (Get-FileHash $file).Hash){throw 'Test alias identity mismatch'}
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
        foreach($process in $processes){if(!$process.HasExited){$process.Kill()}}
        foreach($process in $processes){
            if(!$process.WaitForExit(5000)){throw 'Owned test process did not stop'}
        }
    }finally{foreach($process in $processes){$process.Dispose()}}
}
