param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$CompilerCache = ''
)
$ErrorActionPreference = 'Stop'
$cmake = (Get-Command cmake.exe -ErrorAction Stop).Source
$arguments = @("-DNTVDM_BINARY_DIR=$([IO.Path]::GetFullPath($BuildRoot))")
if ($CompilerCache) {
    $arguments += '-DNTVDM_USE_CCACHE=ON'
    $arguments += "-DNTVDM_CCACHE_EXECUTABLE=$((Resolve-Path -LiteralPath $CompilerCache).Path)"
}
& $cmake @arguments -P "$PSScriptRoot/BuildTree.cmake" | Out-Host
if ($LASTEXITCODE) { throw 'Ninja tree configuration failed' }
# The CMake configuration owns validation and the complete per-command cache
# environment. Direct ninja does not depend on the runner's environment.
([IO.File]::ReadAllText((Join-Path $BuildRoot 'compiler-command.txt'))).Replace('\','/').Replace(':','$:')
