[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Engine,
    [Parameter(Mandatory = $true)][string]$Bios,
    [Parameter(Mandatory = $true)][string]$VgaBios,
    [Parameter(Mandatory = $true)][string]$OptionRom,
    [Parameter(Mandatory = $true)][string]$BundleRoot
)

$ErrorActionPreference = 'Stop'
$bundle = [IO.Path]::GetFullPath($BundleRoot)
if (Test-Path -LiteralPath $bundle) { throw "Refusing to overwrite existing bundle directory: $bundle" }

function Resolve-Input([string]$Path, [string]$Name) {
    $resolved = (Resolve-Path -LiteralPath $Path -ErrorAction Stop).Path
    if (-not (Test-Path -LiteralPath $resolved -PathType Leaf)) { throw "$Name is not a file: $Path" }
    return $resolved
}
function Copy-Verified([string]$Source, [string]$Destination) {
    $hash = (Get-FileHash -LiteralPath $Source -Algorithm SHA256).Hash
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    if ((Get-FileHash -LiteralPath $Destination -Algorithm SHA256).Hash -ne $hash) {
        throw "Copy hash mismatch: $Source"
    }
    return $hash
}

$engine = Resolve-Input $Engine 'Engine'
$bios = Resolve-Input $Bios 'BIOS'
$vga = Resolve-Input $VgaBios 'VGA BIOS'
$option = Resolve-Input $OptionRom 'Option ROM'
New-Item -ItemType Directory -Path $bundle, (Join-Path $bundle 'ROM') -ErrorAction Stop | Out-Null
$engineHash = Copy-Verified $engine (Join-Path $bundle 'runner-bochs.exe')
$biosHash = Copy-Verified $bios (Join-Path $bundle 'ROM\BIOS-bochs-latest')
$vgaHash = Copy-Verified $vga (Join-Path $bundle 'ROM\VGABIOS-lgpl-latest')
$optionHash = Copy-Verified $option (Join-Path $bundle 'ROM\runner-post-handoff.rom')
$manifest = @(
    'schema=runner-bochs-engine-bundle-v1',
    "engine-sha256=$engineHash",
    "bios-sha256=$biosHash",
    "vga-sha256=$vgaHash",
    "optionrom-sha256=$optionHash"
) -join "`n"
[IO.File]::WriteAllText((Join-Path $bundle 'runner-bochs-engine-bundle-v1.txt'), $manifest + "`n", [Text.UTF8Encoding]::new($false))
Write-Host "Prepared fixed Bochs bundle: $bundle"
