# Delegation only: CMD/native owns source selection, compilation and caching.
function Invoke-Win16ProbeBuild {
    param([Parameter(Mandatory)][string]$OutputRoot,
          [Parameter(Mandatory)][ValidateSet('PDCANCEL','PDGPROBE','RAWPROBE','THPROBE','DISCOVER')][string]$Name,
          [string]$Listing='probe.lst')
    $repository=(Resolve-Path "$PSScriptRoot/../..").Path
    $target=if($Name -eq 'DISCOVER'){'win16-discovery'}else{'win16-probes'}
    $line='/d /s /c ""'+$repository+'/tools/build-fixtures.cmd" x86 '+$target+'"'
    $p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repository -WindowStyle Hidden -PassThru -RedirectStandardOutput "$OutputRoot/assemble.txt" -RedirectStandardError "$OutputRoot/assemble.stderr.txt"
    try {
        @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line}|ConvertTo-Json|Set-Content "$OutputRoot/producer.json"
        if(!$p.WaitForExit(300000)){
            $p.Kill($true);$p.WaitForExit();throw 'Win16 producer deadline'
        }
        $p.ExitCode|Set-Content "$OutputRoot/producer.exit"
        if($p.ExitCode){throw 'Win16 producer failed'}
    } finally {$p.Dispose()}
    $cache="$repository/build/.ninja/fixtures-x86/guest-probes/win16"
    Copy-Item "$cache/$Name.EXE" "$OutputRoot/$Name.EXE"
    Copy-Item "$cache/$Name.lst" "$OutputRoot/$Listing"
    if($Name -eq 'DISCOVER') {
        Copy-Item "$repository/build/.ninja/fixtures-x86/wow-discovery-peer.exe" "$OutputRoot/peer.exe"
    }
}
