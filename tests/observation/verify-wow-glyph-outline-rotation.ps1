param(
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [string]$RepositoryRoot,
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)

$ErrorActionPreference = 'Stop'
if (!$RepositoryRoot) {
    $RepositoryRoot = Join-Path $PSScriptRoot '../..'
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$out = [IO.Path]::GetFullPath($BuildRoot)
$vs = (Resolve-Path -LiteralPath $VsDevCmd).Path
$fixture = Join-Path $root 'tests\adapter-mvdm-host-out\wow\wow_glyph_outline_rotation_fixture.c'
$provider = Join-Path $root 'src\wow32-dll\source\wow_public_user_facade.c'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$compile = 'call "' + $vs + '" -arch=x86 -host_arch=x64 >nul && cd /d "' +
    $out + '" && cl /nologo /TC /MT /W4 /WX /I"' + $root + '\src" /I"' + $root + '\src\opennt-abi\source\public\internal\windows\inc" /Fe:wow_glyph_outline_rotation_fixture.exe "' +
    $fixture + '" "' + $provider + '" /link gdi32.lib user32.lib kernel32.lib'
& cmd.exe /d /s /c $compile
if ($LASTEXITCODE) { throw 'x86 WOW glyph-outline fixture build failed.' }
$result = & (Join-Path $out 'wow_glyph_outline_rotation_fixture.exe') 2>&1
$exit = $LASTEXITCODE
[pscustomobject]@{
    kind = 'x86-wow-glyph-outline-ignore-rotation'
    passed = ($exit -eq 0 -and ($result -join "`n") -match 'WOW_GLYPH_OUTLINE_IGNORE_ROTATION_OK')
    exitCode = $exit
    output = @($result)
    fixtureSHA256 = (Get-FileHash -LiteralPath $fixture -Algorithm SHA256).Hash
    providerSHA256 = (Get-FileHash -LiteralPath $provider -Algorithm SHA256).Hash
} | ConvertTo-Json -Depth 3
if ($exit) { exit $exit }
