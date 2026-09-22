[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\\..')).Path,
    [Parameter(Mandatory = $true)] [string]$BuildRoot,
    [string]$NasmExecutable = 'nasm.exe'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$nasm = Get-Command -Name $NasmExecutable -ErrorAction Stop
New-Item -ItemType Directory -Force -Path $build | Out-Null
$source = Join-Path $repository 'tests\\observation\\s44_vdd_bop.asm'
$output = Join-Path $build 'S44BOP.COM'
& $nasm.Source -f bin -o $output $source
if ($LASTEXITCODE -ne 0) { throw 'NASM failed to build S44BOP.COM.' }
@{
    source = 'tests/observation/s44_vdd_bop.asm'
    output = 'S44BOP.COM'
    sha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $output).Hash
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 's44-vdd-bop-manifest.json') -Encoding UTF8
Write-Output "S44 VDD BOP guest probe: $output"
