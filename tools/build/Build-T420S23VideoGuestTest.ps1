[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [ValidateSet('int10', 'direct-vram', 'graphics-vram')][string]$GuestRoute = 'int10',
    [string]$NasmExecutable = 'nasm.exe'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
if (!$build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar,
        [StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildRoot must remain below ${buildBase}: $build"
}
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build root: $build"
}

$nasm = Get-Command -Name $NasmExecutable -ErrorAction Stop
New-Item -ItemType Directory -Path $build | Out-Null
$sourceName = switch ($GuestRoute) {
    'direct-vram' { 'video_direct_vram.asm' }
    'graphics-vram' { 'video_graphics_vram.asm' }
    default { 'video_int10_writer.asm' }
}
$source = Join-Path $repository ('tests\observation\' + $sourceName)
$output = Join-Path $build 'VIDTST.COM'
& $nasm.Source -f bin -o $output $source
if ($LASTEXITCODE -ne 0) {
    throw "NASM failed for video guest fixture: $LASTEXITCODE"
}

[ordered]@{
    schema = 'm0.t420.s23.video-guest.v2'
    route = $GuestRoute
    source = 'tests/observation/' + $sourceName
    output = 'VIDTST.COM'
    sha256 = (Get-FileHash -LiteralPath $output -Algorithm SHA256).Hash.ToLowerInvariant()
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 'manifest.json') -Encoding utf8

Write-Host "S23 $GuestRoute guest fixture completed: $output"
