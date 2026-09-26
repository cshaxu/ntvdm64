param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$WindowObserver,
    [Parameter(Mandatory)][string]$Prefix,
    [string]$PackageRoot='O:\winnt',
    [ValidateSet('WINMINE.EXE','SOL.EXE','WRITE.EXE')]
    [string[]]$Guests=@('WINMINE.EXE','SOL.EXE','WRITE.EXE')
)
# Read-only GUI observations on an unswitched desktop. This does not assert
# gameplay, send input, change a profile or count a timeout as acceptance.
$ErrorActionPreference='Stop'
if($Prefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid prefix'}
$Observer=(Resolve-Path $Observer).Path
$WindowObserver=(Resolve-Path $WindowObserver).Path
$PackageRoot=(Resolve-Path $PackageRoot).Path
$paths=@('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object {Join-Path $PackageRoot $_}
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object {$_.ExecutablePath -in $paths})
}
if((PackageProcesses).Count){throw 'Package already in use'}
$profile=Get-FileHash (Join-Path $PackageRoot 'SYSTEM.INI')
foreach($guest in $Guests){
    $stem=Join-Path $PackageRoot ('logs\'+$Prefix+'-'+$guest.Split('.')[0].ToLowerInvariant())
    if(Test-Path ($stem+'.txt')){throw 'Use a fresh evidence prefix'}
    $launcher=$null
    try {
        $args='"{0}" "{1}" "{2}" {3} --observation-timeout-ms 20000' -f
            (Join-Path $PackageRoot 'run16.exe'),$PackageRoot,($stem+'.txt'),$guest
        $start=[Diagnostics.ProcessStartInfo]::new($Observer,$args)
        $start.UseShellExecute=$false
        $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
        $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
        $launcher=[Diagnostics.Process]::Start($start)
        $started=[DateTime]::UtcNow
        $deadline=[DateTime]::UtcNow.AddSeconds(12)
        $worker=$null
        do {
            $worker=@(PackageProcesses | Where-Object {$_.Name -eq 'ntvdm.exe'}) | Select-Object -First 1
            if($worker -or $launcher.HasExited){break}
            Start-Sleep -Milliseconds 100
        } while([DateTime]::UtcNow -lt $deadline)
        if($worker){
            $samples=[Collections.Generic.List[string]]::new()
            foreach($second in @(4,8,12,16)){
                $remaining=($started.AddSeconds($second)-[DateTime]::UtcNow).TotalMilliseconds
                if($remaining -gt 0 -and $launcher.WaitForExit([int][Math]::Ceiling($remaining))){break}
                if($launcher.HasExited){break}
                $snapshot=@(& $WindowObserver ([string]$worker.ProcessId) ('NTVDMConsoleTest-'+$launcher.Id))
                if($LASTEXITCODE){throw "Window observation failed: $LASTEXITCODE"}
                if(($snapshot -join "`n") -match 'enumeration-error=|open-desktop-error='){
                    throw 'Desktop inspection failed; this is not application evidence'
                }
                $samples.Add(('sample-ms={0}' -f [int]([DateTime]::UtcNow-$started).TotalMilliseconds))
                foreach($line in $snapshot){$samples.Add([string]$line)}
                $samples | Set-Content -LiteralPath ($stem+'-windows.txt')
            }
        }
        if(!$launcher.WaitForExit(30000)){throw 'Observer exceeded its bound'}
        if(!(Test-Path ($stem+'.txt'))){throw 'No observation report'}
        Write-Output "OBSERVED $guest (not gameplay acceptance)"
        Get-Content -LiteralPath ($stem+'.txt') | Select-String '^result=|^exit='
        if(Test-Path ($stem+'-windows.txt')){Get-Content -LiteralPath ($stem+'-windows.txt')}
    } finally {
        foreach($item in @(PackageProcesses)){
            $process=Get-Process -Id $item.ProcessId -ErrorAction SilentlyContinue
            if($process){$process.Kill();if(!$process.WaitForExit(5000)){throw 'Package cleanup timeout'}}
        }
        if($launcher -and !$launcher.HasExited){$launcher.Kill();$launcher.WaitForExit()}
    }
}
if((Get-FileHash (Join-Path $PackageRoot 'SYSTEM.INI')).Hash -ne $profile.Hash){throw 'Profile changed'}
