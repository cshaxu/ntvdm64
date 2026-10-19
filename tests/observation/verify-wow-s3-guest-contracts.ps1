[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$EvidenceRoot,
    [string]$ManifestPath,
    [string]$CommonDialogIdentityHost
)
# Run as an owned hidden background process. No production or original guest
# is compiled here: only independently authored disposable NE witnesses.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$out=[IO.Path]::GetFullPath($EvidenceRoot)
$package=(Resolve-Path -LiteralPath $PackageRoot).Path
$observerPath=(Resolve-Path -LiteralPath $Observer).Path
if(!$CommonDialogIdentityHost){$CommonDialogIdentityHost="$repo/build/.ninja/msvc-x86-release/message-probes/CDLHOST.dll"}
$CommonDialogIdentityHost=(Resolve-Path -LiteralPath $CommonDialogIdentityHost).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $out)) { throw 'Require fresh build-owned evidence' }
$null=New-Item -ItemType Directory -Path $out
$shell=(Get-Process -Id $PID).Path
try {
    $sourceRoots=@('src/wow32-dll','src/mvdm/wow32',
        'src/opennt-host/windows/core/ntuser','src/ntw16s-dll',
        'tests/observation/verify-win16-hook-probe.ps1',
        'tests/observation/verify-wow-s3-guest-contracts.ps1',
        'tests/observation/win16_controls_probe.asm',
        'tests/observation/win16_dialog_probe.asm',
        'tests/observation/win16_hook_probe.asm',
        'tests/observation/win16_common_dialog_probe.asm')
    $sourceRoots+='tests/observation/wow_common_dialog_identity.c'
    $paths=@(& git -C $repo ls-files -co --exclude-standard -- @sourceRoots)|Sort-Object -Unique
    if($LASTEXITCODE){throw 'Source enumeration failed'}
    $paths|ForEach-Object{Get-FileHash -LiteralPath (Join-Path $repo $_)}|
        Export-Csv -NoTypeInformation -LiteralPath "$out/source-sha256.csv"
    if(!$ManifestPath){$ManifestPath="$repo/assets/release/release-manifest.json"}
    $ManifestPath=(Resolve-Path -LiteralPath $ManifestPath).Path
    $manifest=Get-Content $ManifestPath -Raw|ConvertFrom-Json
    if(@($manifest).Count -ne 13){throw 'Require coherent13 package'}
    foreach($row in $manifest){
        if((Get-FileHash -LiteralPath "$package/system32/$($row.name)").Hash -ne $row.sha256){
            throw "Package differs from selected release: $($row.name)"
        }
    }
    Get-FileHash $ManifestPath|Export-Csv -NoTypeInformation "$out/manifest-sha256.csv"
    Get-ChildItem -LiteralPath $package -File -Recurse|Get-FileHash|
        Export-Csv -NoTypeInformation -LiteralPath "$out/package-sha256.csv"
    Get-FileHash -LiteralPath $observerPath|Export-Csv -NoTypeInformation "$out/observer-sha256.csv"
    Get-FileHash -LiteralPath $CommonDialogIdentityHost|Export-Csv -NoTypeInformation "$out/identity-host-sha256.csv"
    $rows=@()
    foreach($case in @('controls','dialog','common-dialog','hook')){
        foreach($mode in @('Off','Win16','All')){
            $name="$case-$mode"
            $line='-NoProfile -ExecutionPolicy Bypass -File "'+$PSScriptRoot+'/verify-win16-hook-probe.ps1" -PackageRoot "'+$package+'" -Observer "'+$observerPath+'" -BuildRoot "'+$out+'/'+$name+'" -Case '+$case+' -Win16Style '+$mode
            if($case -eq 'common-dialog'){$line+=' -CommonDialogRounds 16 -CommonDialogIdentityHost "'+$CommonDialogIdentityHost+'"'}
            $p=Start-Process $shell -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$name.stdout.txt" -RedirectStandardError "$out/$name.stderr.txt"
            $null=$p.Handle
            $start=$p.StartTime.ToUniversalTime()
            @{pid=$p.Id;start=$start.ToString('o');command=$line;timeoutSeconds=180}|
                ConvertTo-Json|Set-Content "$out/$name.process.json"
            Write-Output "$name PID=$($p.Id)"
            if(!$p.WaitForExit(180000)){
                & taskkill.exe /PID $p.Id /T /F | Out-Host
                throw "Owned deadline: $name"
            }
            $p.WaitForExit();$code=$p.ExitCode
            $code|Set-Content "$out/$name.exit.txt"
            $rows+=@{name=$name;exit=$code;seconds=([DateTime]::UtcNow-$start).TotalSeconds}
            $rows|ConvertTo-Json|Set-Content "$out/results.json"
            $p.Dispose()
            # A terminal verifier restores its exact isolated package before
            # the next serial case. Preserve all failed rows, not fail-fast.
        }
    }
    foreach($list in @('source-sha256.csv','package-sha256.csv','observer-sha256.csv','manifest-sha256.csv','identity-host-sha256.csv')){
        foreach($input in Import-Csv -LiteralPath "$out/$list"){
            if((Get-FileHash -LiteralPath $input.Path).Hash -ne $input.Hash){throw "Input changed: $($input.Path)"}
        }
    }
    if(@($rows|Where-Object exit -ne 0).Count){throw 'Guest contract group contains failed cases'}
    '0'|Set-Content "$out/exit.txt"
}catch{
    $_|Out-String|Set-Content "$out/failure.txt"
    '1'|Set-Content "$out/exit.txt"
    exit 1
}
