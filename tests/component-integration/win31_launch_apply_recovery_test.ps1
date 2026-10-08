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
    $process = Start-Process -FilePath cmd.exe -ArgumentList @('/d','/c',$command) -RedirectStandardOutput $stdout -RedirectStandardError $stderr -Wait -PassThru
    if ($process.ExitCode -ne $ExpectedExit) {
        throw "$Script returned $($process.ExitCode), expected $ExpectedExit`n$((Get-Content -LiteralPath $stdout -Raw))$((Get-Content -LiteralPath $stderr -Raw))"
    }
}

$krnlRetail = (Get-FileHash -LiteralPath (Join-Path $reference 'SYSTEM\KRNL386.EXE')).Hash
$winRetail = (Get-FileHash -LiteralPath (Join-Path $reference 'SYSTEM\WIN386.EXE')).Hash
$mouseRetail = (Get-FileHash -LiteralPath (Join-Path $reference 'SYSTEM\MOUSE.DRV')).Hash
$mouseRelease = (Get-FileHash -LiteralPath (Join-Path $release 'MOUSE31.DRV')).Hash

function Assert-Applied([string]$Root) {
    $system = Join-Path $Root 'SYSTEM'
    if ((Get-FileHash -LiteralPath (Join-Path $system 'KRNL386.EXE')).Hash -ne '88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181') { throw 'KRNL386 candidate missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'WIN386.EXE')).Hash -ne 'C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8') { throw 'WIN386 candidate missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'MOUSE.DRV')).Hash -ne $mouseRelease) { throw 'released mouse driver missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'KRNL386.EXE.BAK')).Hash -ne $krnlRetail) { throw 'KRNL386 adjacent backup missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'WIN386.EXE.BAK')).Hash -ne $winRetail) { throw 'WIN386 adjacent backup missing' }
    if ((Get-FileHash -LiteralPath (Join-Path $system 'MOUSE.DRV.BAK')).Hash -ne $mouseRetail) { throw 'mouse adjacent backup missing' }
    foreach ($legacy in @('KRNL386.ORIG','WIN386.ORIG','MOUSE.DRV.ORIG')) {
        if (Test-Path -LiteralPath (Join-Path $Root "PATCH\$legacy")) { throw "legacy PATCH recovery retained: $legacy" }
    }
}

$fresh = New-Fixture 'fresh'
Invoke-Tool 'APPLY.CMD' $fresh
Assert-Applied $fresh
& (Join-Path $repo 'tests/component-integration/win31_launch_profile_test.ps1') -InstallRoot $fresh -ProfileDirectory (Join-Path $fresh 'PATCH')
Invoke-Tool 'APPLY.CMD' $fresh
Assert-Applied $fresh
[IO.File]::WriteAllText((Join-Path $fresh 'PATCH\KEEP.TXT'), 'owner content', [Text.Encoding]::ASCII)
Invoke-Tool 'UNAPPLY.CMD' $fresh
$system = Join-Path $fresh 'SYSTEM'
foreach ($pair in @(@('KRNL386.EXE',$krnlRetail),@('WIN386.EXE',$winRetail),@('MOUSE.DRV',$mouseRetail))) {
    if ((Get-FileHash -LiteralPath (Join-Path $system $pair[0])).Hash -ne $pair[1]) { throw "UNAPPLY did not restore $($pair[0])" }
    if (Test-Path -LiteralPath (Join-Path $system ($pair[0] + '.BAK'))) { throw "UNAPPLY retained $($pair[0]).BAK" }
}
if (!(Test-Path -LiteralPath (Join-Path $fresh 'PATCH\KEEP.TXT') -PathType Leaf)) { throw 'UNAPPLY removed unknown PATCH content' }

$legacy = New-Fixture 'legacy'
Invoke-Tool 'APPLY.CMD' $legacy
New-Item -ItemType Directory -Force -Path (Join-Path $legacy 'PATCH') | Out-Null
Move-Item -LiteralPath (Join-Path $legacy 'SYSTEM\KRNL386.EXE.BAK') -Destination (Join-Path $legacy 'PATCH\KRNL386.ORIG')
Move-Item -LiteralPath (Join-Path $legacy 'SYSTEM\WIN386.EXE.BAK') -Destination (Join-Path $legacy 'PATCH\WIN386.ORIG')
Move-Item -LiteralPath (Join-Path $legacy 'SYSTEM\MOUSE.DRV.BAK') -Destination (Join-Path $legacy 'PATCH\MOUSE.DRV.ORIG')
Invoke-Tool 'APPLY.CMD' $legacy
Assert-Applied $legacy

'PASS win31-launch adjacent backup, repeat apply, legacy migration, and authenticated unapply'
