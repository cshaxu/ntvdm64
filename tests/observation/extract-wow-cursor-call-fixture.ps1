param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [ValidateSet('Cursor','Bitmap')][string]$Kind='Cursor'
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath($BuildRoot)
if (!$output.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build.'
}
$source = Join-Path $root ('src/mvdm/wow32/' + $(if($Kind -eq 'Bitmap'){'wuser.c'}else{'wucursor.c'}))
$text = [IO.File]::ReadAllText($source)
$signature=if($Kind -eq 'Bitmap'){'ULONG FASTCALL WU32LoadBitmap(PVDMFRAME pFrame)'}else{'ULONG FASTCALL WU32LoadCursor(PVDMFRAME pFrame)'}
$start = $text.IndexOf($signature)
if ($start -lt 0) { throw "Original $Kind thunk not found." }
$brace = $text.IndexOf('{', $start)
$depth = 0; $finish = -1
for ($i = $brace; $i -lt $text.Length; ++$i) {
    if ($text[$i] -eq '{') { ++$depth }
    elseif ($text[$i] -eq '}') {
        --$depth
        if ($depth -eq 0) { $finish = $i + 1; break }
    }
}
if ($finish -le $start) { throw "Original $Kind thunk boundary changed." }
New-Item -ItemType Directory -Force $output | Out-Null
[IO.File]::WriteAllText((Join-Path $output ($Kind.ToLowerInvariant()+'-call.inc')),
    $text.Substring($start, $finish - $start), [Text.Encoding]::ASCII)
Get-FileHash $source -Algorithm SHA256
