[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$ManifestPath = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $scriptPath = $MyInvocation.MyCommand.Path
    if ([string]::IsNullOrWhiteSpace($scriptPath)) { throw 'RepositoryRoot was not supplied and script path is unavailable.' }
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $scriptPath)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) { $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-intact-msvc-r1' }
if ([string]::IsNullOrWhiteSpace($ManifestPath)) { $ManifestPath = Join-Path $repository 'artifacts\analysis\t95-s6-intact-native-tuple-manifest-001-20260811-001\intact-native-tuple-manifest.json' }
$build = [IO.Path]::GetFullPath($BuildRoot)
$manifestFile = [IO.Path]::GetFullPath($ManifestPath)
$source = Join-Path $repository 'src\bochs'
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
if (-not (Test-Path -LiteralPath $manifestFile -PathType Leaf)) { throw "Missing intact tuple manifest: $manifestFile" }

$manifest = Get-Content -LiteralPath $manifestFile -Raw | ConvertFrom-Json
if ($manifest.schema -ne 'ntdos64.t95-s6.intact-native-tuple-manifest.v1') { throw 'Unsupported intact tuple manifest schema.' }
function Assert-Hash([string]$Path, [string]$Expected, [string]$Label) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "$Label is missing: $Path" }
    $actual = (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToUpperInvariant()
    if ($actual -ne $Expected.ToUpperInvariant()) { throw "$Label hash mismatch: expected $Expected, got $actual." }
}
foreach ($entry in @($manifest.tuple.makefileFamily)) {
    Assert-Hash (Join-Path $source $entry.relativePath.Replace('/', '\')) $entry.sha256 "Source tuple input $($entry.relativePath)"
}
Assert-Hash (Join-Path $source 'config.h') $manifest.tuple.configHeader.sha256 'Source config.h'
Assert-Hash (Join-Path $source 'config.status') $manifest.tuple.configStatus.sha256 'Source config.status'

$profileSource = [IO.Path]::GetFullPath($manifest.runtimeProfile.path)
$romSource = Join-Path $source 'bios'
foreach ($input in @($profileSource, (Join-Path $romSource 'BIOS-bochs-latest'), (Join-Path $romSource 'VGABIOS-lgpl-latest'))) {
    if (-not (Test-Path -LiteralPath $input -PathType Leaf)) { throw "Declared runtime-profile input is missing: $input" }
}

$sourceMakefile = Join-Path $source 'Makefile'
$lines = [IO.File]::ReadAllLines($sourceMakefile)
$start = [Array]::FindIndex([string[]]$lines, [Predicate[string]]{ param($line) $line -match '^bochs\.exe:' })
if ($start -lt 0) { throw 'Original generated Makefile has no bochs.exe target.' }
$end = -1
for ($i = $start + 1; $i -lt $lines.Count; $i++) { if ([string]::IsNullOrWhiteSpace($lines[$i])) { $end = $i - 1; break } }
if ($end -lt $start) { throw 'Original executable target has no terminating blank line.' }
$target = @($lines[$start..$end])
$target[0] = $target[0] -replace '^bochs\.exe:', 'ntdos64-native-container.exe:'
if (($target -join "`n") -match '(?i)\bbochs\.exe\b') { throw 'Derived target retained a bochs.exe input.' }

New-Item -ItemType Directory -Path $build -Force | Out-Null
& robocopy $source $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "robocopy failed with exit code $LASTEXITCODE" }
foreach ($entry in @($manifest.tuple.makefileFamily)) {
    Assert-Hash (Join-Path $build $entry.relativePath.Replace('/', '\')) $entry.sha256 "Copied tuple input $($entry.relativePath)"
}
Assert-Hash (Join-Path $build 'config.h') $manifest.tuple.configHeader.sha256 'Copied config.h'
Assert-Hash (Join-Path $build 'config.status') $manifest.tuple.configStatus.sha256 'Copied config.status'

New-Item -ItemType Directory -Path (Join-Path $build 'ROM') -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $romSource 'BIOS-bochs-latest') -Destination (Join-Path $build 'ROM\BIOS-bochs-latest') -Force
Copy-Item -LiteralPath (Join-Path $romSource 'VGABIOS-lgpl-latest') -Destination (Join-Path $build 'ROM\VGABIOS-lgpl-latest') -Force
Copy-Item -LiteralPath $profileSource -Destination (Join-Path $build 'intact-native-runtime-profile.bochsrc') -Force
$shim = @('# Generated from the frozen intact native tuple.', '!INCLUDE Makefile', '') + $target + @('')
$shimPath = Join-Path $build 'ntdos64-native-container.mak'
[IO.File]::WriteAllText($shimPath, ($shim -join "`r`n"), [Text.UTF8Encoding]::new($false))

$record = [ordered]@{
    schema = 'ntdos64.t95-s6.intact-native-build-root.v1'
    manifest = $manifestFile
    source = $source
    buildRoot = $build
    tupleMakefiles = $manifest.tuple.makefileFamily.Count
    copiedConfigSha256 = (Get-FileHash -LiteralPath (Join-Path $build 'config.h') -Algorithm SHA256).Hash
    copiedStatusSha256 = (Get-FileHash -LiteralPath (Join-Path $build 'config.status') -Algorithm SHA256).Hash
    profileSha256 = (Get-FileHash -LiteralPath (Join-Path $build 'intact-native-runtime-profile.bochsrc') -Algorithm SHA256).Hash
    shimSha256 = (Get-FileHash -LiteralPath $shimPath -Algorithm SHA256).Hash
    permittedDryRun = 'nmake /n /f ntdos64-native-container.mak ntdos64-native-container.exe'
}
[IO.File]::WriteAllText((Join-Path $build 'intact-native-build-root.json'), ($record | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared intact native build root: $build"
