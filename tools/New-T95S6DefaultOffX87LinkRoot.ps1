[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$R2Root = '',
    [string]$BuildRoot = '',
    [switch]$EnableX87CompatibilityDiagnostic
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($R2Root)) {
    $R2Root = Join-Path $repository 'artifacts\build\bochs-2.6-native-adapter-msvc-r2'
}
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-adapter-msvc-r3'
}
$r2 = [IO.Path]::GetFullPath($R2Root)
$build = [IO.Path]::GetFullPath($BuildRoot)
$r2Exe = Join-Path $r2 'ntdos64-native-adapter-observation.exe'
$r2Map = Join-Path $r2 'ntdos64-native-adapter-observation.map'
$shimRelative = 'ntdos64-native-adapter-observation.mak'
$fpuRelative = 'fpu\fpu.cc'

foreach ($required in @($r2Exe, $r2Map, (Join-Path $r2 $shimRelative), (Join-Path $r2 $fpuRelative))) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) { throw "Missing immutable r2 input: $required" }
}
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }

& robocopy $r2 $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "r2 build-root copy failed with exit code $LASTEXITCODE" }

$sourceFpu = Join-Path $repository 'src\bochs\fpu\fpu.cc'
$copiedFpu = Join-Path $build $fpuRelative
Copy-Item -LiteralPath $sourceFpu -Destination $copiedFpu -Force
$sourceFpuHash = (Get-FileHash -LiteralPath $sourceFpu -Algorithm SHA256).Hash
if ((Get-FileHash -LiteralPath $copiedFpu -Algorithm SHA256).Hash -ne $sourceFpuHash) {
    throw 'Copied fpu.cc hash mismatch.'
}

$removed = @(
    'fpu\fpu.o',
    'fpu\libfpu.a',
    'ntdos64-native-adapter-observation.exe',
    'ntdos64-native-adapter-observation.map',
    'narrow-companion-link-observation.json',
    'narrow-companion-link.log',
    'narrow-companion-link.exitcode',
    'run-narrow-companion-link.cmd'
)
foreach ($relative in $removed) {
    $path = Join-Path $build $relative
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Expected copied artifact is absent: $path" }
    Remove-Item -LiteralPath $path -Force
    if (Test-Path -LiteralPath $path) { throw "Failed to remove copied stale artifact: $path" }
}

$shim = Join-Path $build $shimRelative
$shimHash = (Get-FileHash -LiteralPath $shim -Algorithm SHA256).Hash
$fpuText = Get-Content -LiteralPath $copiedFpu -Raw
if (-not $fpuText.Contains('#define BX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC 0')) {
    throw 'Default-off x87 diagnostic macro is absent from copied fpu.cc.'
}
if ($fpuText.Contains('/DBX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC=1')) {
    throw 'Source root unexpectedly contains an opt-in compiler definition.'
}

$fpuMakefile = Join-Path $build 'fpu\Makefile'
if ($EnableX87CompatibilityDiagnostic) {
    $makeText = Get-Content -LiteralPath $fpuMakefile -Raw
    $needle = 'CXXFLAGS = /nologo /MT /W3 /EHs-c- /DNDEBUG /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Ox '
    $replacement = $needle + '/DBX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC=1 '
    if (-not $makeText.Contains($needle) -or $makeText.Contains('/DBX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC=1')) {
        throw 'Unable to apply one root-local x87 diagnostic compiler opt-in.'
    }
    [IO.File]::WriteAllText($fpuMakefile, $makeText.Replace($needle, $replacement), [Text.UTF8Encoding]::new($false))
    if (-not (Get-Content -LiteralPath $fpuMakefile -Raw).Contains($replacement)) {
        throw 'Root-local x87 diagnostic compiler opt-in did not persist.'
    }
}

$record = [ordered]@{
    schema = 'ntdos64.t95-s6.narrow-companion-build-root.v1'
    baseRoot = $r2
    baseExecutableSha256 = (Get-FileHash -LiteralPath $r2Exe -Algorithm SHA256).Hash
    baseMapSha256 = (Get-FileHash -LiteralPath $r2Map -Algorithm SHA256).Hash
    baseFpuSourceSha256 = (Get-FileHash -LiteralPath (Join-Path $r2 $fpuRelative) -Algorithm SHA256).Hash
    buildRoot = $build
    fpuSourceSha256 = $sourceFpuHash
    shimSha256 = $shimHash
    removedCopiedArtifacts = $removed
    permittedCommand = 'nmake /f ntdos64-native-adapter-observation.mak ntdos64-native-adapter-observation.exe'
    diagnosticMacro = if ($EnableX87CompatibilityDiagnostic) { 'BX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC=1 (root-local fpu Makefile only)' } else { 'BX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC=0 (local source default; no compiler opt-in)' }
    fpuMakefileSha256 = (Get-FileHash -LiteralPath $fpuMakefile -Algorithm SHA256).Hash
    retryPolicy = 'none'
}
[IO.File]::WriteAllText((Join-Path $build 'narrow-companion-build-root.json'), ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared fresh default-off x87 companion root: $build"
