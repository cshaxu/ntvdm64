$ErrorActionPreference = 'Stop'

$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$generator = Get-Content (Join-Path $repository 'tools\build\New-T98S1CurrentAdapterEngineDerivative.ps1') -Raw
foreach ($required in @(
        '[switch]$DeferredStartupPlan',
        "@{ source = 'refs\bochs\main.cc'; destination = 'main.cc'; object = 'main.o' }",
        "@{ source = 'src\bx-core\cpu\exception.cc'; destination = 'cpu\exception.cc'; object = 'cpu\exception.o' }",
        'BX_NTVDM_ENABLE_EXECUTION_PLAN=0',
        'BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1',
        'Deferred derivative must rebuild exactly two Bochs objects.',
        'Default derivative unexpectedly rebuilds a Bochs object.')) {
    if (-not $generator.Contains($required)) {
        throw "T104 deferred derivative lacks required closure term: $required"
    }
}
foreach ($forbidden in @('iodev\\.*:.*\\.cc', 'cpu\\libcpu\.a:')) {
    if ($generator -match $forbidden) {
        throw "T104 deferred derivative expands a prohibited engine build input: $forbidden"
    }
}
Write-Output 'T104 current-adapter deferred derivative lexical closure passed.'
