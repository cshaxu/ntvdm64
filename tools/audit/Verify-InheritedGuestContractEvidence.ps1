param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path,
    [string]$ResearchRoot = 'build/research-ccpu40-v86-guest-contract-20260929',
    [string]$OutputRoot = 'build/M0-T430/S1/r001'
)
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath($RepositoryRoot)
$research = [IO.Path]::GetFullPath((Join-Path $repo $ResearchRoot))
$out = [IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
$buildPrefix = (Join-Path $repo 'build') + [IO.Path]::DirectorySeparatorChar
if (-not $out.StartsWith($buildPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Audit output must remain below repository build/.'
}
if (Test-Path -LiteralPath $out) { throw 'Use a fresh output directory; sealed evidence is not overwritten.' }
New-Item -ItemType Directory -Path $out | Out-Null

function Check-Table([string]$Name, [string[]]$Keys, [int]$ExpectedCount) {
    $rows = @(Import-Csv -LiteralPath (Join-Path $research $Name))
    if ($rows.Count -ne $ExpectedCount) { throw "$Name count differs from inherited claim: $($rows.Count)." }
    $seen = @{}
    foreach ($row in $rows) {
        foreach ($key in $Keys) {
            if ($null -eq $row.PSObject.Properties[$key]) { throw "$Name missing column $key." }
        }
        $identity = ($Keys | ForEach-Object { $row.$_ }) -join [char]31
        if ($seen.ContainsKey($identity)) { throw "$Name duplicate key: $identity." }
        $seen[$identity] = $true
    }
    [pscustomobject]@{ Table=$Name; Rows=$rows.Count; UniqueKeys=$seen.Count }
}

$tables = @(
    Check-Table 'interface-inventory-manifest.csv' @('family','interface_key') 11817
    Check-Table 'guest-host-edge-manifest.csv' @('edge_kind','source','line','guest_edge') 3673
    Check-Table 'host-service-abi-catalog.csv' @('symbol') 23
    Check-Table 'broker-vdm-interface-inventory.csv' @('interface_key') 15
    Check-Table 'closure-evidence-matrix.csv' @('id') 17
)
$families = @(Import-Csv (Join-Path $research 'closure-evidence-matrix.csv'))
if ((($families.id | Sort-Object) -join ',') -ne ((1..17 | ForEach-Object { 'G{0:D2}' -f $_ }) -join ',')) {
    throw 'Inherited family IDs are not exactly G01–G17.'
}

$sources = @(foreach ($row in (Import-Csv (Join-Path $research 'current-formal-source-hashes.csv'))) {
    $path = Join-Path $repo $row.path
    $hash = if (Test-Path -LiteralPath $path -PathType Leaf) { (Get-FileHash -LiteralPath $path).Hash } else { '' }
    $state = if (-not $hash) { 'missing' } elseif ($hash -eq $row.actual_sha256) { 'same' } else { 'changed' }
    [pscustomobject]@{ Origin=$row.origin; Path=$row.path; OldHash=$row.actual_sha256; CurrentHash=$hash; State=$state }
})
$sources | Export-Csv -NoTypeInformation -Encoding UTF8 (Join-Path $out 'source-identity.csv')

$media = @(foreach ($row in (Import-Csv (Join-Path $research 'guest-binary-provenance.csv'))) {
    $name = [IO.Path]::GetFileName($row.deployed_file)
    foreach ($path in (@($row.deployed_file, (Join-Path 'O:/winnt/system32' $name)) | Select-Object -Unique)) {
        $hash = if (Test-Path -LiteralPath $path -PathType Leaf) { (Get-FileHash -LiteralPath $path).Hash } else { '' }
        $state = if (-not $hash) { 'missing' } elseif ($hash -eq $row.sha256) { 'same' } else { 'changed' }
        [pscustomobject]@{ OldPath=$row.deployed_file; CandidatePath=$path; OldHash=$row.sha256; CurrentHash=$hash; State=$state }
    }
})
$media | Export-Csv -NoTypeInformation -Encoding UTF8 (Join-Path $out 'media-identity.csv')

$published = @(foreach ($row in (Get-Content -Raw (Join-Path $repo 'build/M0-T429/S8/r011/published-manifest.json') | ConvertFrom-Json)) {
    $path = Join-Path 'O:/winnt/system32' $row.Name
    $hash = (Get-FileHash -LiteralPath $path).Hash
    if ($hash -ne $row.PublishedHash) { throw "Accepted publication changed: $path." }
    [pscustomobject]@{ Name=$row.Name; Hash=$hash; MatchesAccepted=$true }
})
$published | Export-Csv -NoTypeInformation -Encoding UTF8 (Join-Path $out 'accepted-publication.csv')
$oldGraph = Get-Content -Raw (Join-Path $research 'formal-x86-current/source-manifest.json') | ConvertFrom-Json
$currentGraph = Get-Content -Raw (Join-Path $repo 'build/M0-T427/S2/r001/source-manifest.json') | ConvertFrom-Json
$selection = @(foreach ($key in @('ccpuSources','biosSources','keymouseSources','systemSources',
    'disksSources','supportSources','videoSources','cvidcSources','commsSources','dosSources',
    'demSources','commandSources','redirectorSources','xmsSources','dpmiSources',
    'suballocSources','oemuniSources','hostRoots')) {
    if ($null -eq $oldGraph.PSObject.Properties[$key] -or $null -eq $currentGraph.PSObject.Properties[$key]) {
        throw "Missing selected source group $key."
    }
    $oldItems = @($oldGraph.$key); $currentItems = @($currentGraph.$key)
    [pscustomobject]@{ Group=$key; OldCount=$oldItems.Count; CurrentCount=$currentItems.Count;
        Added=(@($currentItems | Where-Object { $_ -notin $oldItems }) -join ';');
        Removed=(@($oldItems | Where-Object { $_ -notin $currentItems }) -join ';') }
})
$selection | Export-Csv -NoTypeInformation -Encoding UTF8 (Join-Path $out 'selected-original-groups.csv')
$inputFiles = @('audit-snapshot.json','current-formal-source-hashes.csv','guest-binary-provenance.csv',
    'formal-x86-current/source-manifest.json') + @($tables.Table)
@(foreach ($name in $inputFiles) {
    $file = Join-Path $research $name
    [pscustomobject]@{ Path=$file; Hash=(Get-FileHash -LiteralPath $file).Hash }
}) | Export-Csv -NoTypeInformation -Encoding UTF8 (Join-Path $out 'inherited-inputs.csv')
$summary = [ordered]@{
    Head=(git -C $repo rev-parse HEAD); Research=$research; Tables=$tables;
    Sources=@($sources | Group-Object State | Select-Object Name,Count);
    MediaCandidates=@($media | Group-Object State | Select-Object Name,Count);
    SourceUniquePaths=@($sources.Path | Sort-Object -Unique).Count;
    SelectedOriginalGroups=$selection.Count;
    CurrentProfile=$currentGraph.cpuProfile;
    PublishedFiles=$published.Count; RuntimeExecuted=$false
}
$summary | ConvertTo-Json -Depth 6 | Set-Content -Encoding UTF8 (Join-Path $out 'summary.json')
$summary | ConvertTo-Json -Depth 6
