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
if (!$build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar,
        [StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildRoot must remain below ${buildBase}: $build"
}
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
$nasm = Get-Command -Name $NasmExecutable -ErrorAction Stop
New-Item -ItemType Directory -Path $build | Out-Null
$source = Join-Path $repository 'tests\observation\keymouse_capability.asm'
$output = Join-Path $build 'KMTST.COM'
& $nasm.Source -f bin -o $output $source
if ($LASTEXITCODE -ne 0) { throw "NASM failed for S25 keymouse fixture: $LASTEXITCODE" }
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs)) { throw "VS x86 build environment is unavailable: $vs" }
$observerSource = Join-Path $repository 'tests\observation\keymouse_capability_observer.c'
$observerOutput = Join-Path $build 'keymouse-capability-observer.exe'
& cmd.exe /d /s /c ('call "' + $vs + '" -arch=x86 -host_arch=x86 >nul && cl.exe /nologo /TC /W4 /MT /Fe:"' + $observerOutput + '" "' + $observerSource + '" /link kernel32.lib user32.lib')
if ($LASTEXITCODE -ne 0) { throw "MSVC failed for S25 keymouse observer: $LASTEXITCODE" }
[ordered]@{
    schema = 'm0.t420.s25.keymouse-guest.v1'
    source = 'tests/observation/keymouse_capability.asm'
    output = 'KMTST.COM'
    sha256 = (Get-FileHash -LiteralPath $output -Algorithm SHA256).Hash.ToLowerInvariant()
    observer = 'keymouse-capability-observer.exe'
    observerSha256 = (Get-FileHash -LiteralPath $observerOutput -Algorithm SHA256).Hash.ToLowerInvariant()
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 'manifest.json') -Encoding utf8
Write-Host "S25 keymouse guest fixture completed: $output"
