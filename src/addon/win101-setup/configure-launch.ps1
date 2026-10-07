param([Parameter(Mandatory)][ValidateSet('Setup','Installed')][string]$Mode,
      [string]$InstallRoot)
$ErrorActionPreference='Stop'
$package=$PSScriptRoot
$template=Join-Path $package 'WIN31-TEMPLATE.PIF'
if($Mode -eq 'Installed' -and !$InstallRoot){throw 'Supply the directory selected in original Setup'}
$root=if($Mode -eq 'Installed'){[IO.Path]::GetFullPath($InstallRoot).TrimEnd('\')}else{$package}
$output=if($Mode -eq 'Setup'){Join-Path $package 'WORK'}else{Join-Path $root 'PATCH'}
# PIF paths are bounded DOS/ASCII strings; do not silently replace characters.
foreach($path in @($root,$output)) {
    if($path -match '[^\x20-\x7e]' -or $path -match '[\s"%&|<>^!]') {
        throw 'Use a physical DOS-compatible path without spaces or shell metacharacters'
    }
    if(($path+'\AUTOEXEC.NT').Length -ge 64){throw 'Path exceeds the PIF field; choose a shorter physical directory'}
}
if($Mode -eq 'Installed') {
    foreach($name in @('WIN.COM','WIN100.BIN','WIN100.OVL')) {
        if(!(Test-Path "$root/$name")){throw "Setup has not produced $name"}
    }
}
$profile=$output;$temp=Join-Path $output 'TEMP'
New-Item -ItemType Directory -Path $output -Force|Out-Null
if($Mode -eq 'Setup') {
    # Keep root media immutable. Original Setup operates on an owned copy.
    Get-ChildItem -LiteralPath (Split-Path $package -Parent) -File | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination $output -Force
    }
    Copy-Item -LiteralPath "$package/MOUSE.DRV","$package/SETVER.EXE" -Destination $output -Force
}
if($Mode -eq 'Installed') {
    New-Item -ItemType Directory -Path $temp -Force|Out-Null
    Copy-Item -LiteralPath "$package/SETVER.EXE" -Destination "$profile/SETVER.EXE" -Force
}
$pif=[IO.File]::ReadAllBytes($template)
if($pif.Length -lt 391){throw 'Truncated reference PIF'}
function PutString([int]$offset,[int]$size,[string]$value) {
    $text=[Text.Encoding]::ASCII.GetBytes($value)
    if($text.Length -ge $size -or $offset+$size -gt $pif.Length){throw 'PIF field overflow'}
    [Array]::Clear($pif,$offset,$size);[Array]::Copy($text,0,$pif,$offset,$text.Length)
}
$cwd=if($Mode -eq 'Setup'){$output}else{$root}
$program=if($Mode -eq 'Setup'){"$output\SETUP.EXE"}else{"$root\WIN.COM"}
$config="$output\CONFIG.NT"
$autoexec="$output\AUTOEXEC.NT"
PutString 2 30 "Windows 1.01 - $Mode"
PutString 0x24 63 $program
PutString 0x65 64 $cwd
PutString 0xa5 64 ''
# Original packed PIFEXTHDR and W386PIF30/PROPNT31 offsets from pif.h.
$position=0x171;$seen=@{};$foundNt=$false;$found386=$false
while($position -ne 0xffff) {
    if($seen.ContainsKey($position) -or $position+22 -gt $pif.Length){throw 'Invalid PIF extension chain'}
    $seen[$position]=$true
    $signature=[Text.Encoding]::ASCII.GetString($pif,$position,16).Trim([char]0)
    $data=[BitConverter]::ToUInt16($pif,$position+18)
    $length=[BitConverter]::ToUInt16($pif,$position+20)
    if($data+$length -gt $pif.Length){throw 'Truncated PIF extension'}
    if($signature -eq 'WINDOWS 386 3.0') {
        if($length -lt 104){throw 'Short 386 extension'}
        PutString ($data+40) 64 ''; $found386=$true
    }
    if($signature -eq 'WINDOWS NT  3.1') {
        if($length -lt 140){throw 'Short NT extension'}
        PutString ($data+12) 64 $config
        PutString ($data+76) 64 $autoexec; $foundNt=$true
    }
    $position=[BitConverter]::ToUInt16($pif,$position+16)
}
if(!$foundNt -or !$found386){throw 'Reference PIF is missing its accepted extensions'}
$pif[1]=0;$sum=0;for($i=2;$i -lt 0x171;$i++){$sum=($sum+$pif[$i]) -band 255};$pif[1]=[byte]$sum
$name=if($Mode -eq 'Setup'){'SETUP.PIF'}else{'WIN101.PIF'}
[IO.File]::WriteAllBytes((Join-Path $output $name),$pif)
$configText=@'
REM Windows 1.01: dedicated DOS profile, no NT/WOW DOSX.
dos=high, umb
device=%SystemRoot%\system32\himem.sys
files=128
dosonly
'@
$setver="$output\SETVER.EXE"
$configText+="`r`ndevice=$setver`r`n"
$tempDos="$output\TEMP"
$autoText="@echo off`r`nSET TEMP=$tempDos`r`nSET TMP=$tempDos`r`nSET PATH=$cwd;%SystemRoot%\system32`r`n"
[IO.File]::WriteAllText("$profile/CONFIG.NT",$configText.Replace("`r`n","`n").Replace("`n","`r`n"),[Text.Encoding]::ASCII)
[IO.File]::WriteAllText("$profile/AUTOEXEC.NT",$autoText,[Text.Encoding]::ASCII)
if($Mode -eq 'Installed') {
    Copy-Item -LiteralPath "$package/win.cmd.template" -Destination "$output/win.cmd" -Force
    # Original Setup copies loose work-directory files into its destination,
    # including our temporary profiles. Replace those with installed paths.
    Copy-Item -LiteralPath "$profile/CONFIG.NT","$profile/AUTOEXEC.NT" -Destination $root -Force
    [IO.File]::WriteAllBytes((Join-Path $root 'WIN.PIF'),$pif)
    if(Test-Path -LiteralPath "$root/SETUP.EXE") {
        PutString 2 30 'Windows 1.01 - Setup'
        PutString 0x24 63 "$root\SETUP.EXE"
        $sum=0;for($i=2;$i -lt 0x171;$i++){$sum=($sum+$pif[$i]) -band 255};$pif[1]=[byte]$sum
        [IO.File]::WriteAllBytes((Join-Path $root 'SETUP.PIF'),$pif)
    }
}
"Configured $Mode PIF/profile; run16 is resolved through PATH"
