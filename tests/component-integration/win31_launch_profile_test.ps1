param(
    [Parameter(Mandatory)][string]$InstallRoot,
    [Parameter(Mandatory)][string]$ProfileDirectory,
    [string]$BeforeHashes = '',
    [string]$ExpectedKernelHash = '75E931AE000FB7C70CF686DA6F7D3FCB032FDC0E0A4E3187ED68179B85E2CD4A',
    [bool]$ExpectDosOnly = $false,
    [switch]$SetupMode
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
$patch = (Resolve-Path -LiteralPath $ProfileDirectory).Path.TrimEnd('\')
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
if($patch -ne (Join-Path $root 'PATCH')) {throw 'Profiles must be installation-local PATCH files'}
function Assert-Pif([string]$name, [string]$arguments) {
    $bytes = [IO.File]::ReadAllBytes((Join-Path $patch $name))
    function Field([int]$offset,[int]$length) {[Text.Encoding]::ASCII.GetString($bytes,$offset,$length).Trim([char]0)}
    if((Field 36 63) -ne "$root\WIN.COM" -or (Field 101 64) -ne $root -or (Field 165 64) -ne $arguments) {throw "Wrong main PIF fields: $name"}
    if(($bytes[2] -band 0x10) -eq 0) {throw "Synchronous launch PIF must set CloseOnExit: $name"}
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
    $shown = & (Join-Path $repo 'assets/release/PIF.EXE') show (Join-Path $patch $name)
    if($LASTEXITCODE -ne 0) {throw "PIF.EXE rejected $name"}
    $text = $shown -join "`n"
    foreach($expected in @('W386.MAX_XMS_KB=65535','W386.FLAGS=0x00001003','NT31.FLAGS=0x00000001')) {
        if(!$text.Contains($expected)) {throw "Missing legacy launch-profile field $expected in $name"}
    }
}
Assert-Pif 'WINSTD.PIF' '/S'
Assert-Pif 'WIN386.PIF' '/3'
$config = Get-Content -LiteralPath (Join-Path $patch 'CONFIG.NT') -Raw
$auto = Get-Content -LiteralPath (Join-Path $patch 'AUTOEXEC.NT') -Raw
if(($config -match '(?im)^\s*dosonly\s*$') -ne $ExpectDosOnly -or $auto -match '(?im)^\s*(lh\s+)?[^\r\n]*\\dosx(\.exe)?\s*$') {throw 'Wrong DOS-only/DOSX profile'}
if((!$SetupMode -and (!$auto.Contains('lh %SystemRoot%\system32\mscdexnt.exe') -or !$auto.Contains('lh %SystemRoot%\system32\redir'))) -or
   $auto.IndexOf("SET TEMP=$patch\TEMP",[StringComparison]::OrdinalIgnoreCase) -lt 0 -or
   $auto.IndexOf("SET TMP=$patch\TEMP",[StringComparison]::OrdinalIgnoreCase) -lt 0 -or
   $auto.IndexOf("$root\SYSTEM",[StringComparison]::OrdinalIgnoreCase) -lt 0) {throw 'Wrong shared environment'}
if(!(Test-Path -LiteralPath (Join-Path $patch 'TEMP') -PathType Container)) {throw 'Missing launch-local TEMP directory'}
foreach($pair in @(@('WINSTD.CMD','WINSTD.PIF'), @('WIN386.CMD','WIN386.PIF'))) {
    $launch = Get-Content -LiteralPath (Join-Path $patch $pair[0]) -Raw
    if(!$launch.Contains("call run16 `"%~dp0$($pair[1])`"")) {throw "Launcher does not use PATH/PIF: $($pair[0])"}
    if((!$SetupMode -and !$launch.Contains('where /q run16')) -or !$launch.Contains('set "TEMP=%~dp0TEMP"') -or !$launch.Contains('mkdir "%TEMP%"')) {throw "Launcher does not establish its local runtime prerequisites: $($pair[0])"}
}
foreach($forbidden in @('win.cmd','start.cmd','WIN31.PIF')) {if(Test-Path -LiteralPath (Join-Path $patch $forbidden)) {throw "Ambiguous installed launcher remains: $forbidden"}}
if((Get-FileHash -LiteralPath (Join-Path $root 'SYSTEM\WIN386.EXE')).Hash -ne 'C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8') {throw 'Installed WIN386 is not the checked enhanced candidate'}
$winBackup=if($SetupMode){Join-Path $patch 'WIN386.ORIG'}else{Join-Path $root 'SYSTEM\WIN386.EXE.BAK'}
if((Get-FileHash -LiteralPath $winBackup).Hash -ne '6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5') {throw 'Original WIN386 backup is missing'}
if((Get-FileHash -LiteralPath (Join-Path $root 'SYSTEM\KRNL386.EXE')).Hash -ne $ExpectedKernelHash) {throw 'Installed KRNL386 is not the checked standard candidate'}
$krnlBackup=if($SetupMode){Join-Path $patch 'KRNL386.ORIG'}else{Join-Path $root 'SYSTEM\KRNL386.EXE.BAK'}
if((Get-FileHash -LiteralPath $krnlBackup).Hash -ne 'FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980') {throw 'Original KRNL386 backup is missing'}
if(!$SetupMode) {
 if(!(Test-Path -LiteralPath (Join-Path $root 'SYSTEM\MOUSE.DRV.BAK') -PathType Leaf)) {throw 'Original mouse-driver adjacent backup is missing'}
 foreach($legacy in @('WIN386.ORIG','KRNL386.ORIG','MOUSE.DRV.ORIG')) {if(Test-Path -LiteralPath (Join-Path $patch $legacy)) {throw "Legacy PATCH recovery copy remains: $legacy"}}
}
foreach($forbidden in @('WIN386-ADAPTATION.JSON','WIN31-TEMPLATE.PIF','configure-launch.ps1','run-setup.ps1','addon-files.json')) {
    if(Test-Path -LiteralPath (Join-Path $patch $forbidden)) {throw "Installed PATCH retains non-runtime payload: $forbidden"}
}
if($BeforeHashes) {
    $before = Get-Content -LiteralPath $BeforeHashes -Raw | ConvertFrom-Json
    foreach($p in $before.PSObject.Properties) {if((Get-FileHash -LiteralPath $p.Name).Hash -ne $p.Value) {throw "Existing input changed: $($p.Name)"}}
}
'PASS explicit WINSTD/WIN386 PIFs and launchers, shared profile, CMD-owned recovery, and no duplicate tool payload'
