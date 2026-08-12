[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [Parameter(Mandatory = $true)][string]$T130Root,
    [Parameter(Mandatory = $true)][string]$R5Root,
    [switch]$RequireFinalLinkInputs
)

$ErrorActionPreference = 'Stop'
function Get-Sha256([string]$Path) { (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash }
$build = [IO.Path]::GetFullPath($BuildRoot)
$t130 = [IO.Path]::GetFullPath($T130Root)
$r5 = [IO.Path]::GetFullPath($R5Root)
$manifest = Get-Content -LiteralPath (Join-Path $build 't146-s1-cpu-archive-derivative.json') -Raw | ConvertFrom-Json
if ($manifest.directCpuObjectOverride) { throw 'T146 must not use a direct CPU object override.' }
if ((Test-Path -LiteralPath (Join-Path $build 'cpu\cpu_trace.obj'))) { throw 'T146 must not contain the T136 direct override object.' }
$make = Get-Content -LiteralPath (Join-Path $build 'cpu\Makefile.t146') -Raw
foreach ($required in @(
        'BX_NTVDM_ENABLE_PREFETCH_PREDECESSOR_DIAGNOSTIC=1',
        'BX_NTVDM_ENABLE_IRQ13_TRANSFER_DIAGNOSTIC=0',
        'BX_NTVDM_ENABLE_IRET_TF_DIAGNOSTIC=0',
        'BX_NTVDM_ENABLE_REAL_MODE_FAR_JUMP_DIAGNOSTIC=0',
        'BX_NTVDM_ENABLE_STACK_TRANSFER_DIAGNOSTIC=0')) {
    if ($make -notmatch [regex]::Escape($required)) { throw "Missing explicit diagnostic setting: $required" }
}
foreach ($source in Get-ChildItem -LiteralPath (Join-Path $r5 'cpu') -Filter *.cc -File) {
    $actual = Join-Path $build ('cpu\' + $source.Name)
    if ($source.Name -eq 'cpu.cc') { continue }
    if ((Get-Sha256 $actual) -ne (Get-Sha256 $source.FullName)) { throw "Non-diagnostic CPU source differs from r5: $($source.Name)" }
}
$shim = Get-Content -LiteralPath (Join-Path $build 'ntdos64-t146-prefetch-archive.mak') -Raw
if ($shim -match 'cpu_trace\.obj') { throw 'T146 link shim retains a direct CPU override.' }
if ($shim -notmatch 'cpu/libcpu\.a') { throw 'T146 link shim does not require the rebuilt CPU archive.' }
if ($shim -match '(^|\r?\n)(iodev/libiodev\.a|memory/libmemory\.a|gui/libgui\.a):') { throw 'T146 link shim rebuilds a non-CPU archive.' }
foreach ($path in @('main.o','iodev\libiodev.a','memory\libmemory.a','gui\libgui.a')) {
    if ((Get-Sha256 (Join-Path $build $path)) -ne (Get-Sha256 (Join-Path $t130 $path))) { throw "T130 retained input changed: $path" }
}
if ((Get-Sha256 (Join-Path $build 'cpu\t130-machine-composition-exception.obj')) -ne (Get-Sha256 (Join-Path $t130 'cpu\exception.o'))) {
    throw 'Retained T130 exception object changed before CPU archive build.'
}
if ($RequireFinalLinkInputs) {
    $archive = Join-Path $build 'cpu\libcpu.a'
    $exe = Join-Path $build 'ntdos64-t146-prefetch-archive.exe'
    $map = Join-Path $build 'ntdos64-t146-prefetch-archive.map'
    foreach ($path in @($archive, $exe, $map)) {
        if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing final link input/output: $path" }
    }
    if ((Get-Sha256 (Join-Path $build 'cpu\exception.o')) -ne (Get-Sha256 (Join-Path $t130 'cpu\exception.o'))) {
        throw 'Final link does not restore the retained T130 exception object.'
    }
    $mapText = Get-Content -LiteralPath $map -Raw
    if ($mapText -match 'cpu_trace\.obj') { throw 'Final map resolves a prohibited direct CPU override.' }
    if ($mapText -notmatch 'libcpu:cpu\.o') { throw 'Final map does not resolve CPU code from the rebuilt archive.' }
}
Write-Host 'T146 S1 CPU archive derivative boundary verification passed.'
