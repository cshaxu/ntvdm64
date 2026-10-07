param([string]$InstallRoot,[switch]$ConfigureAfterFailure)
$ErrorActionPreference='Stop'
$patch=$PSScriptRoot;$media=Split-Path -Parent $patch
$launcher=Get-Command run16 -CommandType Application -ErrorAction Stop|Select-Object -First 1
$old=Get-Location;$oldTemp=$env:TEMP;$oldTmp=$env:TMP;$result=1
try {
    # Original compressed media runs in place. PATCH owns only its TEMP folder.
    $env:TEMP=Join-Path $patch 'TEMP';$env:TMP=$env:TEMP;New-Item -ItemType Directory -Path $env:TEMP -Force|Out-Null
    Set-Location -LiteralPath $media
    & $launcher.Source (Join-Path $media 'SETUP.EXE');$result=$LASTEXITCODE
    [IO.File]::WriteAllText((Join-Path $patch 'setup-result.txt'),"run16_exit=$result`r`n",[Text.Encoding]::ASCII)
    $configure=($result -eq 0 -or $ConfigureAfterFailure)
    if(!$InstallRoot){$InstallRoot=Read-Host "Setup returned $result. If installation completed, enter its actual directory; otherwise press Enter to skip";$configure=!!$InstallRoot}
    if($configure -and $InstallRoot){& (Join-Path $patch 'configure-launch.ps1') -InstallRoot $InstallRoot -PackagePatch $patch;Write-Host "Configured $InstallRoot\PATCH."}
    else {Write-Warning 'Installed profiles were not generated; see setup-result.txt.'}
} finally {Set-Location -LiteralPath $old.Path;$env:TEMP=$oldTemp;$env:TMP=$oldTmp}
exit $result
