param([string]$InstallRoot,[switch]$ConfigureAfterFailure)
$ErrorActionPreference='Stop'
$patch=$PSScriptRoot;$media=Split-Path -Parent $patch
$launcher=Get-Command run16 -CommandType Application -ErrorAction Stop|Select-Object -First 1
$old=Get-Location;$oldTemp=$env:TEMP;$oldTmp=$env:TMP;$result=1

# SETUP.EXE is a Win16 NE program.  It needs the normal Win16 route and an
# explicit package-local profile; launching its bare EXE provides neither.
# This PIF is generated at runtime because its fields intentionally contain
# the caller-selected package location.
function New-SetupPif {
    $template=[IO.File]::ReadAllBytes((Join-Path $patch 'WIN31-TEMPLATE.PIF'))
    if($template.Length -lt 0x171) {throw 'Truncated Windows 3.1 PIF template'}
    $pif=[byte[]]$template.Clone()
    function PutString([int]$offset,[int]$length,[string]$value) {
        $bytes=[Text.Encoding]::ASCII.GetBytes($value)
        if($bytes.Length -ge $length -or $offset+$length -gt $pif.Length) {throw 'Setup PIF field overflow; move the package to a shorter path'}
        [Array]::Clear($pif,$offset,$length);[Array]::Copy($bytes,0,$pif,$offset,$bytes.Length)
    }
    PutString 2 30 'Windows 3.1 Setup';PutString 0x24 63 (Join-Path $media 'SETUP.EXE');PutString 0x65 64 $media;PutString 0xa5 64 ''
    $position=0x171;$seen=@{};$nt=$false;$w386=$false
    while($position -ne 0xffff) {
        if($seen.ContainsKey($position) -or $position+22 -gt $pif.Length) {throw 'Invalid Setup PIF extension chain'}
        $seen[$position]=$true;$signature=[Text.Encoding]::ASCII.GetString($pif,$position,16).Trim([char]0)
        $data=[BitConverter]::ToUInt16($pif,$position+18);$length=[BitConverter]::ToUInt16($pif,$position+20)
        if($data+$length -gt $pif.Length) {throw 'Truncated Setup PIF extension'}
        if($signature -eq 'WINDOWS 386 3.0') {PutString ($data+40) 64 ''; $w386=$true}
        if($signature -eq 'WINDOWS NT  3.1') {PutString ($data+12) 64 (Join-Path $patch 'CONFIG.NT');PutString ($data+76) 64 (Join-Path $patch 'AUTOEXEC.NT');$nt=$true}
        $position=[BitConverter]::ToUInt16($pif,$position+16)
    }
    if(!$nt -or !$w386) {throw 'Setup PIF is missing required extensions'}
    $sum=0;for($i=2;$i -lt 0x171;$i++) {$sum=($sum+$pif[$i])-band 255};$pif[1]=[byte]$sum
    [IO.File]::WriteAllBytes((Join-Path $patch 'SETUP.PIF'),$pif)
}
try {
    # Original compressed media runs in place. PATCH owns only generated PIF,
    # profile and TEMP files.
    $env:TEMP=Join-Path $patch 'TEMP';$env:TMP=$env:TEMP;New-Item -ItemType Directory -Path $env:TEMP -Force|Out-Null
    [IO.File]::WriteAllText((Join-Path $patch 'CONFIG.NT'),"dos=high, umb`r`ndevice=%SystemRoot%\system32\himem.sys`r`nfiles=128`r`ndosonly`r`n",[Text.Encoding]::ASCII)
    [IO.File]::WriteAllText((Join-Path $patch 'AUTOEXEC.NT'),"@echo off`r`nSET TEMP=$env:TEMP`r`nSET TMP=$env:TEMP`r`nSET PATH=$media;%SystemRoot%\system32`r`n",[Text.Encoding]::ASCII)
    New-SetupPif
    Set-Location -LiteralPath $media
    & $launcher.Source (Join-Path $patch 'SETUP.PIF');$result=$LASTEXITCODE
    [IO.File]::WriteAllText((Join-Path $patch 'setup-result.txt'),"run16_launch_exit=$result`r`n",[Text.Encoding]::ASCII)
    if($result -eq 0 -and !$InstallRoot) {Read-Host 'Original Setup is running independently. Complete and close it, then press Enter here before postconfiguration' | Out-Null}
    $configure=($result -eq 0 -or $ConfigureAfterFailure)
    if(!$InstallRoot){$InstallRoot=Read-Host "Setup launch returned $result. If installation completed, enter its actual directory; otherwise press Enter to skip";$configure=!!$InstallRoot}
    if($configure -and $InstallRoot){& (Join-Path $patch 'configure-launch.ps1') -InstallRoot $InstallRoot -PackagePatch $patch;Write-Host "Configured $InstallRoot\PATCH."}
    else {Write-Warning 'Installed profiles were not generated; see setup-result.txt.'}
} finally {Set-Location -LiteralPath $old.Path;$env:TEMP=$oldTemp;$env:TMP=$oldTmp}
exit $result
