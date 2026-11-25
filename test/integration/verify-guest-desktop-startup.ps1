[CmdletBinding()]
param(
    [Parameter(Mandatory)][ValidateSet('win31-standard','win31-enhanced','win95')][string]$Case,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$InstallSource,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot,
    [switch]$DosFullscreenRoundtrip
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$is95=$Case -eq 'win95'
$version=if($is95){'95'}else{'31'}
$mode=if($Case -eq 'win31-standard'){'std'}else{'386'}
$prepare="$PSScriptRoot/ntvdm64-386/New-Win${version}ProbeCopy.ps1"
$copy=& $prepare -InstallSource $InstallSource -InstallRoot "O:/tmp/q$version" -OutputRoot $OutputRoot -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386"
try {
    $options=if($is95){@()}elseif($mode -eq 'std'){@('1')}else{@('2')}
    & "$repo/tools/select-guest-launch.cmd" $copy.Destination $copy.Task $version 0 0 install @options *> "$OutputRoot/select.log"
    if($LASTEXITCODE){throw "Could not select the installed $Case profile"}
    $runtimeOptions=@{}
    if($DosFullscreenRoundtrip) {
        if($mode -ne '386'){throw 'DOS fullscreen roundtrip requires enhanced mode'}
        & "$repo/tools/build-fixtures.cmd" x86 win31-profile $copy.Destination *> "$OutputRoot/profile-build.log"
        if($LASTEXITCODE){throw 'Fullscreen witnesses did not build'}
        $line=Select-String -LiteralPath "$OutputRoot/profile-build.log" -Pattern '^NTVDM_PROFILE_BIN=(.+)$'
        if(!$line){throw 'Missing fullscreen witness directory'}
        $cache=$line.Matches[0].Groups[1].Value
        & "$repo/tools/prepare-test-inputs.cmd" -GuestPatch $copy.Destination $copy.Task "$PackageRoot/system32/COMMAND.COM" COMMAND.COM
        if($LASTEXITCODE){throw 'Cannot stage unchanged guest COMMAND'}
        & "$repo/tools/prepare-test-inputs.cmd" -GuestRoot $copy.Destination $copy.Task "$cache/ROUNDKEY.COM" FULLKEY.COM
        if($LASTEXITCODE){throw 'Cannot stage fullscreen keyboard witness'}
        & "$repo/assets/release/PIF.EXE" create "$($copy.Destination)/DOSCHECK.PIF" --title 'DOS fullscreen roundtrip' --program "$($copy.Destination)\PATCH\COMMAND.COM" --directory $copy.Destination --arguments "/c $($copy.Destination)\FULLKEY.COM" --config '' --autoexec '' --close-on-exit --w386-flags 0x21002 --min-memory 128 --max-memory 640
        if($LASTEXITCODE){throw 'Cannot configure windowed DOS prompt'}
        $runtimeOptions=@{WindowsProbeImage="$cache/ROUND.EXE";FullscreenRoundtrip=$true}
    }
    & "$PSScriptRoot/win31_runtime_matrix_test.ps1" -PreparedProfile -PackageRoot $PackageRoot -InstallRoot $copy.Destination -Observer $Observer -OutputRoot "$OutputRoot/runtime" -Cases "$mode-dosonly0" -Windows95:$is95 -Win95ExitBoundary:$is95 -TimeoutMs 150000 @runtimeOptions
    if($DosFullscreenRoundtrip) {
        $witness="$OutputRoot/runtime/$mode-dosonly0.observer.txt.roundtrip.txt"
        if(!(Test-Path $witness) -or (Get-Content $witness -Raw).Trim() -cne 'window-input=1 fullscreen-text=1 returned-window-input=1 second-fullscreen=1 same-window=1') {throw 'Missing actual DOS window/fullscreen roundtrip'}
        foreach($name in 'WINDOW.BIN','FULL.RUN','RETURN.REQ','RETURN.FOC','RETURN.KEY','EXEC.TXT') {
            $path=Join-Path "$($copy.Destination)/PATCH" $name
            if(!(Test-Path $path)){throw "Missing guest handshake: $name"}
            Copy-Item $path "$OutputRoot/$name"
        }
        $window=[IO.File]::ReadAllBytes("$OutputRoot/WINDOW.BIN")
        if($window.Length -ne 14 -or [BitConverter]::ToUInt16($window,4) -eq 0 -or
           [BitConverter]::ToInt16($window,10) -le [BitConverter]::ToInt16($window,6) -or
           [BitConverter]::ToInt16($window,12) -le [BitConverter]::ToInt16($window,8)) {throw 'Invalid guest window geometry'}
    }
} finally {
    if($DosFullscreenRoundtrip) {
        foreach($name in 'WINDOW.BIN','FULL.RUN','RETURN.REQ','RETURN.FOC','RETURN.KEY','EXEC.TXT') {
            $path=Join-Path "$($copy.Destination)/PATCH" $name
            if(Test-Path $path){Copy-Item $path "$OutputRoot/$name" -Force}
        }
    }
    . "$repo/test/observation/isolated_package_cleanup.ps1"
    Remove-OwnedGuestProbeCopy $copy.Destination $copy.Task $copy.Source
}
