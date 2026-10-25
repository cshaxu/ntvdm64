param(
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [string]$RepositoryRoot,
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)

$ErrorActionPreference = 'Stop'
if (!$RepositoryRoot) {
    $RepositoryRoot = Join-Path $PSScriptRoot '../..'
}
$repo = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$out = [IO.Path]::GetFullPath($BuildRoot)
$source = Join-Path $repo 'tests\adapter-mvdm-host-out\wow\wow_metafile_host_fixture.c'
$vs = (Resolve-Path -LiteralPath $VsDevCmd).Path
New-Item -ItemType Directory -Force -Path $out | Out-Null
$compile = 'call "' + $vs + '" -arch=x86 -host_arch=x64 >nul && cd /d "' +
    $out + '" && cl /nologo /TC /MT /W4 /WX /Fe:wow_metafile_host_fixture.exe "' +
    $source + '" /link gdi32.lib user32.lib kernel32.lib'
& cmd.exe /d /s /c $compile
if ($LASTEXITCODE) { throw 'x86 WOW metafile host fixture build failed.' }
$path = Join-Path $out 'roundtrip.wmf'
$result = & (Join-Path $out 'wow_metafile_host_fixture.exe') $path 2>&1
$exit = $LASTEXITCODE
[pscustomobject]@{
    kind = 'x86-wow-metafile-public-host'
    passed = ($exit -eq 0 -and ($result -join "`n") -match 'WOW_METAFILE_PUBLIC_HOST_OK')
    exitCode = $exit
    output = @($result)
    sourceSHA256 = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
} | ConvertTo-Json -Depth 3
if ($exit) { exit $exit }
