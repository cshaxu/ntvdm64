param(
    [Parameter(Mandatory)][string]$ToolRoot,
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$ToolRoot = (Resolve-Path -LiteralPath $ToolRoot).Path
New-Item -ItemType Directory -Force -Path $BuildRoot | Out-Null
$BuildRoot = (Resolve-Path -LiteralPath $BuildRoot).Path
if ((Join-Path $BuildRoot 'm\PATCH\TEMP\CONFIG.NT').Length -gt 63) {
    throw 'BuildRoot is too long for the historical 63-character PIF CONFIG field; use a short physical path.'
}
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$release = Join-Path $repo 'assets/release'
foreach ($name in @('PIF.EXE','HASH.EXE','MOUSE101.DRV')) {
    $packageName = if ($name -eq 'MOUSE101.DRV') { 'MOUSE.DRV' } else { $name }
    if ((Get-FileHash -LiteralPath (Join-Path $ToolRoot $packageName)).Hash -ne
        (Get-FileHash -LiteralPath (Join-Path $release $name)).Hash) {
        throw "Win1.01 tool payload is not the released $name"
    }
}

function New-Media([string]$Name) {
    $media = Join-Path $BuildRoot $Name
    New-Item -ItemType Directory -Force -Path $media | Out-Null
    foreach ($name in @('SETUP.EXE','SETUP.LBL','KERNEL.EXE','USER.EXE','GDI.EXE','DISK1','DISK2','DISK3','DISK4','DISK5')) {
        [IO.File]::WriteAllBytes((Join-Path $media $name), [Text.Encoding]::ASCII.GetBytes($name))
    }
    return $media
}

function Invoke-Apply([string]$Media) {
    $input = Join-Path $BuildRoot ('apply-' + [IO.Path]::GetFileName($Media) + '.txt')
    [IO.File]::WriteAllText($input, "$Media`r`n`r`n", [Text.Encoding]::ASCII)
    $command = '(type "{0}" & echo.) | call "{1}"' -f $input,(Join-Path $ToolRoot 'APPLY.CMD')
    $process = Start-Process -FilePath cmd.exe -ArgumentList @('/d','/c',$command) -Wait -PassThru
    if ($process.ExitCode -ne 0) { throw "APPLY.CMD failed: $($process.ExitCode)" }
}

function Invoke-Setup([string]$Media, [string]$Install, [int]$ExitCode) {
    $bin = Join-Path $BuildRoot ('bin-' + [IO.Path]::GetFileName($Media))
    New-Item -ItemType Directory -Force -Path $bin | Out-Null
    $fake = Join-Path $bin 'run16.cmd'
    $body = @"
@echo off
if not exist "%~1" exit /b 91
if not "%WIN101_TEST_EXIT%"=="0" exit /b %WIN101_TEST_EXIT%
mkdir "%WIN101_TEST_INSTALL%" >nul 2>nul
for %%F in (WIN.COM WIN100.BIN WIN100.OVL SETUP.EXE) do type nul > "%WIN101_TEST_INSTALL%\%%F"
exit /b 0
"@
    [IO.File]::WriteAllText($fake, $body, [Text.Encoding]::ASCII)
    $input = Join-Path $BuildRoot ('setup-' + [IO.Path]::GetFileName($Media) + '.txt')
    $answer = if ($ExitCode -eq 0) { $Install } else { '' }
    [IO.File]::WriteAllText($input, "$answer`r`n`r`n", [Text.Encoding]::ASCII)
    $oldPath = $env:PATH; $oldRun16 = $env:RUN16; $oldInstall = $env:WIN101_TEST_INSTALL; $oldExit = $env:WIN101_TEST_EXIT
    try {
        $env:PATH = "$bin;$oldPath"; $env:RUN16 = $fake; $env:WIN101_TEST_INSTALL = $Install; $env:WIN101_TEST_EXIT = "$ExitCode"
        $stdout = Join-Path $BuildRoot ('setup-' + [IO.Path]::GetFileName($Media) + '.out')
        $stderr = Join-Path $BuildRoot ('setup-' + [IO.Path]::GetFileName($Media) + '.err')
        $command = '(type "{0}" & echo.) | call "{1}"' -f $input,(Join-Path $Media 'PATCH\SETUP.CMD')
        $process = Start-Process -FilePath cmd.exe -ArgumentList @('/d','/c',$command) -RedirectStandardOutput $stdout -RedirectStandardError $stderr -Wait -PassThru
        if ($process.ExitCode -ne $ExitCode) {
            throw "SETUP.CMD returned $($process.ExitCode), expected $ExitCode`n$((Get-Content -LiteralPath $stdout -Raw))$((Get-Content -LiteralPath $stderr -Raw))"
        }
    } finally {
        $env:PATH = $oldPath; $env:RUN16 = $oldRun16; $env:WIN101_TEST_INSTALL = $oldInstall; $env:WIN101_TEST_EXIT = $oldExit
    }
}

$media = New-Media 'm'
$install = Join-Path $BuildRoot 'i'
Invoke-Apply $media
$patch = Join-Path $media 'PATCH'
foreach ($name in @('SETUP.CMD','README.TXT','MOUSE.DRV','SETVER.EXE','PIF.EXE','HASH.EXE')) {
    if (!(Test-Path -LiteralPath (Join-Path $patch $name) -PathType Leaf)) { throw "PATCH missing $name" }
}
foreach ($name in @('configure-launch.ps1','run-setup.ps1','win.cmd.template','WIN31-TEMPLATE.PIF','addon-files.json','TEMP')) {
    if (Test-Path -LiteralPath (Join-Path $patch $name)) { throw "PATCH retained forbidden $name" }
}
Invoke-Setup $media $install 0
if (Test-Path -LiteralPath (Join-Path $patch 'TEMP')) { throw 'successful SETUP.CMD retained TEMP' }
foreach ($name in @('WIN101.PIF','CONFIG.NT','AUTOEXEC.NT','WIN.CMD','MOUSE.DRV','SETVER.EXE')) {
    if (!(Test-Path -LiteralPath (Join-Path $install "PATCH\$name") -PathType Leaf)) { throw "installed PATCH missing $name" }
}
foreach ($name in @('WIN.PIF','CONFIG.NT','AUTOEXEC.NT')) {
    if (!(Test-Path -LiteralPath (Join-Path $install $name) -PathType Leaf)) { throw "installed root missing $name" }
}
& (Join-Path $ToolRoot 'PIF.EXE') show (Join-Path $install 'PATCH\WIN101.PIF') | Out-Null
if ($LASTEXITCODE -ne 0) { throw 'installed PIF is invalid' }
if ((Get-Content -LiteralPath (Join-Path $install 'PATCH\WIN.CMD') -Raw) -notmatch '(?im)^call run16 ') {
    throw 'installed WIN.CMD does not resolve run16 through PATH'
}

$failedMedia = New-Media 'f'
Invoke-Apply $failedMedia
Invoke-Setup $failedMedia (Join-Path $BuildRoot 'n') 37
if (Test-Path -LiteralPath (Join-Path $failedMedia 'PATCH\TEMP')) { throw 'failed SETUP.CMD retained TEMP' }
if ((Get-Content -LiteralPath (Join-Path $failedMedia 'PATCH\setup-result.txt') -Raw).Trim() -ne 'run16_exit=37') { throw 'failed SETUP.CMD did not retain true result' }
'PASS Win1.01 APPLY/SETUP CMD orchestration, generated profile, and cleanup'
