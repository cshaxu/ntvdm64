param([string]$BuildRoot = 'build/M0-T420/S40/class-registration-r1')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if (!$output.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build.'
}
$mirror = Join-Path $root 'src/mvdm/wow32/wuwind.c'
$hash = 'AB270E2669CFD383DA4BAAC471BB63E7C9C603581F4F7592AD447E8A783953DC'
foreach ($path in @($mirror, 'O:/repos.external/OpenNT/base/mvdm/wow32/wuwind.c')) {
    if ((Get-FileHash $path).Hash -ne $hash) { throw "Subclass source changed: $path" }
}
$text = [IO.File]::ReadAllText($mirror)
$start = $text.IndexOf('ULONG FASTCALL WU32SetWindowLong(')
$end = $text.IndexOf("`r`n/*++", $start)
if ($start -lt 0 -or $end -le $start) { throw 'Original function boundary changed.' }
New-Item -ItemType Directory -Force $output | Out-Null
[IO.File]::WriteAllText((Join-Path $output 'original-subclass.inc'),
    $text.Substring($start, $end - $start), [Text.Encoding]::ASCII)
$start = $text.IndexOf('ULONG FASTCALL WU32GetWindowLong(')
$end = $text.IndexOf("`r`n/*++", $start)
if ($start -lt 0 -or $end -le $start) { throw 'Original getter boundary changed.' }
[IO.File]::WriteAllText((Join-Path $output 'original-getprocedure.inc'),
    $text.Substring($start, $end - $start), [Text.Encoding]::ASCII)
'WOW_SUBCLASS_EXACT_ORIGINAL_BODY'
$classMirror = Join-Path $root 'src/mvdm/wow32/wuclass.c'
$classHash = '12CDD44DFEC6B74A426165977F941DA86D90BBAD7AA9C59A085010D02528629D'
foreach ($path in @($classMirror, 'O:/repos.external/OpenNT/base/mvdm/wow32/wuclass.c')) {
    if ((Get-FileHash $path).Hash -ne $classHash) { throw "Class subclass source changed: $path" }
}
$text = [IO.File]::ReadAllText($classMirror)
$start = $text.IndexOf('ULONG FASTCALL WU32SetClassLong(')
$end = $text.IndexOf("`r`n/*++", $start)
if ($start -lt 0 -or $end -le $start) { throw 'Original class setter boundary changed.' }
[IO.File]::WriteAllText((Join-Path $output 'original-class-subclass.inc'),
    $text.Substring($start, $end - $start), [Text.Encoding]::ASCII)
'WOW_CLASS_SUBCLASS_EXACT_ORIGINAL_BODY'
