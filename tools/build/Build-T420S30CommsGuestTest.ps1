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
if (!$build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildRoot must remain below ${buildBase}: $build"
}
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
$nasm = Get-Command -Name $NasmExecutable -ErrorAction Stop
New-Item -ItemType Directory -Path $build | Out-Null
$fixtures = @(
    @{ Source = 'tests\observation\comms_host_medium.asm'; Output = 'C30.COM' },
    @{ Source = 'tests\observation\comms_loopback.asm'; Output = 'C31.COM' },
    @{ Source = 'tests\observation\lpt_host_medium.asm'; Output = 'L30.COM' }
)
foreach ($fixture in $fixtures) {
    $source = Join-Path $repository $fixture.Source
    $output = Join-Path $build $fixture.Output
    & $nasm.Source -f bin -o $output $source
    if ($LASTEXITCODE -ne 0) { throw "NASM fixture failed: $($fixture.Output): $LASTEXITCODE" }
}
[ordered]@{
    schema = 'm0.t420.s30.comms-guest.v1'
    outputs = @($fixtures | ForEach-Object {
        [ordered]@{ path = $_.Output; sha256 = (Get-FileHash -LiteralPath (Join-Path $build $_.Output) -Algorithm SHA256).Hash.ToLowerInvariant() }
    })
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 'manifest.json') -Encoding utf8
Write-Host "S30 COM/LPT guest fixtures completed: $build"
