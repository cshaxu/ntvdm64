[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$expected = @('cmd', 'cmddata', 'cmddisp', 'cmdexec', 'cmdexit', 'cmdmisc', 'cmdpif', 'cmdredir', 'cmdconf', 'cmdkeyb', 'cmdenv')
foreach ($architecture in @('x86', 'x64')) {
    $build = Join-Path $root "build\M0-T282\S1\$architecture"
    $archive = Join-Path $build 'original-command-surface.lib'
    $graph = Join-Path $build 'build.ninja'
    if (-not (Test-Path -LiteralPath $archive)) { throw "Missing $architecture COMMAND archive." }
    $content = Get-Content -Raw -LiteralPath $graph
    foreach ($unit in $expected) {
        if ($content -notmatch ([regex]::Escape("dos/command/$unit.c"))) {
            throw "$architecture graph omits original COMMAND unit $unit."
        }
    }
    foreach ($forbidden in @('adapter-bop', 'bochs-core', 'adapter-bochs', 'nt_bop.c', '/src/app/')) {
        if ($content -match [regex]::Escape($forbidden)) {
            throw "$architecture COMMAND graph contains forbidden input $forbidden."
        }
    }
}
Write-Output 'T282 S4 complete original COMMAND static-package verification passed.'
