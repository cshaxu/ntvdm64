param(
    [Parameter(Mandatory)][string]$Grp,
    [Parameter(Mandatory)][string]$FixtureRoot,
    [Parameter(Mandatory)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$grp = (Resolve-Path -LiteralPath $Grp).Path
$fixture = (Resolve-Path -LiteralPath $FixtureRoot).Path
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (!$build.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path -LiteralPath $build)) { throw 'Fresh build-owned evidence root required' }
Copy-Item -LiteralPath $fixture -Destination $build -Recurse -Force
$main = Join-Path $build 'MAIN.GRP'
$before = [IO.File]::ReadAllBytes($main)
$beforeTagOffset = [BitConverter]::ToUInt16($before, 6)
[byte[]]$beforeTag = @()
if ($beforeTagOffset -lt $before.Length) {
    $beforeTag = $before[$beforeTagOffset..($before.Length - 1)]
}
$beforeTagText = [Text.Encoding]::ASCII.GetString($beforeTag)

& $grp replace-root $main --root $build
if ($LASTEXITCODE -ne 0) { throw "GRP replace-root failed: $LASTEXITCODE" }
$shown = & $grp show $main
if ($LASTEXITCODE -ne 0 -or $shown -notcontains "ITEM1=$(Join-Path $build 'WINFILE.EXE')") {
    throw 'GRP show does not expose the relocated Program Manager item'
}
$bytes = [IO.File]::ReadAllBytes($main)
$afterTagOffset = [BitConverter]::ToUInt16($bytes, 6)
if ($afterTagOffset -le $beforeTagOffset) {
    throw 'GRP replacement did not make room before the Win3.1 tag records'
}
[byte[]]$afterTag = @()
if ($afterTagOffset -lt $bytes.Length) {
    $afterTag = $bytes[$afterTagOffset..($bytes.Length - 1)]
}
if (!$beforeTag.Length) {
    if ($afterTag.Length) { throw 'Tagless GRP acquired an unexpected tag suffix' }
} else {
    if ($afterTag.Length -lt 6 -or $beforeTag.Length -lt 6 -or
        [BitConverter]::ToString($afterTag[0..3]) -ne
        [BitConverter]::ToString($beforeTag[0..3])) {
        throw 'GRP replacement changed the leading tag identity'
    }
    if ([BitConverter]::ToUInt16($beforeTag, 0) -eq 0x8000 -and
        ($beforeTag.Length -lt 10 -or $afterTag.Length -lt 10 -or
         [BitConverter]::ToString($afterTag[0..9]) -ne
         [BitConverter]::ToString($beforeTag[0..9]))) {
        throw 'GRP replacement did not retain the Win3.1 tag header'
    }
}
if ($beforeTagText -match 'C:\\WINDOWS\\') {
    $afterTagText = [Text.Encoding]::ASCII.GetString($afterTag)
    if ($afterTagText -match 'C:\\WINDOWS\\' -or
        $afterTagText -notmatch ([regex]::Escape($build) + '\\')) {
        throw 'GRP replacement did not relocate the documented working-directory tag'
    }
}
[uint32]$checksum = 0
for ($offset = 0; $offset + 1 -lt $bytes.Length; $offset += 2) {
    $checksum = ($checksum + [uint32][BitConverter]::ToUInt16($bytes, $offset)) -band 0xffff
}
if ($checksum -ne 0) { throw 'GRP replacement did not preserve the PMCC checksum' }
$bytes[0] = 0
[IO.File]::WriteAllBytes((Join-Path $build 'INVALID.GRP'), $bytes)
$prior = $ErrorActionPreference
$ErrorActionPreference = 'Continue'
& $grp show (Join-Path $build 'INVALID.GRP') 2>$null
$invalid = $LASTEXITCODE
$ErrorActionPreference = $prior
if ($invalid -eq 0) { throw 'GRP show accepted malformed PMCC data' }
"PASS GRP structured root replacement and PMCC rejection; source-tag-bytes=$($beforeTag.Length)"
