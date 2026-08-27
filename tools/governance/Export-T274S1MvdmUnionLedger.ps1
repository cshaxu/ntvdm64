param(
    [Parameter(Mandatory = $true)]
    [string]$OpenNtMvdmRoot,
    [Parameter(Mandatory = $true)]
    [string]$OpenNt45MvdmRoot,
    [Parameter(Mandatory = $true)]
    [string]$RepositoryGuestRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-RelativeKey([string]$Root, [string]$Path) {
    return $Path.Substring($Root.Length).TrimStart([char]'\').Replace([char]'\', [char]'/')
}

function Get-Owner([string]$Key) {
    if ($Key.StartsWith('dos/v86/', [StringComparison]::OrdinalIgnoreCase)) {
        return 'mvdm-guest/dos/v86'
    }
    if ($Key.StartsWith('wow16/', [StringComparison]::OrdinalIgnoreCase) -or
        $Key.StartsWith('bin86/', [StringComparison]::OrdinalIgnoreCase)) {
        return 'mvdm-guest/wow16'
    }
    if ($Key.StartsWith('tools16/', [StringComparison]::OrdinalIgnoreCase)) {
        return 'tools/opennt'
    }
    return 'mvdm-host'
}

function Get-LocalGuestPath([string]$GuestRoot, [string]$Key) {
    if ($Key.StartsWith('dos/v86/', [StringComparison]::OrdinalIgnoreCase)) {
        return Join-Path (Join-Path $GuestRoot 'dos-v86') $Key.Substring('dos/v86/'.Length)
    }
    if ($Key.StartsWith('wow16/', [StringComparison]::OrdinalIgnoreCase)) {
        return Join-Path (Join-Path $GuestRoot 'wow16') $Key.Substring('wow16/'.Length)
    }
    if ($Key.StartsWith('bin86/', [StringComparison]::OrdinalIgnoreCase)) {
        return Join-Path (Join-Path $GuestRoot 'bin86') $Key.Substring('bin86/'.Length)
    }
    return ''
}

function Get-Transfer([string]$Owner, [string]$LocalIdentity) {
    switch ($Owner) {
    'mvdm-guest/dos/v86' { return $LocalIdentity }
    'mvdm-guest/wow16' { return $LocalIdentity }
    'tools/opennt' { return 'tool-supply-later-subtask' }
    default { return 'non-guest-host-supply-later-subtask' }
    }
}

$primary = (Resolve-Path -LiteralPath $OpenNtMvdmRoot).Path.TrimEnd('\')
$secondary = (Resolve-Path -LiteralPath $OpenNt45MvdmRoot).Path.TrimEnd('\')
$guest = (Resolve-Path -LiteralPath $RepositoryGuestRoot).Path.TrimEnd('\')
$records = @{}
foreach ($file in Get-ChildItem -LiteralPath $primary -Recurse -File) {
    $key = Get-RelativeKey $primary $file.FullName
    $records[$key] = [ordered]@{ Primary = $file; Secondary = $null }
}
foreach ($file in Get-ChildItem -LiteralPath $secondary -Recurse -File) {
    $key = Get-RelativeKey $secondary $file.FullName
    if (-not $records.ContainsKey($key)) {
        $records[$key] = [ordered]@{ Primary = $null; Secondary = $file }
    } else {
        $records[$key].Secondary = $file
    }
}

$rows = foreach ($key in $records.Keys | Sort-Object) {
    $record = $records[$key]
    $primaryHash = if ($null -eq $record.Primary) { '' } else {
        (Get-FileHash -LiteralPath $record.Primary.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    }
    $secondaryHash = if ($null -eq $record.Secondary) { '' } else {
        (Get-FileHash -LiteralPath $record.Secondary.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    }
    $selection = ''
    $rationale = ''
    if ($null -eq $record.Primary) {
        $selection = 'opennt-4.5-one-sided'
        $rationale = 'one-sided canonical-superset input'
    } elseif ($null -eq $record.Secondary) {
        $selection = 'opennt-one-sided'
        $rationale = 'one-sided canonical-superset input'
    } elseif ($primaryHash -eq $secondaryHash) {
        $selection = 'dual-identical'
        $rationale = 'same bytes with dual provenance'
    } else {
        $selection = 'opennt-primary-conflict'
        $rationale = 'primary OpenNT NT4 source family selected consistently for conflicting owner package paths; OpenNT-4.5 variant retained as rejected evidence'
    }
    $owner = Get-Owner $key
    $selectedHash = if ($selection -eq 'opennt-4.5-one-sided') {
        $secondaryHash
    } else {
        $primaryHash
    }
    $localPath = Get-LocalGuestPath $guest $key
    $localHash = if ($localPath -ne '' -and (Test-Path -LiteralPath $localPath -PathType Leaf)) {
        (Get-FileHash -LiteralPath $localPath -Algorithm SHA256).Hash.ToLowerInvariant()
    } else {
        ''
    }
    $localIdentity = if ($owner -eq 'mvdm-host' -or $owner -eq 'tools/opennt') {
        ''
    } elseif ($localHash -ne '' -and $localHash -eq $selectedHash) {
        'carry-forward-local-guest-mirror'
    } elseif ($localHash -ne '') {
        'local-guest-path-different-external-evidence-only'
    } else {
        'external-guest-source-outside-current-carried-inventory'
    }
    [pscustomobject][ordered]@{
        target_path = $key
        owner_root = $owner
        transfer = Get-Transfer $owner $localIdentity
        package_root = $key.Split('/')[0]
        selection = $selection
        primary_path = if ($null -eq $record.Primary) { '' } else { $record.Primary.FullName }
        primary_sha256 = $primaryHash
        secondary_path = if ($null -eq $record.Secondary) { '' } else { $record.Secondary.FullName }
        secondary_sha256 = $secondaryHash
        local_guest_path = $localPath
        local_guest_sha256 = $localHash
        rationale = $rationale
    }
}

$directory = Split-Path -Parent $OutputPath
if (-not (Test-Path -LiteralPath $directory -PathType Container)) {
    throw "Output directory does not exist: $directory"
}
$rows | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation |
    Set-Content -LiteralPath $OutputPath -Encoding utf8
Write-Output "Wrote $($rows.Count) union-ledger rows to $OutputPath"
