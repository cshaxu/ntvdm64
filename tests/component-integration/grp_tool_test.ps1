param(
    [Parameter(Mandatory)][string]$Grp,
    [Parameter(Mandatory)][string]$FixtureRoot,
    [Parameter(Mandatory)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$grp = (Resolve-Path -LiteralPath $Grp).Path
$fixture = (Resolve-Path -LiteralPath $FixtureRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { Remove-Item -LiteralPath $build -Recurse -Force }
Copy-Item -LiteralPath $fixture -Destination $build -Recurse -Force
$main = Join-Path $build 'MAIN.GRP'

& $grp replace-root $main --root $build
if ($LASTEXITCODE -ne 0) { throw "GRP replace-root failed: $LASTEXITCODE" }
$shown = & $grp show $main
if ($LASTEXITCODE -ne 0 -or $shown -notcontains "ITEM1=$(Join-Path $build 'WINFILE.EXE')") {
    throw 'GRP show does not expose the relocated Program Manager item'
}
$bytes = [IO.File]::ReadAllBytes($main)
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
'PASS GRP structured root replacement and PMCC rejection'
