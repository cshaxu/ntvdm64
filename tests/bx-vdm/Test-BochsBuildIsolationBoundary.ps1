[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}

$makefile = Join-Path $RepositoryRoot 'src\bochs\Makefile.in'
if (-not (Test-Path -LiteralPath $makefile -PathType Leaf)) {
    throw "Missing Bochs makefile template: $makefile"
}

$source = Get-Content -LiteralPath $makefile -Raw
if ($source -match '(?i)bcrypt') {
    throw 'Bochs build template must not link CLI/BYOB BCrypt capability.'
}

Write-Output 'Bochs build isolation boundary verification passed.'
