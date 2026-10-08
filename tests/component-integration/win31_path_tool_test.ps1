param(
    [Parameter(Mandatory)][string]$InstallRoot,
    [Parameter(Mandatory)][string]$Tool,
    [string]$MalformedInstallRoot = ''
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
$tool = (Resolve-Path -LiteralPath $Tool).Path
$programManager = Join-Path $root 'PROGMAN.INI'
$winIni = Join-Path $root 'WIN.INI'
$report = Join-Path $root 'PATCH\PATH-REPAIR.TXT'

if (!(Test-Path -LiteralPath (Join-Path $root 'WIN.COM') -PathType Leaf)) {
    throw 'Fixture is not an installed Windows 3.1 tree'
}

$originalProgramManager = Get-Content -LiteralPath $programManager -Raw
if ($originalProgramManager -notmatch '(?im)^Group\d+=[A-Z]:\\') {
    throw 'Fixture has no absolute Program Manager group path to repair'
}

# This line names no object inside the selected tree.  It is intentionally
# stale-looking and must survive; otherwise the tool would be a blind text
# replacer rather than a bounded relocation repairer.
Set-Content -LiteralPath $winIni -Value "[T437]`r`nUnrelated=O:\WIN31\DOES-NOT-EXIST.DAT`r`n" -Encoding Default

& $tool --root $root
if ($LASTEXITCODE -ne 0) { throw "WIN31PATH failed: $LASTEXITCODE" }

$repairedProgramManager = Get-Content -LiteralPath $programManager -Raw
$repairedWinIni = Get-Content -LiteralPath $winIni -Raw
if ($repairedProgramManager -notmatch [regex]::Escape($root) + '\\MAIN\.GRP') {
    throw 'Program Manager root was not relocated'
}
if ($repairedWinIni -notmatch 'O:\\WIN31\\DOES-NOT-EXIST\.DAT') {
    throw 'Tool rewrote an unproven path reference'
}
$group = [IO.File]::ReadAllBytes((Join-Path $root 'MAIN.GRP'))
if ([Text.Encoding]::ASCII.GetString($group, 0, 4) -ne 'PMCC') {
    throw 'Fixture MAIN.GRP is not a supported Program Manager group'
}
$itemOffset = [BitConverter]::ToUInt16($group, 0x22)
$targetOffset = [BitConverter]::ToUInt16($group, $itemOffset + 22)
$targetEnd = $targetOffset
while ($targetEnd -lt $group.Length -and $group[$targetEnd] -ne 0) { ++$targetEnd }
$target = [Text.Encoding]::ASCII.GetString($group, $targetOffset, $targetEnd - $targetOffset)
if ($target -ne (Join-Path $root 'WINFILE.EXE')) {
    throw "Program-group command was not relocated: $target"
}
[uint32]$checksum = 0
for ($offset = 0; $offset + 1 -lt $group.Length; $offset += 2) {
    $checksum = ($checksum + [uint32][BitConverter]::ToUInt16($group, $offset)) -band 0xffff
}
if ($checksum -ne 0) { throw 'Program-group checksum was not preserved' }
if (!(Test-Path -LiteralPath $report -PathType Leaf) -or
    (Get-Content -LiteralPath $report -Raw) -notmatch '(?m)^PROGMAN\.INI:') {
    throw 'Missing explicit relocation report'
}

$firstHash = (Get-FileHash -LiteralPath $programManager).Hash
& $tool --root $root
if ($LASTEXITCODE -ne 0) { throw "WIN31PATH idempotence run failed: $LASTEXITCODE" }
if ((Get-FileHash -LiteralPath $programManager).Hash -ne $firstHash) {
    throw 'Second repair run changed an already repaired Program Manager file'
}

if ($MalformedInstallRoot) {
    $malformed = (Resolve-Path -LiteralPath $MalformedInstallRoot).Path.TrimEnd('\')
    $malformedProgramManager = Join-Path $malformed 'PROGMAN.INI'
    $malformedGroup = Join-Path $malformed 'MAIN.GRP'
    if (!(Test-Path -LiteralPath (Join-Path $malformed 'WIN.COM') -PathType Leaf)) {
        throw 'Malformed fixture is not an installed Windows 3.1 tree'
    }
    $before = (Get-FileHash -LiteralPath $malformedProgramManager).Hash
    [byte[]]$bytes = [IO.File]::ReadAllBytes($malformedGroup)
    $bytes[0] = 0
    [IO.File]::WriteAllBytes($malformedGroup, $bytes)
    & $tool --root $malformed
    if ($LASTEXITCODE -ne 3) { throw "Malformed-group rejection returned $LASTEXITCODE" }
    if ((Get-FileHash -LiteralPath $malformedProgramManager).Hash -ne $before -or
        (Test-Path -LiteralPath (Join-Path $malformed 'PATCH'))) {
        throw 'Malformed Program Manager group caused a partial write'
    }
}

'PASS bounded Win31 path discovery, in-tree relocation, report, and idempotence'
