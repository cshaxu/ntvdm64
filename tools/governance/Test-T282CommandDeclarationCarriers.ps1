[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [switch]$RequireArchitectureProbes
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path

$carriers = @(
    @{ name = 'vdmapi.h'; source = 'O:\repos.external\OpenNT\public\internal\base\inc\vdmapi.h'; target = 'src\opennt-platform-abi\source\opennt\public\internal\base\inc\vdmapi.h'; hash = 'B942656BA8761DDB9325A7EEA7997394562D2E57797C923FF5688D7BE86426DE' },
    @{ name = 'pif.h'; source = 'O:\repos.external\OpenNT\public\internal\windows\inc\pif.h'; target = 'src\opennt-platform-abi\source\opennt\public\internal\windows\inc\pif.h'; hash = '224F6BDF5CFA15A37D95F97E24C30D37D947D5F7F5199D50AEAB65B39D150C55' }
)
foreach ($carrier in $carriers) {
    $source = (Get-FileHash -Algorithm SHA256 -LiteralPath $carrier.source).Hash
    $target = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $root $carrier.target)).Hash
    if ($source -ne $carrier.hash -or $target -ne $carrier.hash) {
        throw "$($carrier.name) is not byte-identical to its pinned original declaration carrier."
    }
}

$forwarder = Get-Content -Raw -LiteralPath (Join-Path $root 'src\adapter-win32\include\vdmapi.h')
if ($forwarder -notmatch 'opennt-platform-abi/source/opennt/public/internal/base/inc/vdmapi\.h') {
    throw 'The adapter vdmapi declaration forwarder does not select the exact platform-ABI carrier.'
}
foreach ($architecture in @('x86', 'x64')) {
    $graph = Join-Path $root "build\M0-T282\S1\$architecture\build.ninja"
    if ($RequireArchitectureProbes) {
        if (-not (Test-Path -LiteralPath $graph)) { throw "Missing $architecture declaration-carrier graph." }
        $content = Get-Content -Raw -LiteralPath $graph
        if ($content -notmatch 'adapter-win32/include' -or $content -notmatch 'internal/windows/inc') {
            throw "$architecture graph does not select both adapter vdmapi and original pif carriers."
        }
        foreach ($object in @('cmdexec.obj', 'cmdpif.obj')) {
            if (-not (Test-Path -LiteralPath (Join-Path $root "build\M0-T282\S1\$architecture\obj\$object"))) {
                throw "Missing $architecture focused declaration probe object $object."
            }
        }
    }
}
Write-Output 'T282 S2 COMMAND declaration-carrier verification passed.'
