[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Sha256([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
$build = [IO.Path]::GetFullPath($BuildRoot)
if (-not $build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) { throw "BuildRoot must remain below ${buildBase}: $build" }
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }

$source = Join-Path $repository 'src\opennt-guest\base\mvdm\dos\v86\cmd\share'
$tools = Join-Path $repository 'tools\historical\opennt-4.5'
foreach ($path in @($source, (Join-Path $tools 'masm.exe'), (Join-Path $tools 'link16.exe'))) { if (-not (Test-Path -LiteralPath $path)) { throw "Required primary SHARE input missing: $path" } }

$stage = Join-Path $build 'base\mvdm\dos\v86\cmd\share'
New-Item -ItemType Directory -Path $stage | Out-Null
foreach ($name in @('share.asm','share.lnk','makefile')) { Copy-Item -LiteralPath (Join-Path $source $name) -Destination $stage }
Push-Location $stage
try {
    & (Join-Path $tools 'masm.exe') '/Mx' '/t' '/W1' 'share.asm,share.obj;' 2>&1 | Tee-Object -FilePath (Join-Path $stage 'masm.log')
    if ($LASTEXITCODE -ne 0) { throw "MASM failed: $LASTEXITCODE" }
    & (Join-Path $tools 'link16.exe') '/E' '@share.lnk' 2>&1 | Tee-Object -FilePath (Join-Path $stage 'link16.log')
    if ($LASTEXITCODE -ne 0) { throw "LINK16 failed: $LASTEXITCODE" }
} finally { Pop-Location }

$output = Join-Path $stage 'share.exe'
$expectedHash = '69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc'
if (-not (Test-Path -LiteralPath $output) -or (Get-Item -LiteralPath $output).Length -ne 882 -or (Get-Sha256 $output) -ne $expectedHash) { throw "Primary SHARE identity mismatch: $output" }
$manifest = [ordered]@{
    schema = 'ntvdm64.t225.s8.share-primary-source-build.v1'
    classification = 'primary-original-toolchain-source-built'
    source = [ordered]@{ managedRoot = 'src/opennt-guest'; upstreamRevision = '5c5b979ec08c17d3ca2eb70e8aad62d26515d01c'; guestSourceTransforms = @() }
    tools = [ordered]@{ masm = Get-Sha256 (Join-Path $tools 'masm.exe'); link16 = Get-Sha256 (Join-Path $tools 'link16.exe') }
    output = [ordered]@{ relativePath = 'base/mvdm/dos/v86/cmd/share/SHARE.EXE'; bytes = 882; sha256 = $expectedHash }
}
[IO.File]::WriteAllText((Join-Path $build 'share-build-manifest.json'), (($manifest | ConvertTo-Json -Depth 6) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Host "Primary SHARE source build completed: $output"
