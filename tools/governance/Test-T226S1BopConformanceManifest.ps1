[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [string]$ManifestPath = 'docs/etc/operations/t226-s1-bop-local-conformance-manifest-001.json'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath($RepositoryRoot)
$expected = [IO.Path]::GetFullPath((Join-Path $root $ManifestPath))
if (!(Test-Path -LiteralPath $expected)) { throw "Missing manifest: $expected" }
$temp = Join-Path ([IO.Path]::GetTempPath()) ('ntdos64-t226-s1-' + [guid]::NewGuid().ToString('N') + '.json')
try {
    & (Join-Path $root 'tools/governance/Export-T226S1BopConformanceManifest.ps1') -RepositoryRoot $root -OutputPath $temp
    $actual = Get-Content -LiteralPath $temp -Raw | ConvertFrom-Json
    if ($actual.schema -ne 'ntdos64.t226.s1.bop-local-conformance-manifest.v1') { throw 'Unexpected conformance schema' }
    if (@($actual.entries).Count -ne 203) { throw 'Expected 203 conformance rows' }
    $duplicate = @($actual.entries | Group-Object identity | Where-Object { $_.Count -ne 1 })
    if ($duplicate.Count -ne 0) { throw 'Non-unique conformance identity' }
    foreach ($row in $actual.entries) {
        if ([string]::IsNullOrWhiteSpace([string]$row.packageId) -or
            [string]::IsNullOrWhiteSpace([string]$row.fixtureId) -or
            [string]::IsNullOrWhiteSpace([string]$row.fixturePath) -or
            [string]::IsNullOrWhiteSpace([string]$row.targetSubtask)) {
            throw "Incomplete conformance binding for $($row.identity)"
        }
        if ($row.directReadonlyContract -notmatch 'Direct and Readonly') { throw "Missing Direct/Readonly contract for $($row.identity)" }
    }
    $expectedText = Get-Content -LiteralPath $expected -Raw
    $actualText = Get-Content -LiteralPath $temp -Raw
    if ($expectedText -ne $actualText) { throw 'Tracked conformance manifest is stale; regenerate it with Export-T226S1BopConformanceManifest.ps1.' }
    Write-Host 'T226 S1 BOP local conformance manifest verification passed.'
}
finally {
    if (Test-Path -LiteralPath $temp) { Remove-Item -LiteralPath $temp -Force }
}