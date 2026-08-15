$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$headerPath = Join-Path $repositoryRoot 'src\bx-mantle\bx_ntvdm_extended_memory_v1.h'
$sourcePath = Join-Path $repositoryRoot 'src\bx-mantle\bx_ntvdm_extended_memory_v1.cc'
$machinePath = Join-Path $repositoryRoot 'src\bx-mantle\bx_ntvdm_minimal_machine.cc'
$recipePath = Join-Path $repositoryRoot 'tools\Invoke-T197S6MinimalMachineLinkProbe.ps1'

foreach ($path in @($headerPath, $sourcePath, $machinePath, $recipePath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing T199 S24 input: $path"
    }
}

$header = Get-Content -LiteralPath $headerPath -Raw
$source = Get-Content -LiteralPath $sourcePath -Raw
$machine = Get-Content -LiteralPath $machinePath -Raw
$recipe = Get-Content -LiteralPath $recipePath -Raw

foreach ($pattern in @(
        'BX_NTVDM_EXTMEM_QUERY', 'BX_NTVDM_EXTMEM_ALLOCATE',
        'BX_NTVDM_EXTMEM_FREE', 'BX_NTVDM_EXTMEM_REALLOCATE',
        'BX_NTVDM_EXTMEM_MOVE', 'BX_NTVDM_EXTMEM_QUERY_FREE', 'uint64_t source_offset;',
        'uint64_t destination_offset;', 'uint64_t byte_count;',
        'uint32_t free_kib;', 'uint32_t largest_free_kib;', 'uint64_t physical_address;')) {
    if ($header -notmatch $pattern) {
        throw "Missing fixed-width C2 ABI invariant: $pattern"
    }
}

foreach ($pattern in @(
        'BX_NTVDM_EXTMEM_BASE BX_CONST64\(0x100000\)',
        'BX_NTVDM_EXTMEM_SLOTS 32u', 'backing_ram_readable',
        'backing_ram_writable', 'copy_from_backing_ram',
        'copy_to_backing_ram', 'BX_NTVDM_EXTMEM_REALLOCATE',
        'BX_NTVDM_EXTMEM_MOVE')) {
    if ($source -notmatch $pattern) {
        throw "Missing C2 owner or checked-copy invariant: $pattern"
    }
}

foreach ($pattern in @(
        'adapter', 'OpenNT', 'DOS', 'WOW', 'DEM', 'BOP', 'selector',
        'XMS', 'DPMI', 'malloc', 'new\s', 'get_vector', 'allocate_block',
        'host pointer', 'LoadLibrary', 'bx_vdm')) {
    if ($source -match $pattern -or $header -match $pattern) {
        throw "Forbidden C2 dependency or vocabulary: $pattern"
    }
}

foreach ($pattern in @(
        'bx_ntvdm_extended_memory_v1_set_lifecycle_active\(1u\)',
        'bx_ntvdm_extended_memory_v1_set_lifecycle_active\(0u\)')) {
    if ($machine -notmatch $pattern) {
        throw "Missing C2 lifecycle ownership invariant: $pattern"
    }
}

if ($recipe -notmatch "Name = 'extended_memory'; Path = 'src\\bx-mantle\\bx_ntvdm_extended_memory_v1.cc'") {
    throw 'The whole-core minimal-machine recipe does not name C2 explicitly'
}

Write-Output 't199-s24-extended-memory-boundary: C2 invariants verified'
