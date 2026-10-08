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
foreach ($name in @('PIF.EXE','HASH.EXE','MOUSE.DRV')) {
    if (Test-Path -LiteralPath (Join-Path $ToolRoot $name)) {
        throw "Win1.01 tool directory must not duplicate released $name"
    }
}

$closePif=Join-Path $BuildRoot 'close-on-exit.pif'
& (Join-Path $release 'PIF.EXE') create $closePif --title 'close receipt' --program 'SETUP.EXE' --directory $BuildRoot --arguments '' --config 'CONFIG.NT' --autoexec 'AUTOEXEC.NT' --close-on-exit
if ($LASTEXITCODE -ne 0) { throw 'PIF.EXE could not create a CloseOnExit PIF' }
$closeView=& (Join-Path $release 'PIF.EXE') show $closePif
if ($LASTEXITCODE -ne 0 -or $closeView -notcontains 'STANDARD.MS_FLAGS=0x10' -or $closeView -notcontains 'STANDARD.CLOSE_ON_EXIT=YES') {
    throw 'PIF.EXE did not encode the explicit CloseOnExit PIF contract'
}

function New-Media([string]$Name) {
    $media = Join-Path $BuildRoot $Name
    New-Item -ItemType Directory -Force -Path $media | Out-Null
    foreach ($name in @('SETUP.EXE','SETUP.LBL','KERNEL.EXE','USER.EXE','GDI.EXE','DISK1','DISK2','DISK3','DISK4','DISK5','MOUSE.DRV')) {
        [IO.File]::WriteAllBytes((Join-Path $media $name), [Text.Encoding]::ASCII.GetBytes($name))
    }
    return $media
}

function Invoke-Apply([string]$Media, [int]$ExpectedExit = 0) {
    $input = Join-Path $BuildRoot ('apply-' + [IO.Path]::GetFileName($Media) + '.txt')
    [IO.File]::WriteAllText($input, "$Media`r`n`r`n", [Text.Encoding]::ASCII)
    $command = '(type "{0}" & echo.) | call "{1}"' -f $input,(Join-Path $ToolRoot 'APPLY.CMD')
    $stdout = Join-Path $BuildRoot ('apply-' + [IO.Path]::GetFileName($Media) + '.out')
    $stderr = Join-Path $BuildRoot ('apply-' + [IO.Path]::GetFileName($Media) + '.err')
    $process = Start-Process -FilePath cmd.exe -ArgumentList @('/d','/c',$command) -RedirectStandardOutput $stdout -RedirectStandardError $stderr -Wait -PassThru
    if ($process.ExitCode -ne $ExpectedExit) {
        throw "APPLY.CMD returned $($process.ExitCode), expected $ExpectedExit`n$((Get-Content -LiteralPath $stdout -Raw))$((Get-Content -LiteralPath $stderr -Raw))"
    }
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
        # SETUP.CMD uses internal batch subroutines.  A pipe runs CMD's left
        # and right sides in separate command contexts, which makes CALL :label
        # unreliable; standard-input redirection preserves the batch context.
        $command = 'call "{0}" < "{1}"' -f (Join-Path $Media 'PATCH\SETUP.CMD'),$input
        $process = Start-Process -FilePath cmd.exe -ArgumentList @('/d','/c',$command) -RedirectStandardOutput $stdout -RedirectStandardError $stderr -Wait -PassThru
        if ($process.ExitCode -ne $ExitCode) {
            throw "SETUP.CMD returned $($process.ExitCode), expected $ExitCode`n$((Get-Content -LiteralPath $stdout -Raw))$((Get-Content -LiteralPath $stderr -Raw))"
        }
    } finally {
        $env:PATH = $oldPath; $env:RUN16 = $oldRun16; $env:WIN101_TEST_INSTALL = $oldInstall; $env:WIN101_TEST_EXIT = $oldExit
    }
}

function Invoke-Unapply([string]$Media, [int]$ExpectedExit = 0) {
    $input = Join-Path $BuildRoot ('unapply-' + [IO.Path]::GetFileName($Media) + '.txt')
    [IO.File]::WriteAllText($input, "$Media`r`n`r`n", [Text.Encoding]::ASCII)
    $stdout = Join-Path $BuildRoot ('unapply-' + [IO.Path]::GetFileName($Media) + '.out')
    $stderr = Join-Path $BuildRoot ('unapply-' + [IO.Path]::GetFileName($Media) + '.err')
    $command = '(type "{0}" & echo.) | call "{1}"' -f $input,(Join-Path $ToolRoot 'UNAPPLY.CMD')
    $process = Start-Process -FilePath cmd.exe -ArgumentList @('/d','/c',$command) -RedirectStandardOutput $stdout -RedirectStandardError $stderr -Wait -PassThru
    if ($process.ExitCode -ne $ExpectedExit) {
        throw "UNAPPLY.CMD returned $($process.ExitCode), expected $ExpectedExit`n$((Get-Content -LiteralPath $stdout -Raw))$((Get-Content -LiteralPath $stderr -Raw))"
    }
}

$media = New-Media 'm'
$install = Join-Path $BuildRoot 'i'
$originalMouse = (Get-FileHash -LiteralPath (Join-Path $media 'MOUSE.DRV')).Hash
Invoke-Apply $media
$patch = Join-Path $media 'PATCH'
foreach ($name in @('SETUP.CMD','PIF.EXE','HASH.EXE','SETVER.EXE')) {
    if (!(Test-Path -LiteralPath (Join-Path $patch $name) -PathType Leaf)) { throw "PATCH missing $name" }
}
if ((Get-ChildItem -LiteralPath $patch -Force -File | Select-Object -ExpandProperty Name | Sort-Object) -join ',' -ne 'HASH.EXE,PIF.EXE,SETUP.CMD,SETVER.EXE') {
    throw 'media PATCH did not contain exactly the declared helpers'
}
if ((Get-FileHash -LiteralPath (Join-Path $media 'MOUSE.DRV')).Hash -ne (Get-FileHash -LiteralPath (Join-Path $release 'MOUSE101.DRV')).Hash) {
    throw 'media root did not receive the released replacement mouse driver'
}
if ((Get-FileHash -LiteralPath (Join-Path $media 'MOUSE.DRV.BAK')).Hash -ne $originalMouse) {
    throw 'media MOUSE.DRV.BAK did not preserve the original driver bytes'
}
Invoke-Apply $media
if ((Get-FileHash -LiteralPath (Join-Path $media 'MOUSE.DRV.BAK')).Hash -ne $originalMouse) {
    throw 'repeat apply changed the preserved original driver'
}
foreach ($name in @('configure-launch.ps1','run-setup.ps1','win.cmd.template','WIN31-TEMPLATE.PIF','addon-files.json','TEMP','MOUSE.DRV','README.TXT')) {
    if (Test-Path -LiteralPath (Join-Path $patch $name)) { throw "PATCH retained forbidden $name" }
}
Invoke-Setup $media $install 0
if (Test-Path -LiteralPath (Join-Path $patch 'TEMP')) { throw 'successful SETUP.CMD retained TEMP' }
if ((Get-ChildItem -LiteralPath $patch -Force -File | Select-Object -ExpandProperty Name | Sort-Object) -join ',' -ne 'HASH.EXE,PIF.EXE,SETUP.CMD,SETVER.EXE') {
    throw 'successful SETUP.CMD retained non-helper media PATCH state'
}
foreach ($name in @('WIN101.PIF','CONFIG.NT','AUTOEXEC.NT','WIN.CMD','SETVER.EXE')) {
    if (!(Test-Path -LiteralPath (Join-Path $install "PATCH\$name") -PathType Leaf)) { throw "installed PATCH missing $name" }
}
foreach ($name in @('WIN.PIF','CONFIG.NT','AUTOEXEC.NT')) {
    if (!(Test-Path -LiteralPath (Join-Path $install $name) -PathType Leaf)) { throw "installed root missing $name" }
}
$installedPif=& (Join-Path $release 'PIF.EXE') show (Join-Path $install 'PATCH\WIN101.PIF')
if ($LASTEXITCODE -ne 0 -or $installedPif -notcontains 'STANDARD.CLOSE_ON_EXIT=YES') {
    throw 'installed Windows launch PIF must be valid and CloseOnExit'
}
if ((Get-Content -LiteralPath (Join-Path $install 'PATCH\WIN.CMD') -Raw) -notmatch '(?im)^call run16 ') {
    throw 'installed WIN.CMD does not resolve run16 through PATH'
}
[IO.File]::WriteAllText((Join-Path $patch 'KEEP.TXT'), 'owner content', [Text.Encoding]::ASCII)
Invoke-Unapply $media
if ((Get-FileHash -LiteralPath (Join-Path $media 'MOUSE.DRV')).Hash -ne $originalMouse) {
    throw 'UNAPPLY.CMD did not restore the original root driver bytes'
}
if (Test-Path -LiteralPath (Join-Path $media 'MOUSE.DRV.BAK')) { throw 'UNAPPLY.CMD retained a consumed root backup' }
foreach ($name in @('SETUP.CMD','PIF.EXE','HASH.EXE','SETVER.EXE')) {
    if (Test-Path -LiteralPath (Join-Path $patch $name)) { throw "UNAPPLY.CMD retained helper $name" }
}
if (!(Test-Path -LiteralPath (Join-Path $patch 'KEEP.TXT') -PathType Leaf)) { throw 'UNAPPLY.CMD removed unknown PATCH content' }
Invoke-Unapply $media 2

$failedMedia = New-Media 'f'
Invoke-Apply $failedMedia
Invoke-Setup $failedMedia (Join-Path $BuildRoot 'n') 37
if (Test-Path -LiteralPath (Join-Path $failedMedia 'PATCH\TEMP')) { throw 'failed SETUP.CMD retained TEMP' }
if (Test-Path -LiteralPath (Join-Path $failedMedia 'PATCH\setup-result.txt')) { throw 'failed SETUP.CMD retained non-helper result state' }
$conflictingMedia = New-Media 'c'
[IO.File]::WriteAllBytes((Join-Path $conflictingMedia 'MOUSE.DRV.BAK'), [Text.Encoding]::ASCII.GetBytes('unrecognised backup'))
$beforeConflict = (Get-FileHash -LiteralPath (Join-Path $conflictingMedia 'MOUSE.DRV')).Hash
Invoke-Apply $conflictingMedia 5
if ((Get-FileHash -LiteralPath (Join-Path $conflictingMedia 'MOUSE.DRV')).Hash -ne $beforeConflict) {
    throw 'conflicting backup failure overwrote the original media driver'
}
'PASS Win1.01 APPLY/UNAPPLY/SETUP CMD orchestration, generated profile, recovery, and cleanup'
