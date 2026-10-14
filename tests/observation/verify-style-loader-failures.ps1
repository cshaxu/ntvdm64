param([Parameter(Mandatory)][string]$FixtureRoot,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;$fixtures=(Resolve-Path $FixtureRoot).Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $out
try {
 foreach($arch in @('x86','x64')){foreach($case in @('missing','wrong-width')){
  $dir="$out/$arch-$case/system32";$null=New-Item -ItemType Directory $dir
  foreach($file in @('style-loader-failure-test.exe','run16.exe','nthook32.dll','nthook64.dll')){
   Copy-Item "$fixtures/$arch/system32/$file" "$dir/$file"
  }
  $expected=126
  if($case -eq 'wrong-width'){
   $name=if($arch -eq 'x86'){'ntw16s32.dll'}else{'ntw16s64.dll'}
   $wrong=if($arch -eq 'x86'){'ntw16s64.dll'}else{'ntw16s32.dll'}
   Copy-Item "$fixtures/$arch/system32/$wrong" "$dir/$name";$expected=193
  }
  $p=Start-Process "$dir/style-loader-failure-test.exe" -ArgumentList "$expected" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/result.txt" -RedirectStandardError "$dir/error.txt"
  $null=$p.Handle
  if(!$p.WaitForExit(30000)){& taskkill /PID $p.Id /T /F;throw "Negative loader deadline $arch/$case"}
  $p.WaitForExit();if($p.ExitCode -ne 0){throw "Negative loader failed $arch/$case exit=$($p.ExitCode)"}
 }}
 '0'|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";'1'|Set-Content "$out/exit.txt";exit 1}
