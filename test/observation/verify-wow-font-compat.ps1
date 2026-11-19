param(
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [string]$ProviderObject,
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot '../..'),
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$out = [IO.Path]::GetFullPath($BuildRoot)
New-Item -ItemType Directory -Force $out | Out-Null
& "$PSScriptRoot/verify-wow-glyph-outline-rotation.ps1" -BuildRoot "$out/glyph" -RepositoryRoot $root -VsDevCmd $VsDevCmd -ProviderObject $ProviderObject
if ($LASTEXITCODE) { exit $LASTEXITCODE }
foreach ($case in @(
    @('etm', 'wow_gdi_etm_fixture.c', ''),
    @('etm-native', 'wow_gdi_etm_fixture.c', '/DWOW_ETM_NATIVE'),
    @('tracking', 'wow_font_tracking_fixture.c', ''),
    @('tracking-search', 'wow_font_tracking_fixture.c', '/DWOW_FONT_NATIVE_SEARCH')
)) {
    $source = Join-Path $root "test/adapter-mvdm-host-out/wow/$($case[1])"
    $command = 'call "' + $VsDevCmd + '" -arch=x86 -host_arch=x64 >nul && cd /d "' +
        $out + '" && cl /nologo /TC /MT /W4 /Gz /external:anglebrackets /external:W0 ' + $case[2] + ' /I"' + $root +
        '/src" /Fe:"' + $case[0] + '.exe" "' + $source +
        '" /link gdi32.lib user32.lib kernel32.lib'
    & cmd.exe /d /s /c $command
    if ($LASTEXITCODE) { throw "Font fixture build failed: $($case[0])" }
    & (Join-Path $out "$($case[0]).exe")
    if ($LASTEXITCODE) { throw "Font fixture failed: $($case[0])" }
}
