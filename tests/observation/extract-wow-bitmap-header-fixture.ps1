param(
    [string]$Source = 'O:/repos.external/OpenNT/windows/core/ntuser/client/clres.c',
    [string]$BuildRoot = 'build/M0-T420/S40/bitmap-original-r1'
)
$ErrorActionPreference = 'Stop'
$expected = 'C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5'
if ((Get-FileHash $Source).Hash -ne $expected) { throw 'Original clres.c identity changed' }
$root = [IO.Path]::GetFullPath($BuildRoot)
$allowed = [IO.Path]::GetFullPath('build') + [IO.Path]::DirectorySeparatorChar
if (-not $root.StartsWith($allowed, [StringComparison]::OrdinalIgnoreCase)) { throw 'Fixture output must be under build' }
$null = New-Item -ItemType Directory -Force $root
$sourceText = [IO.File]::ReadAllText($Source)
$pieces = @($sourceText.Substring(0, $sourceText.IndexOf('#include')))
$patterns = @(
    '(?ms)^#define BPP01_MAXCOLORS.*?(?=^typedef struct)',
    '(?ms)^#define BMR_ICON.*?^typedef OLDCURSOR UNALIGNED \*UPOLDCURSOR;\r?\n',
    '(?ms)^DWORD HowManyColors\(.*?^}\r?\n',
    '(?ms)^BOOL TrulyMonochrome\(.*?^}\r?\n',
    '(?ms)^LPBITMAPINFOHEADER CopyDibHdr\(.*?^}\r?\n',
    '(?ms)^HBITMAP Convert1BppToMonoBitmap\(.*?^}\r?\n',
    '(?ms)^HBITMAP CreateScreenBitmap\(.*?^}\r?\n',
    '(?ms)^HBITMAP BitmapFromDIB\(.*?^}\r?\n',
    '(?ms)^HBITMAP ConvertDIBBitmap\(.*?^}\r?\n'
)
foreach ($pattern in $patterns) {
    $matches = [regex]::Matches($sourceText, $pattern)
    if ($matches.Count -ne 1) { throw "Non-unique original extraction: $pattern" }
    $pieces += $matches[0].Value
}
[IO.File]::WriteAllText((Join-Path $root 'original-bitmap-header.inc'), ($pieces -join "`r`n"), [Text.Encoding]::ASCII)
Write-Output "Original source fragments extracted without body edits: $root"
