# Test-only ownership boundary. Never use this for publication or O:\winnt.
# Caller creates the scope before starting any case; exact isolated paths are
# the only cleanup authority, not image names or normal idle deadlines.
function New-IsolatedPackageScope([string]$PackageRoot,[string]$LaunchRoot='') {
    $physical=(Resolve-Path $PackageRoot).Path
    $build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
    if(!$physical.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){
        throw 'Test cleanup requires a repository-build package'
    }
    if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Existing broker; no test ownership'}
    $paths=@()
    foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')){
        $file=Join-Path $physical $name
        $paths+=$file
        if($LaunchRoot){
            $alias=Join-Path $LaunchRoot $name
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
