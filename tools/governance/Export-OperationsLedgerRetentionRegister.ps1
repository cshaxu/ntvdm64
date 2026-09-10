[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$operationsRoot = Join-Path $repositoryRoot 'docs/etc/operations'
$outputPath = Join-Path $operationsRoot 'ledger-retention-register.tsv'

$rows = Get-ChildItem -LiteralPath $operationsRoot -File -Filter '*.tsv' |
    Where-Object { $_.Name -ne 'ledger-retention-register.tsv' } |
    Sort-Object Name |
    ForEach-Object {
        $relativePath = $_.FullName.Substring($repositoryRoot.Length + 1).Replace('\', '/')
        $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash.ToLowerInvariant()
        [PSCustomObject]@{
            path = $relativePath
            bytes = $_.Length
            sha256 = $hash
            owner = 'project documentation governance owner'
            purpose = 'deterministic machine-readable operations evidence ledger'
            retention_disposition = 'retained as indexed support evidence pending reviewed closure disposition'
            retirement_trigger = 'owner-approved archive or removal after closure records durable replacement, path, digest, and schema'
        }
    }

$header = 'path' + [char]9 + 'bytes' + [char]9 + 'sha256' + [char]9 + 'owner' + [char]9 + 'purpose' + [char]9 + 'retention_disposition' + [char]9 + 'retirement_trigger'
$lines = @($header) + @($rows | ForEach-Object {
    @($_.path, $_.bytes, $_.sha256, $_.owner, $_.purpose, $_.retention_disposition, $_.retirement_trigger) -join [char]9
})
[System.IO.File]::WriteAllLines($outputPath, [string[]]$lines, [System.Text.UTF8Encoding]::new($false))
Write-Host "Wrote $($rows.Count) retained-ledger records: $outputPath"
