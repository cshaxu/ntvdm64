param([Parameter(Mandatory)][string]$Win101Build,
      [Parameter(Mandatory)][string]$Win31Build,
      [ValidateSet('candidate','verified')][string]$Win31Acceptance='candidate')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$drivers=@()
foreach($item in @(
    @{id='win101';build=$Win101Build;file='MOUSE101.DRV';source='src/addon/mouse101/mouse101.asm';acceptance='verified'},
    @{id='win31';build=$Win31Build;file='MOUSE31.DRV';source='src/addon/mouse31/mouse31.asm';acceptance=$Win31Acceptance}
)) {
    $root=(Resolve-Path -LiteralPath $item.build).Path
    if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Driver build must be repository-owned'}
    $input=Join-Path $root $item.file
    $build=Get-Content -LiteralPath (Join-Path $root 'build.json') -Raw|ConvertFrom-Json
    $sha=(Get-FileHash -LiteralPath $input).Hash
    $sourceSha=(Get-FileHash -LiteralPath (Join-Path $repo $item.source)).Hash
    if($sha -ne $build.driverSha256 -or $sourceSha -ne $build.sourceSha256){throw "Unmatched driver/source identity: $($item.id)"}
    $drivers+=,[ordered]@{id=$item.id;file=$item.file;sha256=$sha;source=$item.source;
        sourceSha256=$sourceSha;acceptance=$item.acceptance}
}
# All identities are checked before changing either artifact. Preserve the
# separate ten-host-image manifest; add-ons are independently built NE files.
foreach($item in $drivers) {
    $root=if($item.id -eq 'win101'){$Win101Build}else{$Win31Build}
    Copy-Item -LiteralPath (Join-Path $root $item.file) -Destination (Join-Path $repo ('assets/release/'+$item.file)) -Force
}
$addonPath=Join-Path $repo 'assets/release/addon-manifest.json'
$prior=Get-Content -LiteralPath $addonPath -Raw|ConvertFrom-Json
if($prior.schema -ne 1){throw 'Invalid existing add-on manifest'}
$utilities=if($null -eq $prior.utilities){@()}else{@($prior.utilities)}
$allDrivers=@($drivers)+@($prior.drivers|Where-Object {$_.id -notin @('win101','win31')})
[ordered]@{schema=1;drivers=$allDrivers;utilities=$utilities}|ConvertTo-Json -Depth 5|
    Set-Content -LiteralPath $addonPath -Encoding UTF8
'PASS two compiled guest add-ons published; host release manifest unchanged'
