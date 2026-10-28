[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$WindowsRoot)
$ErrorActionPreference='Stop'
function Fail([string]$m){throw $m}
function Backup([string]$p){$b="$p.W95LAUNCH.BAK";if(Test-Path $b){Fail "Recovery copy already exists: $b"};Copy-Item -LiteralPath $p -Destination $b;return $b}
function Record([string]$p,[string]$kind){@{path=$p;kind=$kind;installedSha256=(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash}|ConvertTo-Json -Compress|Add-Content -LiteralPath $script:journal}
function ReplaceText([string]$p,[string]$text){$b=Backup $p;try{[IO.File]::WriteAllText($p,$text,[Text.Encoding]::Default);Record $p 'replace'}catch{Copy-Item -LiteralPath $b -Destination $p -Force;throw}}
function ReplaceCandidate([string]$p,[string]$candidate){$b=Backup $p;try{Move-Item -LiteralPath $candidate -Destination $p -Force;Record $p 'replace'}catch{if(Test-Path -LiteralPath $b){Copy-Item -LiteralPath $b -Destination $p -Force};throw}}
function RecordCreated([string]$p){try{Record $p 'created'}catch{Remove-Item -LiteralPath $p -Force -ErrorAction SilentlyContinue;throw}}
$WindowsRoot=[IO.Path]::GetFullPath($WindowsRoot).TrimEnd('\');$system=Join-Path $WindowsRoot 'SYSTEM';$patch=Join-Path $WindowsRoot 'PATCH'
foreach($p in @("$WindowsRoot\WIN.COM","$system\VMM32.VXD","$WindowsRoot\SYSTEM.DAT","$WindowsRoot\SYSTEM.INI")){if(-not(Test-Path -LiteralPath $p)){Fail "Missing Win95 file: $p"}}
if(((Get-Item -LiteralPath $WindowsRoot -Force).Attributes -band [IO.FileAttributes]::ReparsePoint)){Fail "Refusing Windows root reparse point: $WindowsRoot"}
if(@(Get-ChildItem -LiteralPath $WindowsRoot -Recurse -Force | Where-Object { $_.Attributes -band [IO.FileAttributes]::ReparsePoint }).Count){Fail "Refusing tree containing reparse point: $WindowsRoot"}
foreach($name in @('W95LAUNCH.MANIFEST','CONFIG.NT','AUTOEXEC.NT','WIN.PIF','WIN.CMD','compat95.sys','MOUSE.DRV','SETVER.EXE')){
  if(Test-Path -LiteralPath (Join-Path $patch $name)){Fail "Refusing to overwrite pre-existing PATCH content: $name"}
}
$release=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\assets\release'));$patcher=Join-Path $release 'patch95.exe';$driver=Join-Path $release 'compat95.sys';$setver=Join-Path $env:SystemRoot 'System32\SETVER.EXE'
foreach($p in @($patcher,$driver,$setver,"$release\PIF.EXE","$release\MOUSE31.DRV")){if(-not(Test-Path -LiteralPath $p)){Fail "Missing required component: $p"}}
New-Item -ItemType Directory -Force -Path $patch,"$patch\TEMP" | Out-Null;$script:journal=Join-Path $patch 'W95LAUNCH.MANIFEST';if(Test-Path $script:journal){Fail "Existing launch journal: $script:journal (run UNAPPLY.CMD first)"};New-Item -ItemType File -Path $script:journal|Out-Null
try {
  $pathTool=Join-Path $PSScriptRoot '..\win95-path\Apply-Win95Path.ps1';if(Test-Path $pathTool){& $pathTool -WindowsRoot $WindowsRoot;if($LASTEXITCODE -ne 0){Fail 'Path migration failed'}}
  $vmm="$system\VMM32.VXD";$candidate="$vmm.W95NEW";& $patcher prepare $vmm $candidate;if($LASTEXITCODE -ne 0 -or -not(Test-Path $candidate)){Fail 'VMM32 identity is unsupported or patch creation failed'};ReplaceCandidate $vmm $candidate
  Copy-Item $driver "$patch\compat95.sys";RecordCreated "$patch\compat95.sys";Copy-Item "$release\MOUSE31.DRV" "$patch\MOUSE.DRV";RecordCreated "$patch\MOUSE.DRV";Copy-Item $setver "$patch\SETVER.EXE";RecordCreated "$patch\SETVER.EXE"
  $ini=[IO.File]::ReadAllText("$WindowsRoot\SYSTEM.INI",[Text.Encoding]::Default);if($ini -notmatch '(?im)^\s*mouse\.drv='){$ini+="`r`n[boot]`r`nmouse.drv=$patch\MOUSE.DRV`r`n"}else{$ini=[regex]::Replace($ini,'(?im)^\s*mouse\.drv=.*$',"mouse.drv=$patch\MOUSE.DRV")};if($ini -match '(?im)^\s*PagingOn='){$ini=[regex]::Replace($ini,'(?im)^\s*PagingOn=.*$','PagingOn=1')}else{$ini+="`r`n[386Enh]`r`nPagingOn=1`r`n"};ReplaceText "$WindowsRoot\SYSTEM.INI" $ini
  $config="dos=high, umb`r`ndevice=%SystemRoot%\system32\himem.sys`r`ndevice=$patch\SETVER.EXE`r`nfiles=128`r`ndosonly`r`ndevice=$WindowsRoot\IFSHLP.SYS`r`ndevice=$patch\compat95.sys /ROOT=$WindowsRoot`r`n";$auto="@echo off`r`nSET windir=$WindowsRoot`r`nSET winbootdir=$WindowsRoot`r`nSET TEMP=$patch\TEMP`r`nSET TMP=$patch\TEMP`r`nSET PATH=$WindowsRoot;$WindowsRoot\SYSTEM;%SystemRoot%\system32`r`nSETVER WIN.COM 7.10 >nul`r`nSETVER VMM32.VXD 7.10 >nul`r`n"
  [IO.File]::WriteAllText("$patch\CONFIG.NT",$config,[Text.Encoding]::ASCII);RecordCreated "$patch\CONFIG.NT";[IO.File]::WriteAllText("$patch\AUTOEXEC.NT",$auto,[Text.Encoding]::ASCII);RecordCreated "$patch\AUTOEXEC.NT"
  & "$release\PIF.EXE" create "$patch\WIN.PIF" --title 'Windows 95' --program "$WindowsRoot\WIN.COM" --directory $WindowsRoot --config "$patch\CONFIG.NT" --autoexec "$patch\AUTOEXEC.NT" --close-on-exit --w386-max-xms 65535 --w386-flags 0x1003 --nt31-flags 1;if($LASTEXITCODE -ne 0){Fail 'Could not create WIN.PIF'};RecordCreated "$patch\WIN.PIF"
  [IO.File]::WriteAllText("$patch\WIN.CMD",'@echo off' + "`r`n" + 'where /q run16 || exit /b 2' + "`r`n" + 'call run16 "%~dp0WIN.PIF"' + "`r`n",[Text.Encoding]::ASCII);RecordCreated "$patch\WIN.CMD"
  Write-Host "Win95 launcher prepared: $patch\WIN.CMD"
} catch {Write-Error $_;& "$PSScriptRoot\Undo-Win95Launch.ps1" -WindowsRoot $WindowsRoot -BestEffort;exit 1}
