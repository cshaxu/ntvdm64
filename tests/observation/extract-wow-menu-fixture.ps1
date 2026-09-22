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
    foreach ($base in @((Join-Path $root 'src/mvdm/wow32'), 'O:/repos.external/OpenNT/base/mvdm/wow32')) {
        $path = Join-Path $base $entry[0]
        if ((Get-FileHash $path).Hash -ne $entry[1]) { throw "Menu source changed: $path" }
    }
}
$menu = [IO.File]::ReadAllText((Join-Path $root 'src/mvdm/wow32/wumenu.c'))
$parts = @()
foreach ($name in @('WU32LoadMenu','WU32LoadMenuIndirect')) {
    $start = $menu.IndexOf("ULONG FASTCALL $name(PVDMFRAME pFrame)")
    $end = $menu.IndexOf("`r`n/*++", $start)
    if ($start -lt 0 -or $end -le $start) { throw 'Menu thunk boundary changed.' }
    $parts += $menu.Substring($start, $end-$start)
}
$resource = [IO.File]::ReadAllText((Join-Path $root 'src/mvdm/wow32/wres16.c'))
$start = $resource.IndexOf('#define VALIDPUT')
$end = $resource.IndexOf('MODNAME(', $start)
$macros = $resource.Substring($start, $end-$start)
$start = $resource.IndexOf('DWORD ConvertMenu16(')
$end = $resource.IndexOf('DWORD ConvertDialog16(', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Menu converter boundary changed.' }
[IO.File]::WriteAllText((Join-Path $output 'original-menu-convert.inc'),
    $macros + $resource.Substring($start, $end-$start), [Text.Encoding]::ASCII)
[IO.File]::WriteAllText((Join-Path $output 'original-menu-thunks.inc'),
    ($parts -join "`r`n"), [Text.Encoding]::ASCII)
'WOW_MENU_THUNKS_AND_CONVERTER_EXACT_ORIGINAL'
