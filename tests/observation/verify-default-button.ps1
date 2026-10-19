param([Parameter(Mandatory)][string]$BuildRoot,[string]$NegativeSystem32)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build root required'}
$null=New-Item -ItemType Directory $root
try {
 foreach($arch in @('x86','x64')) {
  $dir="$root/$arch"
  $null=New-Item -ItemType Directory -Force $dir
  $compile="$repo/build/.ninja/msvc-$arch-release/style-fixtures"
  & "$repo/tools/build/New-Ntw16sNinja.ps1" -BuildRoot $compile -Architecture $arch -IncludeTests
  & $env:ComSpec /d /c "$compile/build-style.cmd" *> "$dir/build.txt"
  if($LASTEXITCODE){throw "Build failed $arch"}
  Copy-Item "$compile/*.exe","$compile/*.dll" $dir
  $dll=if($arch -eq 'x86'){'ntw16s32.dll'}else{'ntw16s64.dll'}
  foreach($case in @('candidate','previous')) {
   if($case -eq 'previous' -and !$NegativeSystem32){continue}
   $path=if($case -eq 'previous'){Join-Path $NegativeSystem32 $dll}else{Join-Path $dir $dll}
   $p=Start-Process "$dir/default-button-test.exe" -ArgumentList ('"'+$path+'"') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/$case.txt" -RedirectStandardError "$dir/$case.err"
   $null=$p.Handle
   @{pid=$p.Id;start=$p.StartTime;deadline=$p.StartTime.AddSeconds(20);input=(Get-FileHash $path).Hash}|ConvertTo-Json|Set-Content "$dir/$case-owner.json"
   if(!$p.WaitForExit(20000)){$p.Kill();throw "Default button timeout $arch/$case"};$p.Refresh()
   if($null -eq $p.ExitCode -or ($case -eq 'candidate' -and $p.ExitCode) -or ($case -eq 'previous' -and !$p.ExitCode)){throw "Unexpected default button result $arch/$case $($p.ExitCode)"}
   if($case -eq 'previous' -and !(Select-String -Quiet -SimpleMatch -Path "$dir/$case.err" -Pattern 'GetPixel(dc,x,25)==RGB(0,0,0)')){throw "Negative did not detect the discontinuous default frame: $arch"}
  }
 }
 '0'|Set-Content "$root/exit-code.txt"
}catch {$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit-code.txt";exit 1}
