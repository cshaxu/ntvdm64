param([string]$BuildRoot = 'build/M0-T420/S40/class-registration-r1')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if (!$output.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build.'
}
$hash = '6118B6837C7ED291CC9D70446B7D691699D49C9EF6F2BE60D8E0FAC478D93B65'
$mirror = Join-Path $root 'src/mvdm/wow32/walias.c'
foreach ($path in @($mirror, 'O:/repos.external/OpenNT/base/mvdm/wow32/walias.c')) {
    if ((Get-FileHash $path).Hash -ne $hash) { throw "Alias source changed: $path" }
}
$text = [IO.File]::ReadAllText($mirror)
function Slice([string]$start, [string]$end) {
    $a = $text.IndexOf($start)
    $b = $text.IndexOf($end, $a + $start.Length)
    if ($a -lt 0 -or $b -le $a) { throw 'Alias function boundary changed.' }
    $text.Substring($a, $b - $a)
}
$body = (Slice 'typedef struct _stdclass {' '// Returns a 32 window proc given a class index') +
    (Slice 'PWW FindPWW(' "/*`r`n * PWC GetClassWOWWords")
New-Item -ItemType Directory -Force $output | Out-Null
[IO.File]::WriteAllText((Join-Path $output 'original-alias.inc'), $body, [Text.Encoding]::ASCII)
'WOW_ALIAS_EXACT_ORIGINAL_BODIES'
