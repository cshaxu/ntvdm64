[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$DerivativeRoot,
    [Parameter(Mandatory = $true)][string]$RomRoot,
    [Parameter(Mandatory = $true)][string]$BundleRoot
)

$ErrorActionPreference = 'Stop'
$derivative = [IO.Path]::GetFullPath($DerivativeRoot)
$rom = [IO.Path]::GetFullPath($RomRoot)
$bundle = [IO.Path]::GetFullPath($BundleRoot)
if (Test-Path -LiteralPath $bundle) { throw "Refusing to overwrite existing bundle: $bundle" }
function Resolve-File([string]$Path) { if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing file: $Path" }; [IO.Path]::GetFullPath($Path) }
function Hash([string]$Path) { (Get-FileHash -LiteralPath (Resolve-File $Path) -Algorithm SHA256).Hash }
$engine = Resolve-File (Join-Path $derivative 'ntdos64-t188-copied-state.exe')
$bios = Resolve-File (Join-Path $rom 'BIOS-bochs-latest')
$vga = Resolve-File (Join-Path $rom 'VGABIOS-lgpl-latest')
$optionRom = Resolve-File (Join-Path $rom 'ntdos64-post-handoff.rom')
New-Item -ItemType Directory -Path $bundle -ErrorAction Stop | Out-Null
Copy-Item -LiteralPath $rom -Destination (Join-Path $bundle 'ROM') -Recurse -ErrorAction Stop
Copy-Item -LiteralPath $engine -Destination (Join-Path $bundle 'ntdos64-bochs.exe') -ErrorAction Stop
$manifest = @(
    'schema=ntdos64-bochs-engine-bundle-v1'
    "engine-sha256=$(Hash (Join-Path $bundle 'ntdos64-bochs.exe'))"
    "bios-sha256=$(Hash (Join-Path $bundle 'ROM\BIOS-bochs-latest'))"
    "vga-sha256=$(Hash (Join-Path $bundle 'ROM\VGABIOS-lgpl-latest'))"
    "optionrom-sha256=$(Hash (Join-Path $bundle 'ROM\ntdos64-post-handoff.rom'))"
) -join "`r`n"
[IO.File]::WriteAllText((Join-Path $bundle 'ntdos64-bochs-engine-bundle-v1.txt'),
    ($manifest + "`r`n"), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared immutable T189 S5 bundle: $bundle"
