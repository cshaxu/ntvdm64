[CmdletBinding()]
param([string] $OutputRoot = (Join-Path $PSScriptRoot '../../build/serial-medium'))
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$OutputRoot = [IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Force $OutputRoot | Out-Null
& "$repo/tools/build-fixtures.cmd" x86 serial-medium *> "$OutputRoot/build.log"
if ($LASTEXITCODE) { throw "Serial medium build failed; see $OutputRoot/build.log" }
$exe = "$repo/build/.ninja/fixtures-x86/serial-medium.exe"
$output = & $exe 2>&1
$code = $LASTEXITCODE
$output | Set-Content "$OutputRoot/run.log"
if ($code -ne 0 -or $output -notcontains 'T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK') {
    throw "Serial medium fixture failed ($code); see $OutputRoot/run.log"
}
Write-Output "Original serial medium open/write/read/close contract passed: $exe"
