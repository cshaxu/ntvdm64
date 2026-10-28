[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
if ((Get-Location).Path -ne $repo) { throw 'Run from the repository root' }
$out = Join-Path $repo 'build/t473-s10/shard-2-wordbreak'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$compile = 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul && cl /nologo /MT /O2 /Gy /Fo:build\t473-s10\shard-2-wordbreak\probe.obj /Fe:build\t473-s10\shard-2-wordbreak\probe.exe artifacts\mvdm-audit\shard-2\parallel-review-fifth-wordbreak-probe.c build\.ninja\msvc-x86-release\wow32\obj\wcall32.obj build\.ninja\msvc-x86-release\wow32\obj\walloc16.obj /link /OPT:REF /MAP:build\t473-s10\shard-2-wordbreak\probe.map user32.lib kernel32.lib ntdll.lib'
$compileOutput = & cmd.exe /c $compile 2>&1
if ($LASTEXITCODE -ne 0) { throw "Probe build failed: $compileOutput" }
$probe = Join-Path $out 'probe.exe'
$ordinary = (& $probe 2>&1 | Out-String).Trim()
$ordinaryCode = $LASTEXITCODE
$strict = (& $probe --strict 2>&1 | Out-String).Trim()
$strictCode = $LASTEXITCODE
$expected = 'length255_adjacent=90 length256_adjacent=0 callbacks=2'
if ($ordinaryCode -ne 0 -or $strictCode -ne 1 -or $ordinary -ne $expected -or $strict -ne $expected) {
    throw "Unexpected result: ordinary=$ordinaryCode [$ordinary], strict=$strictCode [$strict]"
}
$map = Get-Content -Raw (Join-Path $out 'probe.map')
foreach ($entry in @('_W32EditNextWord@20', '_malloc16@4', '_free16@4')) {
    if ($map -notmatch [regex]::Escape($entry)) { throw "Missing linked original object: $entry" }
}
$inputs = @(
    'artifacts/mvdm-audit/shard-2/parallel-review-fifth-wordbreak-probe.c',
    'artifacts/mvdm-audit/shard-2/parallel-review-fifth-wordbreak.ps1',
    'src/mvdm/wow32/wcall32.c', 'src/mvdm/wow32/walloc16.c',
    'src/opennt-host/windows/core/ntuser/client/editec.c',
    'src/opennt-host/windows/core/ntuser/client/edmlrare.c',
    'build/.ninja/msvc-x86-release/wow32/obj/wcall32.obj',
    'build/.ninja/msvc-x86-release/wow32/obj/walloc16.obj',
    'build/t473-s10/shard-2-wordbreak/probe.exe',
    'build/t473-s10/shard-2-wordbreak/probe.map',
    'assets/release/WOW32.DLL'
) | ForEach-Object { [ordered]@{ path = $_; sha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $_).Hash } }
$record = [ordered]@{
    result = 'counterexample-observed'; compile = $compile; build_output = @($compileOutput | ForEach-Object { "$_" });
    observe_exit = $ordinaryCode; observe_stdout = $ordinary; strict_exit = $strictCode; strict_stdout = $strict;
    model = 'Actual selected wcall32/walloc16 objects; synthetic guest mapping and callback, two real 256-byte message-heap allocations';
    limits = 'No guest execution, live WOW session, or product modification; strict exit 1 is the expected contract failure';
    inputs = $inputs
}
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'parallel-review-fifth-wordbreak-evidence.json'),
    (($record | ConvertTo-Json -Depth 8) + "`n"), (New-Object Text.UTF8Encoding($false)))
Write-Output $expected
Write-Output "observe_exit=$ordinaryCode strict_exit=$strictCode"
