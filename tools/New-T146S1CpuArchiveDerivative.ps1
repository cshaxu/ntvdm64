[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$T130Root = '',
    [string]$R5Root = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($T130Root)) {
    $T130Root = Join-Path $repository 'artifacts\build\current\t130-machine-outcome-r1'
}
if ([string]::IsNullOrWhiteSpace($R5Root)) {
    $R5Root = Join-Path $repository 'artifacts\build\bochs-2.6-native-s7-runtime-trace-msvc-r5'
}
$t130 = [IO.Path]::GetFullPath($T130Root)
$r5 = [IO.Path]::GetFullPath($R5Root)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }

function Get-Sha256([string]$Path) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing required input: $Path" }
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
}
function Copy-Verified([string]$Source, [string]$Destination) {
    $hash = Get-Sha256 $Source
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    if ((Get-Sha256 $Destination) -ne $hash) { throw "Copy hash mismatch: $Source" }
    return $hash
}

foreach ($path in @('main.o','cpu\exception.o','cpu\libcpu.a','iodev\libiodev.a',
        'memory\libmemory.a','gui\libgui.a','ntdos64-t98-current-adapter.mak')) {
    [void](Get-Sha256 (Join-Path $t130 $path))
}
foreach ($path in @('cpu\Makefile','cpu\cpu.cc')) {
    [void](Get-Sha256 (Join-Path $r5 $path))
}

& robocopy $t130 $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "T130 copy failed: $LASTEXITCODE" }

# Preserve the admitted T130 machine-composition exception object outside the
# CPU archive build.  The archive itself must contain the r5 exception member.
$retainedException = Join-Path $build 'cpu\t130-machine-composition-exception.obj'
[void](Copy-Verified (Join-Path $t130 'cpu\exception.o') $retainedException)

# Re-establish one pinned CPU source tree.  Every CPU translation unit starts
# from r5; the only admitted delta is the registered BX-TRACE-056 cpu.cc.
$sourceHashes = [ordered]@{}
foreach ($source in Get-ChildItem -LiteralPath (Join-Path $r5 'cpu') -Filter *.cc -File) {
    $destination = Join-Path $build ('cpu\' + $source.Name)
    $sourceHashes[$source.Name] = Copy-Verified $source.FullName $destination
}
$diagnosticSource = Join-Path $repository 'src\bochs\cpu\cpu.cc'
$sourceHashes['cpu.cc'] = Copy-Verified $diagnosticSource (Join-Path $build 'cpu\cpu.cc')

$cpuDirectory = Join-Path $build 'cpu'
Get-ChildItem -LiteralPath $cpuDirectory -Filter *.o -File | Remove-Item -Force
Remove-Item -LiteralPath (Join-Path $cpuDirectory 'libcpu.a') -Force

$defines = @(
    '/DBX_NTVDM_ENABLE_PREFETCH_PREDECESSOR_DIAGNOSTIC=1',
    '/DBX_NTVDM_ENABLE_IRQ13_TRANSFER_DIAGNOSTIC=0',
    '/DBX_NTVDM_ENABLE_IRET_TF_DIAGNOSTIC=0',
    '/DBX_NTVDM_ENABLE_REAL_MODE_FAR_JUMP_DIAGNOSTIC=0',
    '/DBX_NTVDM_ENABLE_STACK_TRANSFER_DIAGNOSTIC=0'
)
$cpuMakefile = Join-Path $cpuDirectory 'Makefile'
$cpuMakeText = Get-Content -LiteralPath $cpuMakefile -Raw
$cpuMakeText = [regex]::Replace(
    $cpuMakeText,
    '(?m)^CXXFLAGS = (.*)$',
    ('CXXFLAGS = $1 ' + ($defines -join ' ')),
    1)
if ($cpuMakeText -notmatch 'BX_NTVDM_ENABLE_PREFETCH_PREDECESSOR_DIAGNOSTIC=1') {
    throw 'Unable to add the one permitted CPU diagnostic definition.'
}
$cpuT146Makefile = Join-Path $cpuDirectory 'Makefile.t146'
[IO.File]::WriteAllText($cpuT146Makefile, $cpuMakeText, [Text.UTF8Encoding]::new($false))

$baseShim = Join-Path $t130 'ntdos64-t98-current-adapter.mak'
$shimText = Get-Content -LiteralPath $baseShim -Raw
$shimText = $shimText.Replace('ntdos64-t98-current-adapter.exe', 'ntdos64-t146-prefetch-archive.exe')
$shimText = $shimText.Replace('ntdos64-t98-current-adapter.map', 'ntdos64-t146-prefetch-archive.map')
$shimText = $shimText.Replace('cpu\exception.o iodev/libiodev.a', 'cpu\exception.o iodev/libiodev.a')
$shimText = $shimText.Replace('cpu\exception.o: cpu\exception.cc', 'cpu\exception.o: cpu\t130-machine-composition-exception.obj' + "`r`n" + "`tcopy /Y cpu\t130-machine-composition-exception.obj cpu\exception.o")
$shimText = [regex]::Replace(
    $shimText,
    '(ntdos64-t146-prefetch-archive\.exe:\s+[^\r\n]*machine\\unexp_nt\.c\.obj)',
    '$1 cpu/libcpu.a')
$shim = Join-Path $build 'ntdos64-t146-prefetch-archive.mak'
[IO.File]::WriteAllText($shim, $shimText, [Text.UTF8Encoding]::new($false))

$manifest = [ordered]@{
    schema = 'ntdos64.t146.s1.cpu-archive-derivative.v1'
    mode = 'generator-only-no-compiler-linker-or-runtime-invocation'
    t130Root = $t130
    r5Root = $r5
    t130RetainedInputs = @{}
    r5CpuSources = $sourceHashes
    diagnosticSource = [ordered]@{ path = 'src/bochs/cpu/cpu.cc'; sha256 = $sourceHashes['cpu.cc']; exception = 'BX-TRACE-056' }
    diagnosticDefines = $defines
    directCpuObjectOverride = $false
    prohibitedRebuilds = @('full-bochs','iodev','memory','gui','adapter','cli','machine')
    permittedBuild = @(
        'cd cpu; nmake /nologo /f Makefile.t146 libcpu.a',
        'nmake /nologo /f ntdos64-t146-prefetch-archive.mak ntdos64-t146-prefetch-archive.exe'
    )
}
foreach ($path in @('main.o','cpu\exception.o','iodev\libiodev.a','memory\libmemory.a','gui\libgui.a')) {
    $manifest.t130RetainedInputs[$path] = Get-Sha256 (Join-Path $t130 $path)
}
$manifest.t130RetainedInputs['cpu\exception.o.restored'] = Get-Sha256 $retainedException
$manifest.cpuMakefileSha256 = Get-Sha256 $cpuT146Makefile
$manifest.linkShimSha256 = Get-Sha256 $shim
[IO.File]::WriteAllText((Join-Path $build 't146-s1-cpu-archive-derivative.json'), ($manifest | ConvertTo-Json -Depth 7), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared T146 S1 single-diagnostic CPU archive derivative: $build"
