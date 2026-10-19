[CmdletBinding()]
param(
    [string[]]$Target = @(),
    [object]$OwnershipManifest = '',
    [switch]$Inventory,
    [switch]$CheckResidual,
    [switch]$TasksExited,
    [switch]$Apply
)
# This is the only deletion owner. No arbitrary repository/external root, no
# wildcard expansion, and no force/ignore-danger option. All targets pass
# preflight before the first deletion. TasksExited is a caller attestation,
# NOT a process detector; inspect and join actual owners before using it.
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..')).TrimEnd('\','/')
$build = Join-Path $repo 'build'
$comparison = [StringComparison]::OrdinalIgnoreCase
function File-Sha256([string]$Path) {
    $algorithm=[Security.Cryptography.SHA256]::Create()
    $stream=[IO.File]::OpenRead($Path)
    try { ([BitConverter]::ToString($algorithm.ComputeHash($stream))).Replace('-','') }
    finally { $stream.Dispose(); $algorithm.Dispose() }
}
function Assert-NoLink([IO.FileSystemInfo]$Item) {
    if (($Item.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) {
        throw "Link/junction refused: $($Item.FullName)"
    }
}
function Resolve-BuildTarget([string]$Relative) {
    if ([IO.Path]::IsPathRooted($Relative) -or $Relative -match '(^|[\\/])\.\.([\\/]|$)|[:*?\[\]]') {
        throw "Exact repository-relative target required: $Relative"
    }
    foreach ($part in ($Relative -split '[\\/]')) {
        if (!$part -or $part.TrimEnd(' ','.') -ne $part) { throw "Ambiguous Win32 path spelling refused: $Relative" }
    }
    $absolute = [IO.Path]::GetFullPath((Join-Path $repo $Relative))
    if (!$absolute.StartsWith($build+'\', $comparison)) { throw "Not a build child: $Relative" }
    $tail = $absolute.Substring($build.Length+1)
    $underNinja=($tail -split '[\\/]')[0] -eq '.ninja'
    if (($tail -split '[\\/]')[0] -eq '.ccache' -or $tail -eq '.ninja') { throw "Persistent cache refused: $Relative" }
    # Inspect every ancestor as well as every descendant, never follow links.
    $cursor = Get-Item -LiteralPath $absolute -Force
    while ($cursor.FullName.Length -ge $build.Length) {
        Assert-NoLink $cursor
        if ($underNinja -and $cursor.PSIsContainer -and
            ((Test-Path -LiteralPath (Join-Path $cursor.FullName 'build.ninja')) -or
             (Test-Path -LiteralPath (Join-Path $cursor.FullName 'CMakeCache.txt')))) {
            throw "Configured Ninja cache refused: $Relative"
        }
        $cursor = if ($cursor.PSIsContainer) { $cursor.Parent } else { $cursor.Directory }
        if (!$cursor) { break }
    }
    return $absolute
}
function Get-TreeItems([string]$Path) {
    $pending = [Collections.Generic.Stack[IO.FileSystemInfo]]::new()
    $pending.Push((Get-Item -LiteralPath $Path -Force))
    while ($pending.Count) {
        $item = $pending.Pop()
        Assert-NoLink $item
        if ($item.Name -in @('.git','.hg','.svn') -or $item.Extension -eq '.git') {
            throw "Version-control data refused: $($item.FullName)"
        }
        $item
        if ($item.PSIsContainer) {
            foreach ($child in Get-ChildItem -LiteralPath $item.FullName -Force) { $pending.Push($child) }
        }
    }
}
if (!(Test-Path -LiteralPath $build)) { return }
Assert-NoLink (Get-Item -LiteralPath $build -Force)
if (!$Target.Count) {
    # Final residual inspection uses the same owner, not a second gate.
    $residual=@(Get-ChildItem -LiteralPath $build -Force | ForEach-Object {
        Assert-NoLink $_
        [pscustomobject]@{ Path=$_.FullName.Substring($repo.Length+1); Persistent=($_.Name -in @('.ninja','.ccache')) }
    })
    if ($CheckResidual -and @($residual | Where-Object { !$_.Persistent }).Count) {
        throw "Non-cache build residue: $(($residual | Where-Object { !$_.Persistent } | ForEach-Object Path) -join ', ')"
    }
    $residual
    return
}
if ($CheckResidual) { throw 'Residual inspection does not accept deletion targets' }
if ($Inventory -and $Apply) { throw 'Inventory cannot delete' }
if ($Apply -and !$TasksExited) { throw 'Inspect/join target owners first, then explicitly attest -TasksExited' }
if (!$OwnershipManifest -and !$Inventory) { throw 'A reviewed exact-target ownership manifest is required' }
$manifest = $null
if (!$Inventory) {
    if ($OwnershipManifest -is [string]) {
        $manifestPath = if ([IO.Path]::IsPathRooted($OwnershipManifest)) {
            [IO.Path]::GetFullPath($OwnershipManifest)
        } else { [IO.Path]::GetFullPath((Join-Path $repo $OwnershipManifest)) }
        if (!$manifestPath.StartsWith($repo+'\', $comparison)) { throw 'Ownership manifest must be repository-local' }
        $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
    } else { $manifest = $OwnershipManifest }
}
$reviewByPath = @{}
if (!$Inventory) {
    foreach ($entry in $manifest.reviewedFiles) {
        if ($reviewByPath.ContainsKey($entry.path)) { throw "Duplicate file review: $($entry.path)" }
        $reviewByPath[$entry.path]=$entry
    }
}
$plans = [Collections.Generic.List[object]]::new()
$files = [Collections.Generic.List[object]]::new()
foreach ($relative in $Target) {
    $path = Resolve-BuildTarget $relative
    foreach ($previous in $plans) {
        if ($path.Equals($previous.Absolute,$comparison) -or
            $path.StartsWith($previous.Absolute+'\',$comparison) -or
            $previous.Absolute.StartsWith($path+'\',$comparison)) { throw 'Duplicate/overlapping targets refused' }
    }
    $canonical = $path.Substring($repo.Length+1).Replace('\','/')
    $owner = @(if ($Inventory) { [pscustomobject]@{owner='unverified';reason='read-only inventory'} } else { $manifest.targets | Where-Object { $_.path -eq $canonical } })
    if ($owner.Count -ne 1 -or !$owner[0].owner -or !$owner[0].reason) { throw "Unknown ownership: $canonical" }
    [long]$bytes=0; [long]$count=0
    Write-Verbose "Preflight $canonical"
    $members=[Collections.Generic.List[string]]::new()
    Get-TreeItems $path | ForEach-Object {
        $item = $_
        $key = $item.FullName.Substring($repo.Length+1).Replace('\','/')
        if ($item.PSIsContainer) {
            $members.Add('D'+$key)
            if ($Apply) { $files.Add([pscustomobject]@{Path=$item.FullName;Directory=$true;Length=0;WriteTicks=$item.LastWriteTimeUtc.Ticks}) }
            return
        }
        $members.Add(("F{0}`t{1}`t{2}" -f $key,$item.Length,$item.LastWriteTimeUtc.Ticks))
        # Explicit review is mandatory for configuration, source/guest media,
        # archives and any extension outside the small generated-output set.
        $generated = $item.Extension.ToLowerInvariant() -in @('.obj','.o','.lib','.a','.pdb','.map','.res','.ilk','.exp','.tlog','.log','.txt','.json','.jsonl','.csv','.tsv','.xml','.html','.png','.jpg','.bmp','.etl','.dmp','.ninja','.cmd','.bat','.ps1','.mjs','.js','.py','.c','.h','.cpp','.def','.rsp','.exe','.dll','.manifest','.err','.out')
        $sensitive = $item.Extension -match '(?i)^\.(ini|reg|cfg|config|com|sys|bin|rom|drv|fon|fnt|zip|cab|iso|img|ima|vhd|vhdx|qcow2)$' -or
            $item.Name -match '(?i)^(command|krnl386|krnl286|win386|user|gdi|winmine|sol|write|ntdos|ntio)\.' -or !$generated
        if ($sensitive) {
            if ($Inventory) {
                [pscustomobject]@{Path=$key;Kind='ReviewRequired';Bytes=$item.Length;Sha256=(File-Sha256 $item.FullName)}
            } else {
            $review = $reviewByPath[$key]
            if (!$review -or !$review.reason -or
                (File-Sha256 $item.FullName) -ne $review.sha256) {
                throw "Configuration/media/unknown content needs exact hash review: $key"
            }
            }
        }
        if ($Apply) { $files.Add([pscustomobject]@{Path=$item.FullName;Directory=$false;Length=$item.Length;WriteTicks=$item.LastWriteTimeUtc.Ticks}) }
        $bytes += $item.Length; $count++
    }
    $members.Sort([StringComparer]::Ordinal)
    $sha=[Security.Cryptography.SHA256]::Create()
    try { $inventoryHash=([BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes(($members -join "`n"))))).Replace('-','') }
    finally { $sha.Dispose() }
    if ($Apply -and $owner[0].inventorySha256 -ne $inventoryHash) { throw "Unreviewed tree membership or metadata changed: $canonical" }
    $plans.Add([pscustomobject]@{Path=$canonical;Absolute=$path;Owner=$owner[0].owner;Files=$count;Bytes=$bytes;InventorySha256=$inventoryHash;Applied=[bool]$Apply})
    Write-Verbose "Preflight complete: $canonical ($count files, $bytes bytes)"
}
if ($Apply) {
    # Recheck all files before deletion; no task may write these trees after
    # the caller's quiescence decision. This is not race-proof against an
    # unrelated hostile writer and does not pretend to be an OS lock service.
    foreach ($file in $files) {
        $item = Get-Item -LiteralPath $file.Path -Force
        Assert-NoLink $item
        if ((!$file.Directory -and $item.Length -ne $file.Length) -or $item.LastWriteTimeUtc.Ticks -ne $file.WriteTicks) {
            throw "Target changed during preflight: $($file.Path)"
        }
    }
    foreach ($plan in $plans) {
        Write-Verbose "Removing reviewed target: $($plan.Path)"
        Remove-Item -LiteralPath $plan.Absolute -Recurse -Force
        if (Test-Path -LiteralPath $plan.Absolute) { throw "Residual target: $($plan.Path)" }
    }
}
$plans | Select-Object Path,Owner,Files,Bytes,InventorySha256,Applied
