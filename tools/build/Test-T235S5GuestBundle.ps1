[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$output = (Resolve-Path -LiteralPath $OutputRoot).Path
$manifestPath = Join-Path $output 'guest-bundle-manifest.json'
if (!(Test-Path -LiteralPath $manifestPath)) { throw "Bundle manifest missing: $manifestPath" }
$manifest = Get-Content -Raw -LiteralPath $manifestPath | ConvertFrom-Json
if ($manifest.schema -ne 'ntdos64.t235.s5.guest-bundle.v1' -or @($manifest.unresolvedFallbacks).Count -ne 0) {
    throw 'Bundle manifest is incomplete or has an unexpected schema.'
}
foreach ($entry in $manifest.entries) {
    $destination = Join-Path $output $entry.destination.Replace('/','\')
    if (!(Test-Path -LiteralPath $destination)) { throw "Bundle destination missing: $($entry.destination)" }
    $item = Get-Item -LiteralPath $destination
    if ($item.Length -ne $entry.bytes -or
        (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant() -ne $entry.sha256) {
        throw "Bundle destination identity mismatch: $($entry.destination)"
    }
    $source = if ($entry.source.StartsWith('external/OpenNT-4.5/')) {
        'O:\repos.external\OpenNT-4.5\' + $entry.source.Substring('external/OpenNT-4.5/'.Length).Replace('/','\')
    } else { Join-Path $repository $entry.source.Replace('/','\') }
    if (!(Test-Path -LiteralPath $source)) { throw "Recorded source missing: $($entry.source)" }
    if ((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant() -ne $entry.sha256) {
        throw "Recorded source identity mismatch: $($entry.source)"
    }
}
Write-Host "Verified $(@($manifest.entries).Count) staged guest artifacts with complete origin records."
