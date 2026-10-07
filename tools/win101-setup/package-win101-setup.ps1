param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$OriginalMedia,
      [string]$ReleaseRoot='',
      [Parameter(Mandatory)][string]$Destination,
      [Parameter(Mandatory)][string]$SetverPath,
      [Parameter(Mandatory)][string]$PifTemplate,
      [string]$PreviousManifest='')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned packaging evidence required'
}
$Destination=[IO.Path]::GetFullPath($Destination).TrimEnd('\')
if($Destination -eq [IO.Path]::GetPathRoot($Destination).TrimEnd('\') -or
   $Destination -eq $repo -or $repo.StartsWith($Destination+'\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Choose a dedicated package directory, not a drive or repository root'
}
if(Test-Path $Destination) {
    if(!$PreviousManifest){throw 'Preserve existing package without proven ownership'}
    $previous=Get-Content $PreviousManifest -Raw|ConvertFrom-Json
    if($previous.destination -ne $Destination){throw 'Prior package target mismatch'}
    foreach($entry in $previous.files) {
        if((Get-FileHash (Join-Path $Destination $entry.path)).Hash -ne $entry.sha256){throw 'Existing package changed; preserve user files'}
    }
}
$source=(Resolve-Path $OriginalMedia).Path
if(!$ReleaseRoot){$ReleaseRoot=Join-Path $repo 'assets/release'}
$driver=(Resolve-Path -LiteralPath $ReleaseRoot).Path
$addon=$PSScriptRoot
$mouseSource=Join-Path $repo 'src/addon/win101-mouse-drv'
foreach($inputPath in @($source,$driver,(Resolve-Path $SetverPath).Path,(Resolve-Path $PifTemplate).Path)) {
    if($inputPath -eq $Destination -or $inputPath.StartsWith($Destination+'\',[StringComparison]::OrdinalIgnoreCase)) {
        throw 'Package destination must not contain its source inputs'
    }
}
foreach($file in @('SETUP.EXE','SETUP.LBL','BUILD.LBL','UTILITY.LBL','KERNEL.EXE','USER.EXE','GDI.EXE','EGAHIRES.DRV','DISK1','DISK2','DISK3','DISK4','DISK5')) {
    if(!(Test-Path "$source/$file")){throw "Required original installer input missing: $file"}
}
New-Item -ItemType Directory -Path $root | Out-Null
$stage=Join-Path $root 'package'
New-Item -ItemType Directory -Path $stage -Force|Out-Null
$patch=Join-Path $stage 'PATCH'
New-Item -ItemType Directory -Path $patch | Out-Null
$original=@(Get-ChildItem -LiteralPath $source -File|ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination $stage
    @{name=$_.Name;sha256=(Get-FileHash $_.FullName).Hash;length=$_.Length}
})
& "$addon/apply-setup.ps1" -MediaRoot $stage -ReleaseRoot $driver -SetverPath $SetverPath -PifTemplate $PifTemplate
Copy-Item -LiteralPath "$mouseSource/mouse101.asm" -Destination $patch
# Generate WORK/PIF only when the owner starts Setup at its real short path.
# Packaging does not create disposable work or use any drive substitution.
$manifest=@(Get-ChildItem $stage -Recurse -File|ForEach-Object {
    @{path=$_.FullName.Substring($stage.Length+1);sha256=(Get-FileHash $_.FullName).Hash;length=$_.Length}
})
[ordered]@{originalMedia=$source;originalInputs=$original;driverSha256=(Get-FileHash "$driver/MOUSE101.DRV").Hash;
    destination=$Destination;files=$manifest;run16='PATH';format='unchanged original flat media with all authored additions/work copies under PATCH'} |
    ConvertTo-Json -Depth 6|Set-Content "$root/package-manifest.json"
foreach($entry in $original) {
    if((Get-FileHash "$source/$($entry.name)").Hash -ne $entry.sha256){throw 'Original media changed'}
}
if(Test-Path $Destination) {
    $item=Get-Item -LiteralPath $Destination
    if($item.Attributes -band [IO.FileAttributes]::ReparsePoint -or
       (Resolve-Path $Destination).Path -ne $Destination){throw 'Unsafe replacement target'}
    # Recoverable whole-folder replacement removes our superseded hierarchy
    # without deleting or losing any original/user-added files.
    Move-Item -LiteralPath $Destination -Destination "$root/previous-package"
}
try {
    New-Item -ItemType Directory -Path $Destination | Out-Null
    Copy-Item -Path "$stage/*" -Destination $Destination -Recurse
    foreach($entry in $manifest) {
        if((Get-FileHash (Join-Path $Destination $entry.path)).Hash -ne $entry.sha256){throw 'Published installer identity mismatch'}
    }
} catch {
    if(Test-Path $Destination){Move-Item -LiteralPath $Destination -Destination "$root/failed-package"}
    if(Test-Path "$root/previous-package"){Move-Item -LiteralPath "$root/previous-package" -Destination $Destination}
    throw
}
"PASS complete merged media/new driver/scripts/PIF/Setup profiles at $Destination; installation not run"
