[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$source = Join-Path $RepositoryRoot 'src\mvdm-host\vdmredir\vrdisp.c'
$ledger = Join-Path $RepositoryRoot 'docs\etc\operations\m0-t290-s1-redirector-service-abi-ledger.tsv'
$pathLedger = Join-Path $RepositoryRoot 'docs\etc\operations\m0-t290-s1-redirector-path-interface-ledger.tsv'
$mirror = Join-Path $RepositoryRoot 'src\mvdm-host\vdmredir'
$openNt = 'O:\repos.external\OpenNT\base\mvdm\vdmredir'
$openNt45 = 'O:\repos.external\OpenNT-4.5\nt\private\mvdm\vdmredir'

foreach ($directory in @($mirror, $openNt, $openNt45)) {
    if (-not (Test-Path -LiteralPath $directory -PathType Container)) { throw "Missing Redirector source root: $directory" }
}

$dispatch = @{}
foreach ($line in Get-Content -LiteralPath $source) {
    if ($line -match '^\s*(Vr\w+)\s*,?\s*//\s*0x([0-9a-fA-F]{2})') {
        $dispatch[('57:{0}' -f $matches[2].ToUpperInvariant())] = $matches[1]
    }
}
if ($dispatch.Count -ne 50) { throw "VrDispatchTable must have 50 entries; found $($dispatch.Count)" }

$rows = Import-Csv -LiteralPath $ledger -Delimiter "`t"
$serviceRows = @($rows | Where-Object { $_.selector -match '^57:[0-9A-F]{2}$' })
if ($serviceRows.Count -ne 50) { throw "Service ledger must have 50 rows; found $($serviceRows.Count)" }
foreach ($row in $serviceRows) {
    if (-not $dispatch.ContainsKey($row.selector)) { throw "Ledger selector absent from VrDispatchTable: $($row.selector)" }
    if ($dispatch[$row.selector] -ne $row.original_entry) { throw "Ledger entry mismatch for $($row.selector): expected $($dispatch[$row.selector])" }
    $body = Join-Path $RepositoryRoot ('src\mvdm-host\' + $row.original_body)
    if (-not (Test-Path -LiteralPath $body)) { throw "Ledger body missing: $($row.original_body)" }
}

$files = Get-ChildItem -LiteralPath $mirror -File
if ($files.Count -ne 25) { throw "Redirector mirror must have 25 paths; found $($files.Count)" }
$pathRows = Import-Csv -LiteralPath $pathLedger -Delimiter "`t"
if ($pathRows.Count -ne 25) { throw "Redirector path ledger must have 25 rows; found $($pathRows.Count)" }
foreach ($row in $pathRows) {
    $relativePath = $row.path -replace '/', '\\'
    if (-not (Test-Path -LiteralPath (Join-Path $RepositoryRoot ('src\\mvdm-host\\' + $relativePath)))) {
        throw "Path ledger source missing: $($row.path)"
    }
}
foreach ($file in $files) {
    $openNtFile = Join-Path $openNt $file.Name
    $openNt45File = Join-Path $openNt45 $file.Name
    if (-not (Test-Path -LiteralPath $openNtFile) -or -not (Test-Path -LiteralPath $openNt45File)) {
        throw "Canonical Redirector path missing: $($file.Name)"
    }
    $hash = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
    if ($hash -ne (Get-FileHash -LiteralPath $openNtFile -Algorithm SHA256).Hash -or
        $hash -ne (Get-FileHash -LiteralPath $openNt45File -Algorithm SHA256).Hash) {
        throw "Redirector mirror identity mismatch: $($file.Name)"
    }
}

Write-Output 'T290 S1 Redirector service ledger passed: 50 dispatch entries and 25 exact canonical mirror paths.'
