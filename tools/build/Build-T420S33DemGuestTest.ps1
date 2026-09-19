[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
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
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
$nasm = Get-Command -Name $NasmExecutable -ErrorAction Stop
New-Item -ItemType Directory -Path $build | Out-Null
$source = Join-Path $repository 'tests\observation\dem_capability.asm'
$output = Join-Path $build 'D33.COM'
& $nasm.Source -f bin -o $output $source
if ($LASTEXITCODE -ne 0) { throw "NASM guest fixture failed for S33 DEM: $LASTEXITCODE" }
[ordered]@{
    schema = 'm0.t420.s33.dem-guest.v1'
    source = 'tests/observation/dem_capability.asm'
    outputs = @([ordered]@{ path = 'D33.COM'; sha256 = (Get-FileHash -LiteralPath $output -Algorithm SHA256).Hash.ToLowerInvariant() })
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 'manifest.json') -Encoding utf8
Write-Host "S33 DEM guest fixture completed: $output"
