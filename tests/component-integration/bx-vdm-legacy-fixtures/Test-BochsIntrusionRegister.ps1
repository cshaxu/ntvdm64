[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}

$register = Join-Path $RepositoryRoot 'docs\etc\research\adapter-external-intrusion-exceptions.md'
$config = Join-Path $RepositoryRoot 'refs\bochs\config.cc'
foreach ($path in @($register, $config)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing registered-intrusion input: $path" }
}

$registerText = Get-Content -LiteralPath $register -Raw
if ($registerText -notmatch 'BX-BUILD-002') { throw 'BX-BUILD-002 is not registered.' }

$configText = Get-Content -LiteralPath $config -Raw
if ($configText -notmatch 'get_param_string\("model", base\)->getptr\(\) != 0') {
    throw 'The registered GCC 16 null-pointer correction is absent.'
}
if ($configText -match 'get_param_string\("model", base\)->getptr\(\)>0') {
    throw 'The rejected ordered pointer comparison remains active.'
}

Write-Host 'Bochs intrusion register verification passed.'
