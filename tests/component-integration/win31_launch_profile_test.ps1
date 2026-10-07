param(
    [Parameter(Mandatory)][string]$InstallRoot,
    [Parameter(Mandatory)][string]$ProfileDirectory,
    [string]$BeforeHashes = ''
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
$patch = (Resolve-Path -LiteralPath $ProfileDirectory).Path.TrimEnd('\')
if($patch -ne (Join-Path $root 'PATCH')) {throw 'Profiles must be installation-local PATCH files'}
function Assert-Pif([string]$name, [string]$arguments) {
    $bytes = [IO.File]::ReadAllBytes((Join-Path $patch $name))
    function Field([int]$offset,[int]$length) {[Text.Encoding]::ASCII.GetString($bytes,$offset,$length).Trim([char]0)}
    if((Field 36 63) -ne "$root\WIN.COM" -or (Field 101 64) -ne $root -or (Field 165 64) -ne $arguments) {throw "Wrong main PIF fields: $name"}
    $pos = 369; $nt = $false; $extended = $false
    while($pos -ne 65535) {
        if($pos + 22 -gt $bytes.Length) {throw "Bad PIF extension chain: $name"}
        $sig = Field $pos 16; $data = [BitConverter]::ToUInt16($bytes,$pos+18)
        if($sig -eq 'WINDOWS 386 3.0') {if((Field ($data+40) 64) -ne $arguments) {throw "Wrong extended arguments: $name"}; $extended = $true}
        if($sig -eq 'WINDOWS NT  3.1') {
            if((Field ($data+12) 64) -ne "$patch\CONFIG.NT" -or (Field ($data+76) 64) -ne "$patch\AUTOEXEC.NT") {throw "Wrong profile paths: $name"}
            $nt = $true
        }
        $pos = [BitConverter]::ToUInt16($bytes,$pos+16)
    }
    if(!$nt -or !$extended) {throw "Missing required extensions: $name"}
    $sum = 0; for($i=2;$i -lt 369;$i++) {$sum=($sum+$bytes[$i]) -band 255}
    if($sum -ne $bytes[1]) {throw "Bad PIF checksum: $name"}
}
Assert-Pif 'WINSTD.PIF' '/S'
Assert-Pif 'WIN386.PIF' '/3'
$config = Get-Content -LiteralPath (Join-Path $patch 'CONFIG.NT') -Raw
$auto = Get-Content -LiteralPath (Join-Path $patch 'AUTOEXEC.NT') -Raw
if($config -notmatch '(?m)^dosonly\s*$' -or $auto -match '(?im)^\s*(lh\s+)?[^\r\n]*\\dosx(\.exe)?\s*$') {throw 'Wrong DOS-only/DOSX profile'}
if(!$auto.Contains("SET TEMP=$patch\TEMP") -or !$auto.Contains("$root\SYSTEM")) {throw 'Wrong shared environment'}
foreach($pair in @(@('WINSTD.CMD','WINSTD.PIF'), @('WIN386.CMD','WIN386.PIF'))) {
    $launch = Get-Content -LiteralPath (Join-Path $patch $pair[0]) -Raw
    if(!$launch.Contains("call run16 `"%~dp0$($pair[1])`"")) {throw "Launcher does not use PATH/PIF: $($pair[0])"}
}
foreach($forbidden in @('win.cmd','start.cmd','WIN31.PIF')) {if(Test-Path -LiteralPath (Join-Path $patch $forbidden)) {throw "Ambiguous installed launcher remains: $forbidden"}}
if((Get-FileHash -LiteralPath (Join-Path $root 'SYSTEM\WIN386.EXE')).Hash -ne 'C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8') {throw 'Installed WIN386 is not the checked enhanced candidate'}
if((Get-FileHash -LiteralPath (Join-Path $patch 'WIN386.ORIG')).Hash -ne '6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5') {throw 'Original WIN386 recovery copy is missing'}
if(!(Test-Path -LiteralPath (Join-Path $patch 'WIN386-ADAPTATION.JSON'))) {throw 'Enhanced adaptation provenance is missing'}
if($BeforeHashes) {
    $before = Get-Content -LiteralPath $BeforeHashes -Raw | ConvertFrom-Json
    foreach($p in $before.PSObject.Properties) {if((Get-FileHash -LiteralPath $p.Name).Hash -ne $p.Value) {throw "Existing input changed: $($p.Name)"}}
}
'PASS explicit WINSTD/WIN386 PIFs and launchers, shared profile, and no ambiguous alias'
