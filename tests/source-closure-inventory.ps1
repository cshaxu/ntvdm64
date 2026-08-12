param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
& (Join-Path $root 'tools/Export-DosWow16SourceClosureInventory.ps1') -RepositoryRoot $root -OutputDirectory (Join-Path $root 'docs') | Out-Null
$records = Get-Content -Raw (Join-Path $root 'docs/dos-wow16-source-closure-inventory.json') | ConvertFrom-Json
$allowed = @('source-built','static-data','excluded-prebuilt','source-blocked','host/support','sample/development-only')
if (($records | Where-Object { $allowed -notcontains $_.Classification }).Count) { throw 'Inventory contains an unknown classification.' }
if (($records | Where-Object { [string]::IsNullOrWhiteSpace($_.Classification) -or [string]::IsNullOrWhiteSpace($_.SourceAnchor) }).Count) { throw 'Inventory has an unclassified or unanchored record.' }
$excluded = @($records | Where-Object Classification -eq 'excluded-prebuilt')
if ($excluded.Count -ne 4 -or @($excluded | Where-Object { $_.Evidence.SourceBuilt -ne $false }).Count) { throw 'The four excluded binplace inputs are not preserved as SourceBuilt:false.' }
$built = @($records | Where-Object Classification -eq 'source-built')
if ($built.Count -ne 27 -or @($built | Where-Object { -not $_.Evidence.OutputSha256 -or -not $_.Evidence.OutputBytes -or -not $_.Evidence.BuildManifest -or @($_.Evidence.CommandManifests).Count -eq 0 }).Count) { throw 'Source-built records lack retained identity or command evidence.' }
if (@($records | Where-Object { $_.Scope -eq 'WOW16 runtime/tool candidate' -and $_.Classification -notin @('source-blocked','sample/development-only') }).Count) { throw 'Unrebuilt WOW16 runtime/tool candidates must remain source-blocked.' }
if (-not @($records | Where-Object { $_.Scope -eq 'non-DOS/WOW16 guest-side candidate' }).Count) { throw 'Non-DOS/WOW16 guest-side candidates were omitted.' }
Write-Output "source-closure inventory verified: $($records.Count) records, $($built.Count) source-built"
