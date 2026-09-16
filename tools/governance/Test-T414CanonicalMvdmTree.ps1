[CmdletBinding()]
param(
    [ValidateSet('before', 'after')]
    [string]$Phase = 'before',
    [string]$BaselineRef = 'HEAD',
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$RepositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path

# Each entry is selected original MVDM content.  Project READMEs and the two
# intentionally non-MVDM carries are handled outside this mechanical map.
$maps = @(
    @{ Old = 'src/mvdm-host/'; New = 'src/mvdm/'; Kind = 'host' },
    @{ Old = 'src/mvdm-guest/dos/v86/'; New = 'src/mvdm/dos/v86/'; Kind = 'guest-dos' },
    @{ Old = 'src/mvdm-guest/bin86/'; New = 'src/mvdm/bin86/'; Kind = 'guest-bin86' },
    @{ Old = 'src/mvdm-guest/wow16/'; New = 'src/mvdm/wow16/'; Kind = 'guest-wow16' },
    @{ Old = 'src/mvdm-tools/vdmutils/'; New = 'src/mvdm/vdmutils/'; Kind = 'tool' },
    @{ Old = 'src/mvdm-softpc-firmware/softpc.new/'; New = 'src/mvdm/softpc.new/'; Kind = 'firmware' }
)
$excluded = @(
    'src/mvdm-host/kernel-vdm/',
    'src/mvdm-host/README.md',
    'src/mvdm-guest/README.md',
    'src/mvdm-tools/README.md',
    'src/mvdm-softpc-firmware/README.md'
)

function Invoke-Git([string[]]$Arguments) {
    $output = & git -C $RepositoryRoot @Arguments
    if ($LASTEXITCODE -ne 0) { throw "git $($Arguments -join ' ') failed" }
    return @($output)
}

$tracked = if ($Phase -eq 'before') {
    Invoke-Git @('ls-files')
} else {
    @(& git -C $RepositoryRoot ls-tree -r --name-only $BaselineRef -- src)
}
if ($LASTEXITCODE -ne 0) { throw "Cannot enumerate baseline tree: $BaselineRef" }
$rows = [System.Collections.Generic.List[object]]::new()
foreach ($map in $maps) {
    $hits = @($tracked | Where-Object { $_.StartsWith($map.Old, [System.StringComparison]::Ordinal) })
    if ($hits.Count -eq 0) { throw "No tracked source selected for $($map.Old)" }
    foreach ($old in $hits) {
        $isExcluded = $false
        foreach ($excludedPath in $excluded) {
            if ($excludedPath.EndsWith('/')) {
                if ($old.StartsWith($excludedPath, [System.StringComparison]::Ordinal)) { $isExcluded = $true; break }
            } elseif ($old -eq $excludedPath) {
                $isExcluded = $true; break
            }
        }
        if ($isExcluded) { continue }
        $relative = $old.Substring($map.Old.Length)
        $rows.Add([pscustomobject]@{ Old = $old; New = $map.New + $relative; Kind = $map.Kind })
    }
}
foreach ($path in $excluded) {
    if ($Phase -eq 'after' -and $path -in @('src/mvdm-host/README.md', 'src/mvdm-guest/README.md', 'src/mvdm-tools/README.md', 'src/mvdm-softpc-firmware/README.md')) { continue }
    if ($path.EndsWith('/')) {
        if (@($tracked | Where-Object { $_.StartsWith($path, [System.StringComparison]::Ordinal) }).Count -eq 0) {
            throw "Expected exclusion is absent: $path"
        }
    } elseif ($tracked -notcontains $path) {
        throw "Expected exclusion is absent: $path"
    }
}

$duplicates = @($rows | Group-Object New | Where-Object { $_.Count -ne 1 })
if ($duplicates.Count -ne 0) { throw "Destination collision(s): $($duplicates.Name -join ', ')" }

$baselineBlobs = @{}
foreach ($line in @(& git -C $RepositoryRoot ls-tree -r $BaselineRef -- src)) {
    if ($line -match '^[0-9]+ blob ([0-9a-f]+)\t(.+)$') { $baselineBlobs[$matches[2]] = $matches[1] }
}
if ($LASTEXITCODE -ne 0) { throw "Cannot read baseline tree: $BaselineRef" }
$fullPaths = [System.Collections.Generic.List[string]]::new()
foreach ($row in $rows) {
    $path = if ($Phase -eq 'before') { $row.Old } else { $row.New }
    $full = Join-Path $RepositoryRoot $path
    if (!(Test-Path -LiteralPath $full -PathType Leaf)) { throw "Missing mapped file: $path" }
    if (!$baselineBlobs.ContainsKey($row.Old)) { throw "Missing baseline blob: $($row.Old)" }
    $fullPaths.Add($full)
}
$actualBlobs = @($fullPaths | & git -C $RepositoryRoot hash-object --stdin-paths)
if ($LASTEXITCODE -ne 0 -or $actualBlobs.Count -ne $rows.Count) { throw 'Cannot hash complete mapped tree' }
for ($index = 0; $index -lt $rows.Count; $index++) {
    $row = $rows[$index]
    if ($baselineBlobs[$row.Old] -ne $actualBlobs[$index].Trim()) {
        throw "Blob mismatch: $($fullPaths[$index]) from $($row.Old)"
    }
}
$checked = $rows.Count

$counts = $rows | Group-Object Kind | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T414 canonical MVDM map passed: phase=$Phase baseline=$BaselineRef files=$checked $($counts -join ' ') exclusions=$($excluded.Count)"
