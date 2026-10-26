param(
    [string]$PifBuild='',
    [string]$HashBuild='',
    [string]$GrpBuild='',
    [string]$LnkBuild=''
)

$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$items=@(
    @{id='pif'; file='PIF.EXE'; build=$PifBuild; source='src/addon/pif/pif.c'},
    @{id='hash';file='HASH.EXE';build=$HashBuild;source='src/addon/hash/hash.c'},
    @{id='grp'; file='GRP.EXE'; build=$GrpBuild; source='src/addon/grp/grp.c'},
    @{id='lnk'; file='LNK.EXE'; build=$LnkBuild; source='src/addon/lnk/lnk.c'}
) | Where-Object build
if(!@($items).Count){throw 'At least one utility build is required'}
$manifest=@()
foreach($item in $items) {
    $build=(Resolve-Path -LiteralPath $item.build).Path
    $image=if(Test-Path -LiteralPath $build -PathType Leaf) { $build } else { Join-Path $build $item.file }
    if(!$image.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)) {
        throw 'Utility build must be repository-owned'
    }
    if(!(Test-Path -LiteralPath $image -PathType Leaf)) { throw "Missing $($item.file)" }
    $bytes=[IO.File]::ReadAllBytes($image)
    if($bytes.Length -lt 64 -or [BitConverter]::ToUInt16($bytes,0) -ne 0x5A4D) { throw "Invalid PE: $($item.file)" }
    $pe=[BitConverter]::ToInt32($bytes,0x3C)
    if($pe -lt 0 -or $pe+6 -ge $bytes.Length -or [BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x8664) { throw "Not AMD64: $($item.file)" }
    $manifest += [ordered]@{id=$item.id;file=$item.file;architecture='amd64';sha256=(Get-FileHash -LiteralPath $image).Hash;source=$item.source}
}
foreach($item in $items) {
    $build=(Resolve-Path -LiteralPath $item.build).Path
    $image=if(Test-Path -LiteralPath $build -PathType Leaf) { $build } else { Join-Path $build $item.file }
    Copy-Item -LiteralPath $image -Destination (Join-Path $repo ('assets/release/'+$item.file)) -Force
}
$addonPath=Join-Path $repo 'assets/release/addon-manifest.json'
$prior=Get-Content -LiteralPath $addonPath -Raw|ConvertFrom-Json
if($prior.schema -ne 1 -or $null -eq $prior.drivers){throw 'Invalid existing add-on manifest'}
$updatedIds=@($manifest|ForEach-Object id)
$utilities=@($prior.utilities|Where-Object {$_.id -notin $updatedIds})+@($manifest)
[ordered]@{schema=1;drivers=@($prior.drivers);utilities=$utilities}|ConvertTo-Json -Depth 6|Set-Content -LiteralPath $addonPath -Encoding UTF8
'PASS selected AMD64 add-on utilities published; other utility/driver entries retained'
