param(
    [Parameter(Mandatory)][string]$InstallRoot,
    [string]$OutputDirectory = '',
    [string]$PackagePatch = ''
)
$ErrorActionPreference = 'Stop'

# This is deliberately an installed-PATCH generator. It never modifies the
# default NT profiles, source media, or an existing guest executable.
$root = (Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
if(!$OutputDirectory) {$OutputDirectory = Join-Path $root 'PATCH'}
$output = [IO.Path]::GetFullPath($OutputDirectory).TrimEnd('\')
if(!$PackagePatch) {$PackagePatch = $PSScriptRoot}
$package = (Resolve-Path -LiteralPath $PackagePatch).Path.TrimEnd('\')
if($output -ne (Join-Path $root 'PATCH')) {throw 'Installed launch output must be the installation-local PATCH directory'}

foreach($path in @($root, $output)) {
    if($path -match '[^\x21-\x7e]|["%&|<>^!]') {throw 'Use a short physical DOS-compatible installation path; no drive mapping'}
    if(($path + '\AUTOEXEC.NT').Length -ge 64) {throw 'Path exceeds the PIF field; choose a shorter physical directory'}
}
foreach($name in @('WIN.COM','SYSTEM\DOSX.EXE','SYSTEM\KRNL386.EXE','SYSTEM\WIN386.EXE','WIN31.PIF','SYSTEM.INI')) {
    if(!(Test-Path -LiteralPath (Join-Path $root $name) -PathType Leaf)) {throw "Missing installed Windows 3.1 input: $name"}
}
foreach($name in @('MOUSE31.DRV','WIN31-TEMPLATE.PIF','winstd.cmd.template','win386.cmd.template','WIN386-ADAPTED.EXE','WIN386-ADAPTATION.JSON')) {
    if(!(Test-Path -LiteralPath (Join-Path $package $name) -PathType Leaf)) {throw "Missing package PATCH payload: $name"}
}

New-Item -ItemType Directory -Path $output -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $package 'MOUSE31.DRV') -Destination (Join-Path $output 'MOUSE31.DRV') -Force

# Owner-approved S1 retail-copy adaptation: validate the exact original or
# already-adapted identity, retain the first original in PATCH, then install
# only the checked candidate. No arbitrary executable is accepted here.
$retailHash = '6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5'
$candidateHash = 'C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8'
$targetWin386 = Join-Path $root 'SYSTEM\WIN386.EXE'
$currentHash = (Get-FileHash -LiteralPath $targetWin386).Hash
$candidate = Join-Path $package 'WIN386-ADAPTED.EXE'
if((Get-FileHash -LiteralPath $candidate).Hash -ne $candidateHash) {throw 'Unexpected enhanced adaptation identity'}
if($currentHash -eq $retailHash) {
    $backup = Join-Path $output 'WIN386.ORIG'
    if(!(Test-Path -LiteralPath $backup)) {Copy-Item -LiteralPath $targetWin386 -Destination $backup}
    Copy-Item -LiteralPath $candidate -Destination $targetWin386 -Force
} elseif($currentHash -ne $candidateHash) {throw 'Unsupported installed WIN386 identity'}
Copy-Item -LiteralPath (Join-Path $package 'WIN386-ADAPTATION.JSON') -Destination (Join-Path $output 'WIN386-ADAPTATION.JSON') -Force

# Require exactly one selection instead of silently changing a foreign INI.
$iniPath = Join-Path $root 'SYSTEM.INI'
$ini = [IO.File]::ReadAllText($iniPath, [Text.Encoding]::ASCII)
$boot = [regex]::Match($ini, '(?ims)^\[boot\][^\r\n]*\r?\n(?<body>.*?)(?=^\[|\z)')
if(!$boot.Success -or [regex]::Matches($boot.Groups['body'].Value, '(?im)^mouse\.drv=[^\r\n]*').Count -ne 1) {throw 'SYSTEM.INI has no unambiguous [boot] mouse.drv selection'}
$originalIni = Join-Path $output 'SYSTEM.INI.ORIG'
if(!(Test-Path -LiteralPath $originalIni)) {[IO.File]::WriteAllText($originalIni, $ini, [Text.Encoding]::ASCII)}
$body = [regex]::Replace($boot.Groups['body'].Value, '(?im)^mouse\.drv=[^\r\n]*', "mouse.drv=$output\MOUSE31.DRV")
$updatedIni = $ini.Substring(0, $boot.Groups['body'].Index) + $body + $ini.Substring($boot.Groups['body'].Index + $boot.Groups['body'].Length)
[IO.File]::WriteAllText($iniPath, $updatedIni, [Text.Encoding]::ASCII)

$config = @"
REM Windows 3.1: isolated DOS profile shared by WINSTD and WIN386.
dos=high, umb
device=%SystemRoot%\system32\himem.sys
files=128
dosonly
"@.Replace("`n", "`r`n")
$auto = @"
@echo off
REM Use this installation's SYSTEM\DOSX.EXE; never preload NT/WOW DOSX.
lh %SystemRoot%\system32\mscdexnt.exe
lh %SystemRoot%\system32\redir
SET TEMP=$output\TEMP
SET TMP=$output\TEMP
SET PATH=$root;$root\SYSTEM;%SystemRoot%\system32
"@.Replace("`n", "`r`n")
[IO.File]::WriteAllText((Join-Path $output 'CONFIG.NT'), $config, [Text.Encoding]::ASCII)
[IO.File]::WriteAllText((Join-Path $output 'AUTOEXEC.NT'), $auto, [Text.Encoding]::ASCII)
New-Item -ItemType Directory -Path (Join-Path $output 'TEMP') -Force | Out-Null

$template = [IO.File]::ReadAllBytes((Join-Path $package 'WIN31-TEMPLATE.PIF'))
if($template.Length -lt 0x171) {throw 'Truncated Windows 3.1 PIF template'}

function New-Win31Pif([string]$name, [string]$title, [string]$arguments) {
    $pif = [byte[]]$template.Clone()
    function PutString([int]$offset, [int]$size, [string]$value) {
        $text = [Text.Encoding]::ASCII.GetBytes($value)
        if($text.Length -ge $size -or $offset + $size -gt $pif.Length) {throw "PIF field overflow: $name"}
        [Array]::Clear($pif, $offset, $size); [Array]::Copy($text, 0, $pif, $offset, $text.Length)
    }
    PutString 2 30 $title; PutString 0x24 63 "$root\WIN.COM"; PutString 0x65 64 $root; PutString 0xa5 64 $arguments
    $position = 0x171; $seen = @{}; $foundNt = $false; $found386 = $false
    while($position -ne 0xffff) {
        if($seen.ContainsKey($position) -or $position + 22 -gt $pif.Length) {throw "Invalid PIF extension chain: $name"}
        $seen[$position] = $true
        $signature = [Text.Encoding]::ASCII.GetString($pif, $position, 16).Trim([char]0)
        $data = [BitConverter]::ToUInt16($pif, $position + 18); $length = [BitConverter]::ToUInt16($pif, $position + 20)
        if($data + $length -gt $pif.Length) {throw "Truncated PIF extension: $name"}
        if($signature -eq 'WINDOWS 386 3.0') {if($length -lt 104) {throw "Short 386 PIF extension: $name"}; PutString ($data + 40) 64 $arguments; $found386 = $true}
        if($signature -eq 'WINDOWS NT  3.1') {if($length -lt 140) {throw "Short NT PIF extension: $name"}; PutString ($data + 12) 64 "$output\CONFIG.NT"; PutString ($data + 76) 64 "$output\AUTOEXEC.NT"; $foundNt = $true}
        $position = [BitConverter]::ToUInt16($pif, $position + 16)
    }
    if(!$foundNt -or !$found386) {throw "Missing PIF extension: $name"}
    $sum = 0; for($i = 2; $i -lt 0x171; $i++) {$sum = ($sum + $pif[$i]) -band 255}; $pif[1] = [byte]$sum
    [IO.File]::WriteAllBytes((Join-Path $output $name), $pif)
}

New-Win31Pif 'WINSTD.PIF' 'Windows 3.1 Standard' '/S'
New-Win31Pif 'WIN386.PIF' 'Windows 3.1 386 Enhanced' '/3'
Copy-Item -LiteralPath (Join-Path $package 'winstd.cmd.template') -Destination (Join-Path $output 'WINSTD.CMD') -Force
Copy-Item -LiteralPath (Join-Path $package 'win386.cmd.template') -Destination (Join-Path $output 'WIN386.CMD') -Force
"Configured installed PATCH at ${output}: WINSTD and WIN386 share CONFIG.NT/AUTOEXEC.NT; the checked recoverable WIN386 adaptation is installed."
