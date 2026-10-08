param(
    [string]$MediaRoot,
    [string]$ReleaseRoot = '',
    [Parameter(Mandatory)][string]$Patchset,
    [Parameter(Mandatory)][string]$AdaptedWin386,
    [Parameter(Mandatory)][string]$AdaptedKrnl386,
    [Parameter(Mandatory)][string]$RetailWin386,
    [Parameter(Mandatory)][string]$RetailKrnl386
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
if(!$MediaRoot) {$MediaRoot = Read-Host 'Original Windows 3.1 installation media directory'}
if(!$MediaRoot) {throw 'No installation media directory supplied'}
$media = (Resolve-Path -LiteralPath $MediaRoot).Path.TrimEnd('\')
if((Get-Item -LiteralPath $media).Attributes -band [IO.FileAttributes]::ReparsePoint) {throw 'Media directory must not be a link'}
if(!$ReleaseRoot) {$ReleaseRoot = Join-Path $repo 'assets/release'}
$release = (Resolve-Path -LiteralPath $ReleaseRoot).Path
foreach($name in @('SETUP.EXE','DOSX.EX_','KRNL386.EX_','WIN386.EX_')) {
    if(!(Test-Path -LiteralPath (Join-Path $media $name) -PathType Leaf)) {throw "Missing original Win3.1 media/input: $name"}
}
$manifest = Get-Content -LiteralPath (Join-Path $release 'addon-manifest.json') -Raw | ConvertFrom-Json
$entry = @($manifest.drivers | Where-Object {$_.id -eq 'win31'})
if($manifest.schema -ne 1 -or $entry.Count -ne 1 -or $entry[0].file -ne 'MOUSE31.DRV') {throw 'Invalid release Win3.1 mouse-driver manifest'}
$driver = Join-Path $release $entry[0].file
if(!(Test-Path -LiteralPath $driver) -or (Get-FileHash -LiteralPath $driver).Hash -ne $entry[0].sha256) {throw 'Released Win3.1 mouse-driver hash mismatch'}

function HashBytes([byte[]]$bytes) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try {return ([BitConverter]::ToString($sha.ComputeHash($bytes))).Replace('-','')}
    finally {$sha.Dispose()}
}
$files = @{}
$files['MOUSE31.DRV'] = [IO.File]::ReadAllBytes($driver)
$files['PATCHSET.EXE'] = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $Patchset).Path)
$files['README.TXT'] = [IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'README.txt'))
$files['KRNL386-ADAPTED.EXE'] = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $AdaptedKrnl386).Path)
$files['KRNL386-RETAIL.EXE'] = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $RetailKrnl386).Path)
$files['SETUP.CMD'] = [IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'setup.cmd.template'))
$files['WIN386-ADAPTED.EXE'] = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $AdaptedWin386).Path)
$files['WIN386-RETAIL.EXE'] = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $RetailWin386).Path)

$patch = Join-Path $media 'PATCH'; $owned = @{}
if(Test-Path -LiteralPath $patch) {
    if((Get-Item -LiteralPath $patch).Attributes -band [IO.FileAttributes]::ReparsePoint) {throw 'PATCH must not be a link'}
    $record = Join-Path $patch 'addon-files.json'
    if(Test-Path -LiteralPath $record) {
        $prior = Get-Content -LiteralPath $record -Raw | ConvertFrom-Json
        if($prior.id -ne 'win31') {throw 'PATCH belongs to a different add-on'}
        foreach($item in $prior.files) {$owned[$item.name] = $item.sha256}
    }
    foreach($item in @(Get-ChildItem -LiteralPath $patch -Recurse -File)) {
        $relative = $item.FullName.Substring($patch.Length + 1)
        # Runtime Setup scratch is expressly package-owned.  It is not part
        # of the authored payload and must not make a failed Setup impossible
        # to repair/repackage; every other unknown PATCH file remains guarded.
        if($relative -like 'TEMP\*') {continue}
        if($relative -ne 'addon-files.json' -and !$owned.ContainsKey($relative)) {
            throw "Preserve unknown existing PATCH payload: $relative"
        }
    }
}
foreach($name in $files.Keys) {
    $target = Join-Path $patch $name
    if(Test-Path -LiteralPath $target) {
        if((Get-Item -LiteralPath $target).Attributes -band [IO.FileAttributes]::ReparsePoint) {throw "Linked PATCH file: $name"}
        $hash = (Get-FileHash -LiteralPath $target).Hash
        if($hash -ne (HashBytes $files[$name]) -and $hash -ne $owned[$name]) {throw "Preserve changed PATCH file: $name"}
    }
}
New-Item -ItemType Directory -Path $patch -Force | Out-Null
$records = @(foreach($name in ($files.Keys | Sort-Object)) {
    [IO.File]::WriteAllBytes((Join-Path $patch $name), $files[$name])
    [ordered]@{name=$name;sha256=(HashBytes $files[$name])}
})
# The package manifest belongs in the build evidence, not in the user-facing
# PATCH directory.  It is deliberately not emitted beside the installer.
if(Test-Path -LiteralPath (Join-Path $patch 'addon-files.json')) {Remove-Item -LiteralPath (Join-Path $patch 'addon-files.json') -Force}
Write-Host "Prepared $patch; original media remain unchanged."
Write-Host 'Run PATCH\SETUP.CMD. After original Setup, enter the actual installation directory when prompted.'
