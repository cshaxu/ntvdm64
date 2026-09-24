param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot '../..'),
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$out = [IO.Path]::GetFullPath($BuildRoot)
$vs = (Resolve-Path -LiteralPath $VsDevCmd).Path
$source = Join-Path $root 'tests\adapter-mvdm-host-out\wow\wow_gdi_handle_abi_probe.c'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$compile = 'call "' + $vs + '" -arch=x86 -host_arch=x64 >nul && cd /d "' +
    $out + '" && cl /nologo /TC /MT /W4 /Gd /Fe:wow_gdi_handle_abi_probe.exe "' +
    $source + '" /link gdi32.lib kernel32.lib'
& cmd.exe /d /s /c $compile
if ($LASTEXITCODE) { throw 'x86 GDI handle-contract probe build failed.' }
$result = & (Join-Path $out 'wow_gdi_handle_abi_probe.exe') 2>&1
$exit = $LASTEXITCODE
[pscustomobject]@{
    kind = 'x86-modern-gdi-query-table-contract-observation'
    directContract = ($exit -eq 0)
    exitCode = $exit
    output = @($result)
    sourceSHA256 = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
} | ConvertTo-Json -Depth 3
