param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path,
    [string]$PackageRoot
)
$ErrorActionPreference = 'Stop'
function Get-SpoolSha256([string]$Path) {
    $stream = [IO.File]::OpenRead($Path)
    $algorithm = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($algorithm.ComputeHash($stream)).Replace('-', '') }
    finally { $algorithm.Dispose(); $stream.Dispose() }
}
# Original, load-only guest companion. No compilation or guest-byte changes.
$relative = 'src/mvdm/wow16/winspool/WINSPOOL.EXE'
$source = Join-Path $RepositoryRoot $relative
$expected = 'E4E787340F124476090ED0F22837D7D7ACAB37FD7BBE49DB46CAE9AC4944D535'
if ((Get-SpoolSha256 $source) -ne $expected) {
    throw 'Unrecognized original WINSPOOL.EXE'
}
$bytes = [IO.File]::ReadAllBytes($source)
$ne = [BitConverter]::ToInt32($bytes, 0x3c)
if ($bytes.Length -ne 2112 -or [BitConverter]::ToUInt16($bytes, $ne) -ne 0x454e) {
    throw 'Unexpected WINSPOOL NE profile'
}
$moduleCount = [BitConverter]::ToUInt16($bytes, $ne + 0x1e)
$modules = $ne + [BitConverter]::ToUInt16($bytes, $ne + 0x28)
$names = $ne + [BitConverter]::ToUInt16($bytes, $ne + 0x2a)
if ($moduleCount -ne 1) { throw 'Unexpected WINSPOOL module dependencies' }
$name = $names + [BitConverter]::ToUInt16($bytes, $modules)
$dependency = [Text.Encoding]::ASCII.GetString($bytes, $name + 1, $bytes[$name])
if ($dependency -ne 'KERNEL') { throw 'WINSPOOL no longer imports only KERNEL' }
$segments = $ne + [BitConverter]::ToUInt16($bytes, $ne + 0x22)
$code = [BitConverter]::ToUInt16($bytes, $segments) -shl [BitConverter]::ToUInt16($bytes, $ne + 0x32)
# Own retained map/LST: argument sizes and original DGDI function numbers.
foreach ($row in @(@(0x10, '6A-0C-68-00-00-68-C4-01'),
                   @(0x1d, '6A-1A-68-00-00-68-C5-01'),
                   @(0x2a, '6A-12-68-00-00-68-C6-01'))) {
    if ([BitConverter]::ToString($bytes, $code + $row[0], 8) -ne $row[1]) {
        throw 'Original spool thunk ABI mismatch'
    }
}
# Entry 0x37 has the original cProc prologue; int 3 is at 0x41 (LST line 133).
if ($bytes[$code + 0x41] -ne 0xcc) { throw 'Original ExtTextOut limitation changed' }
$record = [ordered]@{
    source = $relative; destination = 'system32/WINSPOOL.EXE'
    sha256 = $expected.ToLowerInvariant(); bytes = $bytes.Length
    role = 'immutable original WOW16 printing facade'; imports = @($dependency)
    limitations = @('Original ExtTextOut export executes int 3; not repaired or accepted as functional')
}
if ($PackageRoot) {
    $package = (Resolve-Path -LiteralPath $PackageRoot).Path
    $build = [IO.Path]::GetFullPath((Join-Path $RepositoryRoot 'build')) + [IO.Path]::DirectorySeparatorChar
    if (!$package.StartsWith($build, [StringComparison]::OrdinalIgnoreCase)) {
        throw 'This acceptance stager only accepts a build-owned package'
    }
    foreach ($required in @('system32/USER.EXE', 'system32/KRNL386.EXE', 'system32/WOW32.DLL')) {
        if (!(Test-Path -LiteralPath (Join-Path $package $required) -PathType Leaf)) {
            throw "Incomplete WOW acceptance package: $required"
        }
    }
    $target = Join-Path $package $record.destination
    if (Test-Path -LiteralPath $target) {
        if ((Get-SpoolSha256 $target) -ne $expected) {
            throw 'Refusing to replace a different guest spool module'
        }
    } else { Copy-Item -LiteralPath $source -Destination $target }
    if ((Get-SpoolSha256 $target) -ne $expected) { throw 'Staged guest identity mismatch' }
    $record | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $package 'wow-spool-companion.json')
}
[pscustomobject]$record
