[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,
      [Parameter(Mandatory)][string]$WorkerBuildRoot,
      [Parameter(Mandatory)][string]$WowBuildRoot)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$out = [IO.Path]::GetFullPath((Join-Path $root $OutputRoot))
if (!$out.StartsWith(([IO.Path]::GetFullPath((Join-Path $root 'build')) + '\'),
    [StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Use a fresh directory below build/.'
}
New-Item -ItemType Directory -Path $out | Out-Null
$worker = Get-Content (Join-Path $WorkerBuildRoot 'build.ninja') -Raw
$wow = Get-Content (Join-Path $WowBuildRoot 'build.ninja') -Raw
if ($worker -notmatch 'src/lib-win32/crt-compat/crt_compat\.c' -or
    $worker -match 'src/ntvdm-exe/win32/crt_compat\.c' -or
    $worker -notmatch 'build ntvdm\.exe[^\r\n]*lib-crt-compat\.lib' -or
    $wow -notmatch 'src/lib-win32/crt-compat/crt_compat\.c' -or
    $wow -match 'src/ntvdm-exe/win32/crt_compat\.c') {
    throw 'Production CRT source ownership mismatch.'
}
$workerMap = Get-Content (Join-Path $WorkerBuildRoot 'ntvdm.exe.map') -Raw
$wowMap = Get-Content (Join-Path $WowBuildRoot 'wow32.dll.map') -Raw
foreach ($symbol in @('_sprintf','_sscanf')) {
    if ($workerMap -notmatch ("(?m)^.*\s" + $symbol + "\s+.*lib-crt-compat:crt_compat\.obj\s*$")) {
        throw "NTVDM CRT provider mismatch: $symbol"
    }
}
if ($wowMap -notmatch '(?m)^.*\s_sprintf\s+.*crt_compat\.obj\s*$') {
    throw 'WOW32 CRT provider mismatch: sprintf'
}
$wrapper = Join-Path $out 'compiler.cmd'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off', ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', '%*') | Set-Content $wrapper -Encoding ASCII
$test = Join-Path $out 'test.obj'
$provider = Join-Path $out 'crt_compat.obj'
& $wrapper cl /nologo /c /MT /D_NO_CRT_STDIO_INLINE /D_CRT_SECURE_NO_WARNINGS "/Fo$test" (Join-Path $root 'test/lib-win32/crt-compat/crt_compat_test.c') *> (Join-Path $out 'compile-test.log')
if ($LASTEXITCODE) { throw 'Test compile failed.' }
& $wrapper cl /nologo /c /MT "/Fo$provider" (Join-Path $root 'src/lib-win32/crt-compat/crt_compat.c') *> (Join-Path $out 'compile-provider.log')
if ($LASTEXITCODE) { throw 'Provider compile failed.' }
$exe = Join-Path $out 'crt-test.exe'
& $wrapper link /nologo "/OUT:$exe" $test $provider *> (Join-Path $out 'link.log')
if ($LASTEXITCODE) { throw 'Valid provider link failed.' }
& $exe *> (Join-Path $out 'result.log')
if ($LASTEXITCODE) { throw 'Formatting behavior failed.' }
& $wrapper link /nologo "/OUT:$out/missing.exe" $test *> (Join-Path $out 'missing.log')
if (!$LASTEXITCODE -or (Get-Content (Join-Path $out 'missing.log') -Raw) -notmatch 'LNK2019') {
    throw 'Missing provider was not rejected.'
}
Copy-Item $provider (Join-Path $out 'duplicate.obj')
& $wrapper link /nologo "/OUT:$out/duplicate.exe" $test $provider (Join-Path $out 'duplicate.obj') *> (Join-Path $out 'duplicate.log')
if (!$LASTEXITCODE -or (Get-Content (Join-Path $out 'duplicate.log') -Raw) -notmatch 'LNK2005') {
    throw 'Duplicate provider was not rejected.'
}
Write-Output 'PASS CRT source ownership, formatting and missing/duplicate provider controls'
