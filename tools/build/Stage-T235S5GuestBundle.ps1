[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputRoot,
    [string]$SourceBuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Add-Entry([System.Collections.Generic.List[object]]$Entries, [string]$Kind,
    [string]$Destination, [string]$Source, [string]$BuildRecipe = '') {
    if (Test-Path -LiteralPath $Destination) { throw "Refusing to overwrite staged file: $Destination" }
    Copy-Item -LiteralPath $Source -Destination $Destination
    $item = Get-Item -LiteralPath $Destination
    $Entries.Add([ordered]@{
        destination = $item.FullName.Substring($script:output.Length + 1).Replace('\','/')
        origin = $Kind
        source = if ($Source.StartsWith($script:repository + '\', [StringComparison]::OrdinalIgnoreCase)) {
            $Source.Substring($script:repository.Length + 1).Replace('\','/')
        } elseif ($Source.StartsWith($script:opennt45 + '\', [StringComparison]::OrdinalIgnoreCase)) {
            ('external/OpenNT-4.5/nt/private/mvdm/' + $Source.Substring($script:opennt45.Length + 1)).Replace('\','/')
        } else { $Source.Replace('\','/') }
        buildRecipe = $BuildRecipe
        bytes = $item.Length
        sha256 = (Get-FileHash -LiteralPath $Destination -Algorithm SHA256).Hash.ToLowerInvariant()
    })
}

$script:repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$script:output = [IO.Path]::GetFullPath($OutputRoot)
$opennt = Join-Path $script:repository 'refs\opennt'
$script:opennt45 = 'O:\repos.external\OpenNT-4.5\nt\private\mvdm'
if (!(Test-Path -LiteralPath $opennt)) { throw "OpenNT reference root missing: $opennt" }
if (!(Test-Path -LiteralPath $script:opennt45)) { throw "OpenNT-4.5 product root missing: $script:opennt45" }
if ((Test-Path -LiteralPath (Join-Path $script:output 'dos')) -or
    (Test-Path -LiteralPath (Join-Path $script:output 'wow16')) -or
    (Test-Path -LiteralPath (Join-Path $script:output 'guest-bundle-manifest.json'))) {
    throw "Refusing to overwrite an existing guest bundle below: $script:output"
}

$dos = Join-Path $script:output 'dos'
$wow = Join-Path $script:output 'wow16'
New-Item -ItemType Directory -Force -Path $dos, $wow | Out-Null
$entries = [System.Collections.Generic.List[object]]::new()
$unresolvedFallbacks = [System.Collections.Generic.List[object]]::new()
$extensions = @('.sys','.com','.exe','.dll','.drv','.fon','.pif','.nt')

# Preserve original prebuilt OpenNT payloads whenever they exist.
$dosSource = Join-Path $opennt 'base\mvdm\dos\v86'
Get-ChildItem -LiteralPath $dosSource -Recurse -File |
    Where-Object { $extensions -contains $_.Extension.ToLowerInvariant() } |
    Sort-Object FullName | ForEach-Object {
        $destination = Join-Path $dos $_.Name
        if (Test-Path -LiteralPath $destination) { throw "DOS basename collision requires an explicit package decision: $($_.Name)" }
        Add-Entry $entries 'opennt-prebuilt' $destination $_.FullName
    }

foreach ($name in @('config.nt','autoexec.nt')) {
    Add-Entry $entries 'opennt-prebuilt' (Join-Path $dos $name.ToUpperInvariant()) (Join-Path $opennt "base\mvdm\bin86\$name")
}

# OpenNT-4.5 provides the additional original utility payload and DOSX image.
Get-ChildItem -LiteralPath (Join-Path $script:opennt45 'dos\v86') -Recurse -File |
    Where-Object { $extensions -contains $_.Extension.ToLowerInvariant() } |
    Sort-Object FullName | ForEach-Object {
        $destination = Join-Path $dos $_.Name
        if (!(Test-Path -LiteralPath $destination)) { Add-Entry $entries 'opennt-4.5-prebuilt' $destination $_.FullName }
    }
Add-Entry $entries 'opennt-4.5-prebuilt' (Join-Path $dos 'DOSX.EXE') (Join-Path $script:opennt45 'dpmi\486\dosx.exe')

# LANMAN is a Win16 driver installed with the MVDM support set.
Add-Entry $entries 'opennt-prebuilt' (Join-Path $wow 'LANMAN.DRV') (Join-Path $opennt 'base\mvdm\bin86\lanman.drv')

# WOW has genuine same-name debug/retail artifacts, so preserve source-relative layout.
$wowSource = Join-Path $opennt 'base\mvdm\wow16'
Get-ChildItem -LiteralPath $wowSource -Recurse -File |
    Where-Object { $extensions -contains $_.Extension.ToLowerInvariant() } |
    Sort-Object FullName | ForEach-Object {
        $relative = $_.FullName.Substring($wowSource.Length + 1)
        $destination = Join-Path $wow $relative
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
        Add-Entry $entries 'opennt-prebuilt' $destination $_.FullName
    }

Get-ChildItem -LiteralPath (Join-Path $script:opennt45 'wow16') -Recurse -File |
    Where-Object { $extensions -contains $_.Extension.ToLowerInvariant() } |
    Sort-Object FullName | ForEach-Object {
        $sourceRoot = Join-Path $script:opennt45 'wow16'
        $relative = $_.FullName.Substring($sourceRoot.Length + 1)
        $destination = Join-Path $wow $relative
        if (!(Test-Path -LiteralPath $destination)) {
            New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
            Add-Entry $entries 'opennt-4.5-prebuilt' $destination $_.FullName
        }
    }

Get-ChildItem -LiteralPath (Join-Path $script:opennt45 'bin86') -File |
    Where-Object { $extensions -contains $_.Extension.ToLowerInvariant() } |
    Sort-Object Name | ForEach-Object {
        $destination = Join-Path $wow (Join-Path 'bin86' $_.Name)
        if (!(Test-Path -LiteralPath $destination)) {
            New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
            Add-Entry $entries 'opennt-4.5-prebuilt' $destination $_.FullName
        }
    }

# Original system bitmap fonts used by the hosted Windows/WOW set.
$fontSource = Join-Path $opennt 'base\win32\winnls\fontsup\system'
Get-ChildItem -LiteralPath $fontSource -File -Filter '*.fon' | Sort-Object Name | ForEach-Object {
    $destination = Join-Path $wow (Join-Path 'fonts' $_.Name)
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
    Add-Entry $entries 'opennt-prebuilt' $destination $_.FullName
}

if ($SourceBuildRoot) {
    $sourceBuild = (Resolve-Path -LiteralPath $SourceBuildRoot).Path
    foreach ($item in @(
        [ordered]@{ file='HIMEM.SYS'; recipe='tools/build/Build-T235S5HimemFallback.ps1: OpenNT 4.5 MASM/LINK16/RELOC/STRIPDD' },
        [ordered]@{ file='REDIR.EXE'; recipe='tools/build/Build-T235S5GuestFallbacks.ps1: OpenNT dos/v86/redir/makefile with OpenNT 4.5 MASM/LINK16' }
    )) {
        $source = Join-Path $sourceBuild $item.file
        if (Test-Path -LiteralPath $source -PathType Leaf) {
            Add-Entry $entries 'source-built' (Join-Path $dos $item.file) $source $item.recipe
        } else {
            $unresolvedFallbacks.Add([ordered]@{ file = $item.file; requiredBuildRecipe = $item.recipe })
        }
    }
}

$manifest = [ordered]@{
    schema = 'ntdos64.t235.s5.guest-bundle.v1'
    layout = [ordered]@{ dos = 'dos'; wow16 = 'wow16'; root = 'executable sibling directory' }
    sourcePolicy = 'original OpenNT prebuilt payload first; source-built only when no original payload exists'
    unresolvedFallbacks = $unresolvedFallbacks
    entries = $entries
}
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $script:output 'guest-bundle-manifest.json') -Encoding utf8
Write-Host "Staged $($entries.Count) guest artifacts into $script:output ($(@($entries | Where-Object origin -eq 'opennt-prebuilt').Count) original, $(@($entries | Where-Object origin -eq 'source-built').Count) source-built)"
