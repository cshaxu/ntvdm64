[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$matrix = Join-Path $root 'docs/etc/operations/m0-t290-s5-remaining-provider-disposition-matrix.tsv'
$rows = Import-Csv -LiteralPath $matrix -Delimiter "`t"
if ($rows.Count -ne 27) {
    throw "Expected 27 S5 provider disposition rows, found $($rows.Count)."
}

$builders = @(
    @{ Name = 'local'; Script = 'New-T290S5VrNetApiLocalNinja.ps1'; Exe = 't290-s5-vrnetapi-local-fixture.exe' },
    @{ Name = 'computer-name'; Script = 'New-T290S5VrNetApiComputerNameNinja.ps1'; Exe = 't290-s5-vrnetapi-computer-name-fixture.exe' },
    @{ Name = 'user-name'; Script = 'New-T290S5VrNetApiUserNameNinja.ps1'; Exe = 't290-s5-vrnetapi-user-name-fixture.exe' }
)

foreach ($architecture in @('x86', 'x64')) {
    foreach ($builder in $builders) {
        & (Join-Path $PSScriptRoot $builder.Script) -Architecture $architecture -RepositoryRoot $root
        if (-not $?) {
            throw "Failed to generate $($builder.Name) $architecture graph."
        }

        $build = Join-Path $root ("build/M0-T290/S5/vrnetapi-{0}-{1}" -f $builder.Name, $architecture)
        & ninja -C $build
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to build $($builder.Name) $architecture fixture."
        }

        $fixture = Join-Path $build (Join-Path 'bin' $builder.Exe)
        & $fixture
        if ($LASTEXITCODE -ne 0) {
            throw "Failed $($builder.Name) $architecture fixture with exit $LASTEXITCODE."
        }
    }
}

Write-Host 'T290 S5 remaining-provider matrix passed: 27 disposition rows and 3 original-body fixtures on x86/x64.'
