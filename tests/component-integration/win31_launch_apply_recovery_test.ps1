param(
    [Parameter(Mandatory)][string]$ReferenceRoot,
    [Parameter(Mandatory)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$tool = Join-Path $repo 'tools/win31-launch'
$release = Join-Path $repo 'assets/release'
$reference = (Resolve-Path -LiteralPath $ReferenceRoot).Path.TrimEnd('\')
New-Item -ItemType Directory -Force -Path $BuildRoot | Out-Null
$build = (Resolve-Path -LiteralPath $BuildRoot).Path.TrimEnd('\')
if ((Join-Path $build 'fresh\PATCH\CONFIG.NT').Length -gt 63) {
    throw 'BuildRoot is too long for the historical 63-character PIF CONFIG field; use a short physical path.'
}

foreach ($name in @('WIN.COM','SYSTEM.INI','SYSTEM\KRNL386.EXE','SYSTEM\WIN386.EXE','SYSTEM\MOUSE.DRV')) {
    if (!(Test-Path -LiteralPath (Join-Path $reference $name) -PathType Leaf)) {
        throw "Missing reference fixture input: $name"
    }
}
if ((Get-FileHash -LiteralPath (Join-Path $reference 'SYSTEM\KRNL386.EXE')).Hash -ne 'FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980') {
    throw 'Reference KRNL386 must be the approved retail identity'
}
if ((Get-FileHash -LiteralPath (Join-Path $reference 'SYSTEM\WIN386.EXE')).Hash -ne '6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5') {
    throw 'Reference WIN386 must be the approved retail identity'
}

# Malformed INI must fail without changing bytes; unrelated sections and
# comment lines must survive a successful transformation.
$configTool = Join-Path $tool 'PATCH386.EXE'
$badIni = Join-Path $build 'duplicate-system.ini'
[IO.File]::WriteAllText($badIni,"[386Enh]`r`nPaging=on`r`n[386Enh]`r`nPaging=on`r`n",[Text.Encoding]::ASCII)
$badHash = (Get-FileHash -LiteralPath $badIni).Hash
$badProcess = Start-Process -FilePath $configTool -ArgumentList @('--config', $badIni) -WindowStyle Hidden -Wait -PassThru `
    -RedirectStandardOutput (Join-Path $build 'duplicate-config.out') -RedirectStandardError (Join-Path $build 'duplicate-config.err')
if ($badProcess.ExitCode -eq 0 -or (Get-FileHash -LiteralPath $badIni).Hash -ne $badHash) { throw 'Duplicate 386Enh was not a no-write failure' }
$mixedIni = Join-Path $build 'mixed-system.ini'
[IO.File]::WriteAllText($mixedIni,"[386Enh]`r`n; preserve`r`nPaging=on`r`n  [other]`r`nPaging=keep`r`n",[Text.Encoding]::ASCII)
& $configTool --config $mixedIni
if ($LASTEXITCODE) { throw 'Valid mixed SYSTEM.INI was rejected' }
$mixed = Get-Content -LiteralPath $mixedIni -Raw
if (!$mixed.Contains("; preserve`r`n") -or !$mixed.Contains("  [other]`r`nPaging=keep`r`n")) { throw 'Unrelated SYSTEM.INI content changed' }
$mixedHash = (Get-FileHash -LiteralPath $mixedIni).Hash
& $configTool --config $mixedIni
if ($LASTEXITCODE -or (Get-FileHash -LiteralPath $mixedIni).Hash -ne $mixedHash) { throw 'Configuration transform is not idempotent' }

function New-Fixture([string]$Name) {
    $root = Join-Path $build $Name
    New-Item -ItemType Directory -Force -Path (Join-Path $root 'SYSTEM') | Out-Null
    Copy-Item -LiteralPath (Join-Path $reference 'WIN.COM'),(Join-Path $reference 'SYSTEM.INI') -Destination $root
    Copy-Item -LiteralPath (Join-Path $reference 'SYSTEM\KRNL386.EXE'),(Join-Path $reference 'SYSTEM\WIN386.EXE'),(Join-Path $reference 'SYSTEM\MOUSE.DRV') -Destination (Join-Path $root 'SYSTEM')
    return $root
}

function Invoke-Tool([string]$Script, [string]$Root, [int]$ExpectedExit = 0) {
    $input = Join-Path $build (([IO.Path]::GetFileNameWithoutExtension($Script)) + '-' + ([IO.Path]::GetFileName($Root)) + '.txt')
    [IO.File]::WriteAllText($input, "$Root`r`n`r`n", [Text.Encoding]::ASCII)
    $stdout = "$input.out"; $stderr = "$input.err"
    $command = 'call "{0}" < "{1}"' -f (Join-Path $tool $Script),$input
    $process = Start-Process -FilePath cmd.exe -ArgumentList @('/d','/c',$command) -WindowStyle Hidden -RedirectStandardOutput $stdout -RedirectStandardError $stderr -Wait -PassThru
    if ($process.ExitCode -ne $ExpectedExit) {
        throw "$Script returned $($process.ExitCode), expected $ExpectedExit`n$((Get-Content -LiteralPath $stdout -Raw))$((Get-Content -LiteralPath $stderr -Raw))"
    }
    return (Get-Content -LiteralPath $stdout -Raw)
}

$krnlRetail = (Get-FileHash -LiteralPath (Join-Path $reference 'SYSTEM\KRNL386.EXE')).Hash
$winRetail = (Get-FileHash -LiteralPath (Join-Path $reference 'SYSTEM\WIN386.EXE')).Hash
$mouseRetail = (Get-FileHash -LiteralPath (Join-Path $reference 'SYSTEM\MOUSE.DRV')).Hash
$mouseRelease = (Get-FileHash -LiteralPath (Join-Path $release 'MOUSE31.DRV')).Hash
$systemOriginal = (Get-FileHash -LiteralPath (Join-Path $reference 'SYSTEM.INI')).Hash

function Assert-Applied([string]$Root) {
    $system = Join-Path $Root 'SYSTEM'
    if ((Get-FileHash -LiteralPath (Join-Path $system 'KRNL386.EXE')).Hash -ne '75E931AE000FB7C70CF686DA6F7D3FCB032FDC0E0A4E3187ED68179B85E2CD4A') { throw 'KRNL386 candidate missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'WIN386.EXE')).Hash -ne 'C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8') { throw 'WIN386 candidate missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'MOUSE.DRV')).Hash -ne $mouseRelease) { throw 'released mouse driver missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'KRNL386.EXE.BAK')).Hash -ne $krnlRetail) { throw 'KRNL386 adjacent backup missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'WIN386.EXE.BAK')).Hash -ne $winRetail) { throw 'WIN386 adjacent backup missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'MOUSE.DRV.BAK')).Hash -ne $mouseRetail) { throw 'mouse adjacent backup missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $Root 'PATCH\SYSTEM.INI.BAK')).Hash -ne $systemOriginal) { throw 'SYSTEM.INI recovery copy missing' }
    $ini = Get-Content -LiteralPath (Join-Path $Root 'SYSTEM.INI') -Raw
    foreach ($setting in @('Paging=off','32BitDiskAccess=off','COM1Irq=-1','COM2Irq=-1','COM3Irq=-1','COM4Irq=-1')) {
        if ($ini -notmatch ('(?im)^' + [regex]::Escape($setting) + '\s*$')) { throw "Missing SYSTEM.INI setting: $setting" }
    }
    if ($ini -match '(?im)^\s*PermSwap(DOSDrive|SizeK)\s*=') { throw 'Permanent swap still configured' }
    $recorded=(Get-Content -LiteralPath (Join-Path $Root 'PATCH\SYSTEM.INI.SHA256') -Raw).Trim()
    if ($recorded -ne (Get-FileHash -LiteralPath (Join-Path $Root 'SYSTEM.INI')).Hash) { throw 'SYSTEM.INI recovery hash mismatch' }
    foreach ($legacy in @('KRNL386.ORIG','WIN386.ORIG','MOUSE.DRV.ORIG')) {
        if (Test-Path -LiteralPath (Join-Path $Root "PATCH\$legacy")) { throw "legacy PATCH recovery retained: $legacy" }
    }
}

$fresh = New-Fixture 'fresh'
$applyOutput = Invoke-Tool 'APPLY.CMD' $fresh
foreach ($expected in @(
    "Replace: KRNL386.EXE => $fresh\SYSTEM",
    "Replace: WIN386.EXE => $fresh\SYSTEM",
    "Replace: MOUSE.DRV => $fresh\SYSTEM",
    "New: CONFIG.NT => $fresh\PATCH",
    "New: WINSTD.PIF => $fresh\PATCH",
    "New: WIN386.CMD => $fresh\PATCH")) {
    if (!$applyOutput.Contains($expected)) { throw "APPLY action report missing: $expected" }
}
Assert-Applied $fresh
$configuredSystem = [IO.File]::ReadAllBytes((Join-Path $fresh 'SYSTEM.INI'))
[IO.File]::AppendAllText((Join-Path $fresh 'SYSTEM.INI'), "`r`n; owner edit`r`n")
Invoke-Tool 'UNAPPLY.CMD' $fresh 5
if (!(Test-Path -LiteralPath (Join-Path $fresh 'PATCH\SYSTEM.INI.BAK'))) { throw 'Refused recovery removed configuration backup' }
[IO.File]::WriteAllBytes((Join-Path $fresh 'SYSTEM.INI'),$configuredSystem)
& (Join-Path $repo 'tests/component-integration/win31_launch_profile_test.ps1') -InstallRoot $fresh -ProfileDirectory (Join-Path $fresh 'PATCH') -ExpectedKernelHash '75E931AE000FB7C70CF686DA6F7D3FCB032FDC0E0A4E3187ED68179B85E2CD4A' -ExpectDosOnly $false
Invoke-Tool 'APPLY.CMD' $fresh
Assert-Applied $fresh
# A repeat apply cannot overwrite a driver whose identity is no longer the
# released add-on.  The original adjacent backup remains recoverable.
[IO.File]::WriteAllBytes((Join-Path $fresh 'SYSTEM\MOUSE.DRV'), [byte[]](1,2,3))
$unknownMouse = (Get-FileHash -LiteralPath (Join-Path $fresh 'SYSTEM\MOUSE.DRV')).Hash
Invoke-Tool 'APPLY.CMD' $fresh 5
if ((Get-FileHash -LiteralPath (Join-Path $fresh 'SYSTEM\MOUSE.DRV')).Hash -ne $unknownMouse) { throw 'unrecognised mouse driver was overwritten' }
[IO.File]::Copy((Join-Path $release 'MOUSE31.DRV'), (Join-Path $fresh 'SYSTEM\MOUSE.DRV'), $true)
[IO.File]::WriteAllBytes((Join-Path $fresh 'SYSTEM\MOUSE.DRV'), [byte[]](4,5,6))
[IO.File]::WriteAllText((Join-Path $fresh 'PATCH\KEEP.TXT'), 'owner content', [Text.Encoding]::ASCII)
$unapplyOutput = Invoke-Tool 'UNAPPLY.CMD' $fresh
foreach ($expected in @(
    "Restore: KRNL386.EXE => $fresh\SYSTEM",
    "Restore: WIN386.EXE => $fresh\SYSTEM",
    "Restore: MOUSE.DRV => $fresh\SYSTEM",
    "Remove: CONFIG.NT => $fresh\PATCH",
    "Remove: WINSTD.PIF => $fresh\PATCH")) {
    if (!$unapplyOutput.Contains($expected)) { throw "UNAPPLY action report missing: $expected" }
}
$system = Join-Path $fresh 'SYSTEM'
foreach ($pair in @(@('KRNL386.EXE',$krnlRetail),@('WIN386.EXE',$winRetail),@('MOUSE.DRV',$mouseRetail))) {
    if ((Get-FileHash -LiteralPath (Join-Path $system $pair[0])).Hash -ne $pair[1]) { throw "UNAPPLY did not restore $($pair[0])" }
    if (Test-Path -LiteralPath (Join-Path $system ($pair[0] + '.BAK'))) { throw "UNAPPLY retained $($pair[0]).BAK" }
}
if (!(Test-Path -LiteralPath (Join-Path $fresh 'PATCH\KEEP.TXT') -PathType Leaf)) { throw 'UNAPPLY removed unknown PATCH content' }
if ((Get-FileHash -LiteralPath (Join-Path $fresh 'SYSTEM.INI')).Hash -ne $systemOriginal) { throw 'UNAPPLY did not restore original SYSTEM.INI' }
if (Test-Path -LiteralPath (Join-Path $fresh 'PATCH\SYSTEM.INI.BAK')) { throw 'UNAPPLY retained SYSTEM.INI recovery copy' }

$unknown=New-Fixture 'unknown'
$unknownKernel=Join-Path $unknown 'SYSTEM\KRNL386.EXE'
$bytes=[IO.File]::ReadAllBytes($unknownKernel);$bytes[0x3d]=0;[IO.File]::WriteAllBytes($unknownKernel,$bytes)
$unknownHash=(Get-FileHash $unknownKernel).Hash
Invoke-Tool 'APPLY.CMD' $unknown 5
if((Get-FileHash $unknownKernel).Hash -ne $unknownHash){throw 'Unknown kernel was modified'}

$badInstall=New-Fixture 'bad-config'
Copy-Item -LiteralPath $badIni -Destination (Join-Path $badInstall 'SYSTEM.INI') -Force
$badInstallHash=(Get-FileHash (Join-Path $badInstall 'SYSTEM.INI')).Hash
Invoke-Tool 'APPLY.CMD' $badInstall 5
if((Get-FileHash (Join-Path $badInstall 'SYSTEM.INI')).Hash -ne $badInstallHash -or
   (Test-Path (Join-Path $badInstall 'PATCH\SYSTEM.INI.BAK')) -or
   (Get-FileHash (Join-Path $badInstall 'SYSTEM\KRNL386.EXE')).Hash -ne $krnlRetail) {
    throw 'Invalid SYSTEM.INI did not leave original installation intact'
}

'PASS win31-launch fresh, repeat, unknown-image refusal, and adjacent backup preservation'
