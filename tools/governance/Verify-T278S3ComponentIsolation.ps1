[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$ledger = @(Import-Csv (Join-Path $ops 'm0-t278-destination-source-identity-ledger.tsv') -Delimiter "`t")
$components = @('mvdm-host','mvdm-support','mvdm-tools','mvdm-softpc-firmware')

foreach ($component in $components) {
    $componentRoot = Join-Path $root ('src/' + $component)
    $expected = @($ledger | Where-Object destination_component -eq $component | ForEach-Object { $_.destination_relative_path })
    $actual = @(Get-ChildItem -LiteralPath $componentRoot -File -Recurse | ForEach-Object {
        $_.FullName.Substring($componentRoot.Length + 1).Replace('\','/')
    } | Where-Object { $_ -ne 'README.md' })
    if (Compare-Object ($expected | Sort-Object) ($actual | Sort-Object)) { throw "Mirror file-set differs from frozen ledger: $component" }
}

# Current build configuration is deliberately small. Exclude historical source
# mirrors and archived tooling, whose makefiles are evidence rather than product inputs.
$configFiles = @(Get-ChildItem -LiteralPath $root -File -Recurse | Where-Object {
    $_.FullName -notmatch '\\(src\\opennt-mvdm-|tools\\historical\\)' -and
    $_.Name -in @('CMakeLists.txt','build.ninja') -or
    ($_.FullName -notmatch '\\(src\\opennt-mvdm-|tools\\historical\\)' -and $_.Extension -in @('.cmake','.vcxproj','.vcproj','.sln','.props'))
})
$oldPathPattern = 'mvdm-host[\\/]((inc|oemuni|suballoc|vdmutils)|dirs|makefil0|softpc\.new[\\/](base[\\/]bios|bios|roms|data))'
$stale = @($configFiles | Select-String -Pattern $oldPathPattern -AllMatches)
if ($stale.Count) { throw "Current build configuration still names moved path(s): $($stale.Path -join ', ')" }
& (Join-Path $PSScriptRoot 'Verify-T278S2PhysicalMirrorMove.ps1') -RepositoryRoot $root
Write-Host "PASS: T278 S3 component file sets equal frozen ledger; active-config-files=$($configFiles.Count); stale-moved-path-references=0."
