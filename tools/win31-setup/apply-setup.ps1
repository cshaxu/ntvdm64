param(
    [string]$MediaRoot,
    [string]$ReleaseRoot = '',
    [Parameter(Mandatory)][string]$PifTemplate,
    [string]$AdaptedWin386 = '',
    [string]$AdaptationManifest = ''
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
$files['WIN31-TEMPLATE.PIF'] = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $PifTemplate).Path)
foreach($name in @('configure-launch.ps1','run-setup.ps1','winstd.cmd.template','win386.cmd.template','README.txt')) {
    $files[$name] = [IO.File]::ReadAllBytes((Join-Path $PSScriptRoot $name))
}
$files['SETUP.CMD'] = [IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'setup.cmd.template'))
if($AdaptedWin386 -or $AdaptationManifest) {
    if(!$AdaptedWin386 -or !$AdaptationManifest) {throw 'Supply both checked enhanced adaptation files or neither'}
    $files['WIN386-ADAPTED.EXE'] = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $AdaptedWin386).Path)
    $files['WIN386-ADAPTATION.JSON'] = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $AdaptationManifest).Path)
}

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
[ordered]@{id='win31';releaseDriver=$entry[0];files=$records} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $patch 'addon-files.json') -Encoding UTF8
Write-Host "Prepared $patch; original media remain unchanged."
Write-Host 'Run PATCH\SETUP.CMD. After original Setup, enter the actual installation directory when prompted.'
