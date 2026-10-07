param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$PifTemplate,
      [Parameter(Mandatory)][string]$SetverPath)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path $root)){throw 'Fresh build-owned fixture required'}
New-Item -ItemType Directory -Path $root|Out-Null
$source=Join-Path $repo 'tools/win101-setup'
foreach($name in @('configure-launch.ps1','run-setup.ps1','win.cmd.template')) {
    Copy-Item -LiteralPath "$source/$name" -Destination $root
}
Copy-Item -LiteralPath $PifTemplate -Destination "$root/WIN31-TEMPLATE.PIF"
Copy-Item -LiteralPath $SetverPath -Destination "$root/SETVER.EXE"
Copy-Item -LiteralPath (Join-Path $repo 'assets/release/MOUSE101.DRV') -Destination "$root/MOUSE.DRV"
# Deliberately inert existence fixtures, never executable guest replacements.
foreach($name in @('WIN.COM','WIN100.BIN','WIN100.OVL','SETUP.EXE')) {
    [IO.File]::WriteAllBytes("$root/$name",[byte[]]@(0))
}
& "$root/configure-launch.ps1" -Mode Installed -InstallRoot $root
$pif=[IO.File]::ReadAllBytes("$root/PATCH/WIN101.PIF")
function Field([int]$offset,[int]$length) {
    [Text.Encoding]::ASCII.GetString($pif,$offset,$length).Trim([char]0)
}
if((Field 0x24 63) -ne "$root\WIN.COM" -or (Field 0x65 64) -ne $root -or
   (Field 0xa5 64) -ne ''){throw 'PIF destination or argument mismatch'}
$position=0x171;$nt=$false
while($position -ne 0xffff) {
    if((Field $position 16) -eq 'WINDOWS NT  3.1') {
        $data=[BitConverter]::ToUInt16($pif,$position+18)
        if((Field ($data+12) 64) -ne "$root\PATCH\CONFIG.NT" -or
           (Field ($data+76) 64) -ne "$root\PATCH\AUTOEXEC.NT"){throw 'PIF profile mismatch'}
        $nt=$true
    }
    $position=[BitConverter]::ToUInt16($pif,$position+16)
}
if(!$nt){throw 'Missing NT profile'}
foreach($name in @('WIN.PIF','SETUP.PIF')) {
    $installed=[IO.File]::ReadAllBytes("$root/$name")
    $text=[Text.Encoding]::ASCII.GetString($installed)
    $program=if($name -eq 'WIN.PIF'){'WIN.COM'}else{'SETUP.EXE'}
    if(!$text.Contains("$root\$program") -or !$text.Contains("$root\PATCH\CONFIG.NT") -or
       $text.Contains('\WORK\')){throw 'Installed root PIF retains temporary dependency'}
}
$config=Get-Content "$root/PATCH/CONFIG.NT" -Raw
$auto=Get-Content "$root/PATCH/AUTOEXEC.NT" -Raw
if(!$config.Contains("device=$root\PATCH\SETVER.EXE") -or
   !$auto.Contains("SET TEMP=$root\PATCH\TEMP") -or
   !$auto.Contains("SET PATH=$root;")){throw 'Generated profile destination mismatch'}
$launcher=Get-Content "$root/PATCH/win.cmd" -Raw
if(Test-Path "$root/PATCH/start.cmd"){throw 'Redundant launcher generated'}
if(!$launcher.Contains('call run16 "%~dp0WIN101.PIF"')){throw 'Launcher must use PATH and its own directory'}
$before=(Get-FileHash "$root/PATCH/WIN101.PIF").Hash
foreach($invalid in @("$root\missing", "$root\bad path", ("$root\"+('x'*70)))) {
    $failed=$false
    try { & "$root/configure-launch.ps1" -Mode Installed -InstallRoot $invalid } catch {$failed=$true}
    if(!$failed){throw 'Invalid destination accepted'}
}
if((Get-FileHash "$root/PATCH/WIN101.PIF").Hash -ne $before){throw 'Refusal mutated valid PIF'}
# Exercise orchestration with a PATH launcher fixture, never original Setup.
[IO.File]::WriteAllText("$root/run16.cmd",@'
@echo off
if not "%WIN101_TEST_EXIT%"=="0" exit /b %WIN101_TEST_EXIT%
copy /y "%~dp0WORK\SETUP.PIF" "%~dp0SETUP.PIF" >nul
copy /y "%~dp0WORK\CONFIG.NT" "%~dp0CONFIG.NT" >nul
copy /y "%~dp0WORK\AUTOEXEC.NT" "%~dp0AUTOEXEC.NT" >nul
exit /b 0
'@)
$oldPath=$env:PATH;$oldExit=$env:WIN101_TEST_EXIT
try {
    $env:PATH="$root;$oldPath";$env:WIN101_TEST_EXIT='0'
    & powershell -NoProfile -ExecutionPolicy Bypass -File "$root/run-setup.ps1" -InstallRoot $root
    if($LASTEXITCODE -ne 0 -or (Test-Path "$root/WORK")){throw 'Success did not clean temporary WORK'}
    foreach($name in @('SETUP.PIF','WIN.PIF','CONFIG.NT','AUTOEXEC.NT')) {
        $text=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes("$root/$name"))
        if($text.Contains('\WORK\')){throw "Copied installer dependency remains in $name"}
    }
    $env:WIN101_TEST_EXIT='37'
    & powershell -NoProfile -ExecutionPolicy Bypass -File "$root/run-setup.ps1" -InstallRoot $root
    if($LASTEXITCODE -ne 37 -or (Test-Path "$root/WORK")){throw 'Failure result or WORK cleanup incorrect'}
    if((Get-Content "$root/setup-result.txt" -Raw).Trim() -ne 'run16_exit=37'){throw 'Actual failure not recorded'}
    if((Get-FileHash "$root/PATCH/WIN101.PIF").Hash -ne $before){throw 'Failure changed installed profile'}
    & powershell -NoProfile -ExecutionPolicy Bypass -File "$root/run-setup.ps1" -InstallRoot $root -ConfigureAfterFailure
    if($LASTEXITCODE -ne 37 -or (Test-Path "$root/WORK")){throw 'Confirmed recovery lost actual failure or cleanup'}
} finally {$env:PATH=$oldPath;$env:WIN101_TEST_EXIT=$oldExit}
$forbidden=@(Get-ChildItem $source -File|Select-String -Pattern '(?i)\b[A-Z]:[\\/]|\bsubst\b')
if($forbidden.Count){throw 'Fixed drive path or mapping command remains in component'}
'PASS destination PIF/profile, PATH launcher, three refusals, success/failure WORK cleanup and path audit; no Setup or guest execution'
