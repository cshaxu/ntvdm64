$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
$sourcePath = Join-Path $repositoryRoot 'src/adapter-bochs/machine_facade.cc'
$headerPath = Join-Path $repositoryRoot 'src/adapter-bochs/machine_facade.h'
$source = Get-Content -LiteralPath $sourcePath -Raw
$header = Get-Content -LiteralPath $headerPath -Raw
$semanticSource = $source -replace '/\*[\s\S]*?\*/', ''
$semanticHeader = $header -replace '/\*[\s\S]*?\*/', ''

foreach ($pattern in @('ordinary_ram_readable', 'ordinary_ram_writable',
        'copy_from_ordinary_ram', 'copy_to_ordinary_ram')) {
    if ($source -notmatch $pattern) { throw "Missing mechanical action primitive: $pattern" }
}
if ($source.IndexOf('ordinary_ram_readable') -gt $source.IndexOf('copy_from_ordinary_ram') -or
    $source.IndexOf('ordinary_ram_writable') -gt $source.IndexOf('copy_to_ordinary_ram')) {
    throw 'Mechanical action must preflight before any copy primitive.'
}
foreach ($pattern in @('bop', 'service', 'provider', 'OpenNT', 'DOS',
        'adapter_runtime', 'host_session', 'SIM->', 'bx_devices')) {
    if ($semanticSource -match $pattern -or $semanticHeader -match $pattern) {
        throw "Forbidden machine mechanical-action dependency: $pattern"
    }
}
Write-Output 'adapter-bochs-machine-facade-boundary: selector-blind copied RAM action verified'
