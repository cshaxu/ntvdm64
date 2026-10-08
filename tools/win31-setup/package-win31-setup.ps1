param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$OriginalMedia,
    [Parameter(Mandatory)][string]$Destination,
    [Parameter(Mandatory)][string]$NtDos,
    [Parameter(Mandatory)][string]$PifTemplate,
    [Parameter(Mandatory)][string]$OriginalWin386,
    [string]$ReleaseRoot = '',
    [string]$PreviousManifest = ''
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path

# The only available source PIF is a functional historical profile and may
# therefore contain the path of its former installation.  PATCHSET overwrites
# every path-bearing field before it writes either installed PIF, but keep the
# embedded resource itself path-neutral as well: a shipped executable must not
# retain that former path as dead template data.
function New-PathNeutralPifTemplate([string]$Source, [string]$Destination) {
    $bytes = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $Source).Path)
    if($bytes.Length -lt 0x171) {throw 'Truncated PIF template'}
    function Clear-Field([int]$Offset, [int]$Length) {
        if($Offset -lt 0 -or $Length -lt 1 -or $Offset + $Length -gt $bytes.Length) {
            throw 'PIF field outside template'
        }
        [Array]::Clear($bytes, $Offset, $Length)
    }
    Clear-Field 2 30; Clear-Field 0x24 63; Clear-Field 0x65 64; Clear-Field 0xa5 64
    $position = 0x171; $seen = @{}; $nt = $false; $w386 = $false
    while($position -ne 0xffff) {
        if($seen.ContainsKey($position) -or $position + 22 -gt $bytes.Length) {throw 'Invalid PIF extension chain'}
        $seen[$position] = $true
        $signature = [Text.Encoding]::ASCII.GetString($bytes, $position, 16).Trim([char]0)
        $next = [BitConverter]::ToUInt16($bytes, $position + 16)
        $data = [BitConverter]::ToUInt16($bytes, $position + 18)
        $length = [BitConverter]::ToUInt16($bytes, $position + 20)
        if($data + $length -gt $bytes.Length) {throw 'Truncated PIF extension'}
        if($signature -eq 'WINDOWS 386 3.0') {
            if($length -lt 104) {throw 'Short Windows 386 PIF extension'}
            Clear-Field ($data + 40) 64; $w386 = $true
        }
        if($signature -eq 'WINDOWS NT  3.1') {
            if($length -lt 140) {throw 'Short Windows NT PIF extension'}
            Clear-Field ($data + 12) 64; Clear-Field ($data + 76) 64; $nt = $true
        }
        $position = $next
    }
    if(!$nt -or !$w386) {throw 'Missing required PIF extension'}
    $sum = 0; for($i = 2; $i -lt 0x171; $i++) {$sum = ($sum + $bytes[$i]) -band 255}; $bytes[1] = [byte]$sum
    [IO.File]::WriteAllBytes($Destination, $bytes)
}
$build = [IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$build.StartsWith((Join-Path $repo 'build') + '\', [StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $build)) {throw 'Use a fresh build-owned packaging output'}
$source = (Resolve-Path -LiteralPath $OriginalMedia).Path.TrimEnd('\')
$destination = [IO.Path]::GetFullPath($Destination).TrimEnd('\')
if($destination -eq [IO.Path]::GetPathRoot($destination).TrimEnd('\') -or $destination -eq $repo -or $repo.StartsWith($destination + '\', [StringComparison]::OrdinalIgnoreCase)) {throw 'Choose a dedicated package directory, not a drive or repository root'}
if(!$ReleaseRoot) {$ReleaseRoot = Join-Path $repo 'assets/release'}
$release = (Resolve-Path -LiteralPath $ReleaseRoot).Path
if($source -eq $destination -or $source.StartsWith($destination + '\', [StringComparison]::OrdinalIgnoreCase)) {throw 'Package destination must not contain source media'}
if((Test-Path -LiteralPath $destination) -and !$PreviousManifest) {throw 'Preserve existing package without a proven previous manifest'}
if(Test-Path -LiteralPath $destination) {
    $previous = Get-Content -LiteralPath $PreviousManifest -Raw | ConvertFrom-Json
    if($previous.destination -ne $destination) {throw 'Previous package target mismatch'}
    foreach($entry in $previous.files) {if((Get-FileHash -LiteralPath (Join-Path $destination $entry.path)).Hash -ne $entry.sha256) {throw 'Existing package changed; preserve user files'}}
}
New-Item -ItemType Directory -Path $build | Out-Null
$stage = Join-Path $build 'package'; New-Item -ItemType Directory -Path $stage | Out-Null
# PATCH is authored delivery state, never original media.  A source directory
# may carry an older user package; do not copy it into a fresh package and do
# not mutate it.  apply-setup.ps1 creates a new, minimal PATCH in $stage.
Get-ChildItem -LiteralPath $source -Force | Where-Object {$_.Name -ine 'PATCH'} | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination $stage -Recurse -Force}
$adaptation = Join-Path $build 'retail-adaptation'
& (Join-Path $PSScriptRoot 'adapt-retail-dosmgr.ps1') -OriginalWin386 $OriginalWin386 -NtDos $NtDos -OutputRoot $adaptation
$krnlAdaptation = Join-Path $build 'retail-krnl386-adaptation'
$krnlRetailRoot = Join-Path $build 'retail-krnl386'
$retailKrnl = Join-Path $krnlRetailRoot 'KRNL386.EXE'
New-Item -ItemType Directory -Path $krnlRetailRoot | Out-Null
& expand.exe (Join-Path $stage 'KRNL386.EX_') $retailKrnl | Out-Null
if($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath $retailKrnl)) {throw 'Could not expand original KRNL386.EX_'}
& (Join-Path $PSScriptRoot 'adapt-retail-krnl386.ps1') -OriginalKrnl386 $retailKrnl -OutputRoot $krnlAdaptation
$szddBuild = Join-Path $build 'szddpack'
New-Item -ItemType Directory -Path $szddBuild | Out-Null
$szddPack = Join-Path $szddBuild 'SZDDPACK.EXE'
$szddBuildSource = Join-Path $repo 'src/addon/win31-setup/BUILD-SZDDPACK.CMD'
& cmd.exe /d /c ('"{0}" "{1}" "{2}"' -f $szddBuildSource,$szddPack,$szddBuild)
if($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath $szddPack)) {throw 'SZDDPACK build failed'}

# Setup selects the retail compressed filenames.  Replace those exact source
# representations in the build-owned copy before Setup gets a chance to copy
# and load either protected-mode image; an installed PATCH would run too late.
$expandedWin386 = Join-Path $build 'retail-win386/WIN386.EXE'
New-Item -ItemType Directory -Path (Split-Path -Parent $expandedWin386) | Out-Null
& expand.exe (Join-Path $stage 'WIN386.EX_') $expandedWin386 | Out-Null
if($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath $expandedWin386) -or
   (Get-FileHash -LiteralPath $expandedWin386).Hash -ne (Get-FileHash -LiteralPath $OriginalWin386).Hash) {
    throw 'Source WIN386.EX_ does not expand to the declared retail input'
}
foreach($replacement in @(
    @{compressed=(Join-Path $stage 'KRNL386.EX_'); expanded=(Join-Path $krnlAdaptation 'KRNL386.EXE'); check='KRNL386.EXE'},
    @{compressed=(Join-Path $stage 'WIN386.EX_'); expanded=(Join-Path $adaptation 'WIN386.EXE'); check='WIN386.EXE'}
)) {
    Remove-Item -LiteralPath $replacement.compressed -Force
    & $szddPack $replacement.expanded $replacement.compressed
    if($LASTEXITCODE -ne 0) {throw "Could not write derived $($replacement.compressed)"}
    $expandedCheck = Join-Path $build ("expanded-check/" + $replacement.check)
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $expandedCheck) | Out-Null
    if(Test-Path -LiteralPath $expandedCheck) {Remove-Item -LiteralPath $expandedCheck -Force}
    & expand.exe $replacement.compressed $expandedCheck | Out-Null
    if($LASTEXITCODE -ne 0 -or
       (Get-FileHash -LiteralPath $expandedCheck).Hash -ne (Get-FileHash -LiteralPath $replacement.expanded).Hash) {
        throw "Derived $($replacement.compressed) does not expand exactly"
    }
}
$patchsetRoot = Join-Path $build 'patchset'
New-Item -ItemType Directory -Path $patchsetRoot | Out-Null
$neutralPif = Join-Path $patchsetRoot 'PIF-TEMPLATE.BIN'
New-PathNeutralPifTemplate $PifTemplate $neutralPif
$patchsetSource = Join-Path $repo 'src/addon/win31-setup/BUILD.CMD'
& cmd.exe /d /c ('"{0}" "{1}" "{2}" "{3}"' -f $patchsetSource,$neutralPif,(Join-Path $patchsetRoot 'PATCHSET.EXE'),$patchsetRoot)
if($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath (Join-Path $patchsetRoot 'PATCHSET.EXE'))) {throw 'PATCHSET build failed'}
& (Join-Path $PSScriptRoot 'apply-setup.ps1') -MediaRoot $stage -ReleaseRoot $release -Patchset (Join-Path $patchsetRoot 'PATCHSET.EXE') -AdaptedWin386 (Join-Path $adaptation 'WIN386.EXE') -AdaptedKrnl386 (Join-Path $krnlAdaptation 'KRNL386.EXE') -RetailWin386 $expandedWin386 -RetailKrnl386 $retailKrnl
$manifest = @(Get-ChildItem -LiteralPath $stage -Recurse -File | ForEach-Object {
    [ordered]@{path=$_.FullName.Substring($stage.Length + 1);sha256=(Get-FileHash -LiteralPath $_.FullName).Hash;length=$_.Length}
})
[ordered]@{
    originalMedia=$source
    destination=$destination
    format='build-owned derived media; only KRNL386.EX_ and WIN386.EX_ replace retail compressed representations'
    originalKrnl386Sha256=(Get-FileHash -LiteralPath $retailKrnl).Hash
    replacementKrnl386Sha256=(Get-FileHash -LiteralPath (Join-Path $krnlAdaptation 'KRNL386.EXE')).Hash
    originalWin386Sha256=(Get-FileHash -LiteralPath $OriginalWin386).Hash
    replacementWin386Sha256=(Get-FileHash -LiteralPath (Join-Path $adaptation 'WIN386.EXE')).Hash
    files=$manifest
} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 'package-manifest.json') -Encoding UTF8
if(Test-Path -LiteralPath $destination) {Move-Item -LiteralPath $destination -Destination (Join-Path $build 'previous-package')}
try {
    New-Item -ItemType Directory -Path $destination | Out-Null
    Get-ChildItem -LiteralPath $stage -Force | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination $destination -Recurse -Force
    }
    foreach($entry in $manifest) {if((Get-FileHash -LiteralPath (Join-Path $destination $entry.path)).Hash -ne $entry.sha256) {throw 'Published package identity mismatch'}}
} catch {
    if(Test-Path -LiteralPath $destination) {Move-Item -LiteralPath $destination -Destination (Join-Path $build 'failed-package')}
    if(Test-Path -LiteralPath (Join-Path $build 'previous-package')) {Move-Item -LiteralPath (Join-Path $build 'previous-package') -Destination $destination}
    throw
}
Write-Host "PASS derived Win3.1 media/PATCH package at $destination; installation not run"
