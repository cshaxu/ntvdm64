param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned root required'}
$null=New-Item -ItemType Directory -Force $root
try {
 foreach($arch in @('x86','x64')) {
  $dir=Join-Path $root $arch
  $null=New-Item -ItemType Directory -Force $dir
  $compile="$repo/build/.ninja/msvc-$arch-release/style-fixtures"
  & "$repo/tools/build/New-Ntw16sNinja.ps1" -BuildRoot $compile -Architecture $arch -IncludeTests
  & $env:ComSpec /d /c "$compile/build-style.cmd" *> "$dir/build.txt"
  if($LASTEXITCODE){throw "Build failed: $arch"}
  Copy-Item "$compile/*.exe","$compile/*.dll" $dir
  $dll=if($arch -eq 'x86'){'ntw16s32.dll'}else{'ntw16s64.dll'}
  $p=Start-Process "$dir/title-marker-test.exe" -ArgumentList ('"'+$dir+'\'+$dll+'"') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/result.txt" -RedirectStandardError "$dir/error.txt"
  $null=$p.Handle
  if(!$p.WaitForExit(15000)){Stop-Process -Id $p.Id -Force;throw "Title fixture timeout: $arch"}
  $p.Refresh()
  if($null -eq $p.ExitCode -or $p.ExitCode -ne 0){throw "Title fixture failed: $arch exit=$($p.ExitCode)"}
  if((Get-Item "$dir/error.txt").Length -ne 0){throw "Title fixture emitted failure: $arch"}
 }
 '0'|Set-Content "$root/exit-code.txt"
} catch { $_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit-code.txt";exit 1 }
