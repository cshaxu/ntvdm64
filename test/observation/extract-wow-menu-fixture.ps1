param([string]$BuildRoot = 'build/M0-T420/S40/class-registration-r1')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if (!$output.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build.'
}
New-Item -ItemType Directory -Force $output | Out-Null
foreach ($entry in @(
    @('wumenu.c','0FC60E98713F9C37F6FEE8C5DF25DCBA55277DBAEF5B82C49A90E62AE6DB72C6'),
    @('wres16.c','24324D4B95B7C5AC102E9751872AF1DB779EFC0CFF223B32AA066034CB874AE9'))) {
    $path = Join-Path 'O:/repos.external/OpenNT/base/mvdm/wow32' $entry[0]
    if ((Get-FileHash $path).Hash -ne $entry[1]) { throw "Pinned original menu source changed: $path" }
}
function Read-MenuSource([string]$Base, [string]$Name) {
    [IO.File]::ReadAllText((Join-Path $Base $Name)).Replace("`r`n", "`n")
}
function Get-MenuPart([string]$Text, [string]$Begin, [string]$End) {
    $start = $Text.IndexOf($Begin, [StringComparison]::Ordinal)
    if ($start -lt 0) { throw "Menu source boundary missing: $Begin" }
    $endIndex = $Text.IndexOf($End, $start, [StringComparison]::Ordinal)
    if ($endIndex -le $start) { throw "Menu source end boundary missing: $End" }
    $Text.Substring($start, $endIndex - $start)
}
$mirrorBase = Join-Path $root 'src/mvdm-host/wow32'
$originalBase = 'O:/repos.external/OpenNT/base/mvdm/wow32'
$menu = Read-MenuSource $mirrorBase 'wumenu.c'
$originalMenu = Read-MenuSource $originalBase 'wumenu.c'
$parts = @()
foreach ($name in @('WU32LoadMenu','WU32LoadMenuIndirect')) {
    $begin = "ULONG FASTCALL $name(PVDMFRAME pFrame)"
    $part = Get-MenuPart $menu $begin "`n/*++"
    if ($part -cne (Get-MenuPart $originalMenu $begin "`n/*++")) { throw "Menu thunk differs from pinned original: $name" }
    $parts += $part
}
$resource = Read-MenuSource $mirrorBase 'wres16.c'
$originalResource = Read-MenuSource $originalBase 'wres16.c'
$macros = Get-MenuPart $resource '#define VALIDPUT' 'MODNAME('
$originalMacros = Get-MenuPart $originalResource '#define VALIDPUT' 'MODNAME('
$converter = Get-MenuPart $resource 'DWORD ConvertMenu16(' 'DWORD ConvertDialog16('
$originalConverter = Get-MenuPart $originalResource 'DWORD ConvertMenu16(' 'DWORD ConvertDialog16('
if ($macros -cne $originalMacros -or $converter -cne $originalConverter) { throw 'Menu converter differs from pinned original' }
[IO.File]::WriteAllText((Join-Path $output 'original-menu-convert.inc'),
    $macros + $converter, [Text.Encoding]::ASCII)
[IO.File]::WriteAllText((Join-Path $output 'original-menu-thunks.inc'),
    ($parts -join "`r`n"), [Text.Encoding]::ASCII)
# Controlled resource inputs are namespaced together with only their original
# clients. Never shadow the provider's real W32 callbacks at archive link time.
$clientPath=Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/clmenu.c'
$coverPath=Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/clres.c'
$client=[IO.File]::ReadAllText($clientPath)
$cover=[IO.File]::ReadAllText($coverPath)
$start=$client.IndexOf('HMENU CommonLoadMenu(')
if($start -lt 0){throw 'Original menu resource-client boundary changed'}
$clientBodies=$client.Substring($start)
$start=$cover.IndexOf('HANDLE WOWFindResourceExWCover(')
if($start -lt 0){throw 'Original Unicode resource-cover boundary changed'}
$end=[regex]::Match($cover.Substring($start),'(?m)^#endif\r?$')
if(!$end.Success){throw 'Original resource-cover closing boundary changed'}
$coverBody=$cover.Substring($start,$end.Index)
[IO.File]::WriteAllText((Join-Path $output 'original-menu-resource-inputs.inc'),
    "/* Microsoft OpenNT source-owned bodies; see source-inputs below. */`r`n"+$coverBody+$clientBodies,[Text.Encoding]::ASCII)
Get-FileHash -LiteralPath $clientPath,$coverPath|
    Export-Csv -NoTypeInformation (Join-Path $output 'menu-resource-input-source-sha256.csv')
'WOW_MENU_THUNKS_AND_CONVERTER_EXACT_ORIGINAL'
