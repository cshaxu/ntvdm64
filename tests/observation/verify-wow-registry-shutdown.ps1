param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'A fresh repository build output directory is required'
}
New-Item -ItemType Directory $out | Out-Null
$provider = Join-Path $repo 'build/.ninja/msvc-x86-release/wow32'
$source = Join-Path $repo 'tests/mvdm-host/wow_registry_shutdown_fixture.c'
$definition = Join-Path $repo 'tests/mvdm-host/wow_registry_shutdown_fixture.def'
$include = Join-Path $repo 'src/wow32-dll/include'
$object = Join-Path $provider 'obj/wow32-dll/wow_registry_binding.obj'
$ninja = (Get-Command ninja.exe -ErrorAction Stop).Source
& $ninja -C $provider 'obj/wow32-dll/wow_registry_binding.obj'
if ($LASTEXITCODE) { throw 'Could not build selected registry binding object' }
$command = 'call "' + $VsDevCmd + '" -arch=x86 -host_arch=x64 >nul && cl /nologo /W4 /MT /I "' + $include + '" /Fo"' + $out + '\fixture.obj" "' + $source + '" /link /nologo /out:"' + $out + '\fixture.exe" /def:"' + $definition + '" "' + $object + '" kernel32.lib'
& cmd.exe /d /s /c $command
if ($LASTEXITCODE) { throw 'Registry fixture compile or link failed' }
& "$out/fixture.exe" | Tee-Object -FilePath "$out/result.log"
if ($LASTEXITCODE -or !(Select-String -LiteralPath "$out/result.log" -SimpleMatch 'WOW_REGISTRY_POPULATED_SHUTDOWN_OK')) {
    throw 'Registry fixture failed or did not produce its success marker'
}
@($source, $definition, $object, "$out/fixture.exe") | Get-FileHash -Algorithm SHA256 | Export-Csv -NoTypeInformation "$out/inputs.csv"
