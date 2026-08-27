param(
    [Parameter(Mandatory = $true)]
    [string]$UnionLedgerPath,
    [Parameter(Mandatory = $true)]
    [string]$OpenNtRoot,
    [Parameter(Mandatory = $true)]
    [string]$OpenNt45PrivateRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Normalize-Include([string]$Value) {
    return $Value.Replace([char]47, [char]92).TrimStart([char]46, [char]92).ToLowerInvariant()
}

function Get-SelectedPath($Row) {
    if ($Row.selection -eq 'opennt-4.5-one-sided') {
        return $Row.secondary_path
    }
    return $Row.primary_path
}

$sourceExtensions = @('.c', '.cc', '.cpp', '.cxx', '.h', '.hpp', '.inc', '.asm', '.s')
$crtOrSdk = @(
    'assert.h', 'ctype.h', 'direct.h', 'errno.h', 'fcntl.h', 'float.h', 'io.h',
    'limits.h', 'malloc.h', 'memory.h', 'process.h', 'setjmp.h', 'share.h',
    'signal.h', 'stdarg.h', 'stddef.h', 'stdio.h', 'stdlib.h', 'string.h',
    'sys\\types.h', 'sys/types.h', 'time.h', 'wchar.h', 'windows.h', 'winbase.h',
    'wincon.h', 'windef.h', 'winerror.h', 'wingdi.h', 'winnt.h', 'winreg.h',
    'winsock.h', 'winsock2.h', 'winspool.h', 'winuser.h', 'ws2tcpip.h'
) | ForEach-Object { Normalize-Include $_ }

$ledger = Import-Csv -LiteralPath $UnionLedgerPath -Delimiter "`t"
$hostRows = @($ledger | Where-Object {
    $_.owner_root -eq 'mvdm-host' -and
    $sourceExtensions -contains [IO.Path]::GetExtension($_.target_path).ToLowerInvariant()
})

$selectedMvdm = @{}
foreach ($row in $ledger | Where-Object { $_.owner_root -eq 'mvdm-host' }) {
    $key = Normalize-Include $row.target_path
    $selectedMvdm[$key] = Get-SelectedPath $row
}

$externalRoots = @(
    (Resolve-Path -LiteralPath $OpenNtRoot).Path.TrimEnd([char]92),
    (Resolve-Path -LiteralPath $OpenNt45PrivateRoot).Path.TrimEnd([char]92)
)
$externalIndex = @{}
foreach ($root in $externalRoots) {
    foreach ($file in Get-ChildItem -LiteralPath $root -Recurse -File -Include *.h,*.inc) {
        $key = $file.Name.ToLowerInvariant()
        if (-not $externalIndex.ContainsKey($key)) {
            $externalIndex[$key] = [System.Collections.Generic.List[string]]::new()
        }
        $externalIndex[$key].Add($file.FullName)
    }
}

$occurrences = @{}
foreach ($row in $hostRows) {
    $sourcePath = Get-SelectedPath $row
    $line = 0
    foreach ($text in Get-Content -LiteralPath $sourcePath) {
        $line++
        if ($text -notmatch '^\s*#\s*include\s*[<"]([^">]+)[">]') {
            continue
        }
        $include = Normalize-Include $matches[1]
        if (-not $occurrences.ContainsKey($include)) {
            $occurrences[$include] = [System.Collections.Generic.List[object]]::new()
        }
        $occurrences[$include].Add([pscustomobject]@{
            source = $row.target_path
            source_path = $sourcePath
            line = $line
            form = if ($text -match '<') { 'angle' } else { 'quote' }
        })
    }
}

$output = foreach ($include in $occurrences.Keys | Sort-Object) {
    $hits = $occurrences[$include]
    $localPaths = [System.Collections.Generic.List[string]]::new()
    foreach ($hit in $hits) {
        $candidate = Join-Path (Split-Path -Parent $hit.source_path) $include
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            $localPaths.Add((Resolve-Path -LiteralPath $candidate).Path)
        }
    }
    $localPaths = @($localPaths | Sort-Object -Unique)
    $mvdmPath = $null
    foreach ($pair in $selectedMvdm.GetEnumerator()) {
        if ($pair.Key -eq $include -or $pair.Key.EndsWith("$([char]92)$include")) {
            $mvdmPath = $pair.Value
            break
        }
    }
    $disposition = ''
    $selected = ''
    $rationale = ''
    if ($localPaths.Count -gt 0) {
        $disposition = 'mvdm-local-direct'
        $selected = $localPaths -join ';'
        $rationale = 'quoted include resolves beside one or more selected MVDM sources'
    } elseif ($null -ne $mvdmPath) {
        $disposition = 'mvdm-selected-include'
        $selected = $mvdmPath
        $rationale = 'include name maps to a selected MVDM union path'
    } elseif ($crtOrSdk -contains $include) {
        $disposition = 'toolchain-or-public-sdk'
        $rationale = 'CRT or public Windows SDK declaration; do not mirror into OpenNT platform ABI'
    } elseif ($externalIndex.ContainsKey([IO.Path]::GetFileName($include))) {
        $candidates = @($externalIndex[[IO.Path]::GetFileName($include)] | Sort-Object -Unique)
        if ($candidates.Count -eq 1) {
            $disposition = 'exact-opennt-platform-candidate'
            $selected = $candidates[0]
            $rationale = 'unique header candidate outside selected MVDM union; source/ABI review required before import'
        } else {
            $disposition = 'ambiguous-opennt-platform-candidate'
            $selected = $candidates -join ';'
            $rationale = 'multiple same-basename candidates; package and declaration review required before import'
        }
    } else {
        $disposition = 'unresolved-external-include'
        $rationale = 'no selected MVDM, CRT/SDK or unique OpenNT header candidate found'
    }
    [pscustomobject][ordered]@{
        include = $include
        disposition = $disposition
        occurrence_count = $hits.Count
        source_count = @($hits.source | Sort-Object -Unique).Count
        forms = @($hits.form | Sort-Object -Unique) -join ';'
        selected_candidate = $selected
        first_source = ($hits | Select-Object -First 1).source
        first_line = ($hits | Select-Object -First 1).line
        rationale = $rationale
    }
}

$directory = Split-Path -Parent $OutputPath
if (-not (Test-Path -LiteralPath $directory -PathType Container)) {
    throw "Output directory does not exist: $directory"
}
$output | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation |
    Set-Content -LiteralPath $OutputPath -Encoding utf8
Write-Output "Wrote $($output.Count) direct-include rows to $OutputPath"
