param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$build = [IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$build.StartsWith((Join-Path $repo 'build') + '\', [StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path -LiteralPath $build)) { throw 'Use a fresh build-owned fixture' }
$media = Join-Path $build 'media'
$patch = Join-Path $media 'PATCH'
$bin = Join-Path $build 'bin'
$incomplete = Join-Path $build 'incomplete'
New-Item -ItemType Directory -Path $patch,$bin,$incomplete | Out-Null
Copy-Item (Join-Path $repo 'tools/win31-setup/SETUP.CMD') (Join-Path $patch 'SETUP.CMD')
@'
@echo off
if not "%TEMP%"=="%TMP%" exit /b 81
if not exist "%TEMP%\" exit /b 82
>"%RUN16_TEST_TEMP%" echo %TEMP%
exit /b 0
'@ | Set-Content -LiteralPath (Join-Path $bin 'run16.cmd') -Encoding ASCII
$oldPath=$env:PATH; $oldTemp=$env:TEMP; $oldTmp=$env:TMP
$oldNoPause=$env:NTVDM_SETUP_TEST_NO_PAUSE; $oldProbe=$env:RUN16_TEST_TEMP
try {
    $env:PATH="$bin;$oldPath"
    $env:TEMP=$build; $env:TMP=$build
    $env:NTVDM_SETUP_TEST_NO_PAUSE='1'
    $env:RUN16_TEST_TEMP=Join-Path $build 'temp.txt'
    $output = ($incomplete | & cmd.exe /d /c (Join-Path $patch 'SETUP.CMD') 2>&1 | Out-String)
    if($LASTEXITCODE -ne 5 -or $output -notmatch 'Setup may have been cancelled or is incomplete') {
        throw "Incomplete installation was not rejected: $LASTEXITCODE $output"
    }
    if((Get-Content -LiteralPath $env:RUN16_TEST_TEMP -Raw).TrimEnd() -ne (Join-Path $patch 'TEMP')) {
        throw 'Setup did not receive its package-local temporary directory'
    }
    if(Test-Path -LiteralPath (Join-Path $patch 'TEMP')) {throw 'Empty Setup temporary directory remains'}
    if(Test-Path -LiteralPath (Join-Path $incomplete 'PATCH')) {throw 'Incomplete installation was modified'}
    'PASS Setup runner uses package-local TEMP and rejects an incomplete installation'
} finally {
    $env:PATH=$oldPath; $env:TEMP=$oldTemp; $env:TMP=$oldTmp
    $env:NTVDM_SETUP_TEST_NO_PAUSE=$oldNoPause; $env:RUN16_TEST_TEMP=$oldProbe
}
