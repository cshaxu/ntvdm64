param([Parameter(Mandatory)][string]$BuildRoot,[string]$NegativeSystem32,[string]$System32)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if($NegativeSystem32){$NegativeSystem32=(Resolve-Path $NegativeSystem32).Path}
if($System32){$System32=(Resolve-Path $System32).Path}
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build root required'}
$null=New-Item -ItemType Directory $root
try{
 foreach($arch in @('x86','x64')){
  $dir="$root/$arch"
  & "$repo/tools/build/New-Ntw16sNinja.ps1" -BuildRoot $dir -Architecture $arch -IncludeTests
  & "$dir/build-style.cmd" *> "$dir/build.txt"
  if($LASTEXITCODE){throw "Build failed $arch"}
  $dll=if($arch -eq 'x86'){'ntw16s32.dll'}else{'ntw16s64.dll'}
  $cases=@('candidate');if($NegativeSystem32){$cases+='previous'}
  foreach($case in $cases){
   $path=if($case -eq 'previous'){Join-Path $NegativeSystem32 $dll}elseif($System32){Join-Path $System32 $dll}else{Join-Path $dir $dll}
   $p=Start-Process "$dir/style-dialog-test.exe" -ArgumentList ('"'+$path+'"') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/$case.txt" -RedirectStandardError "$dir/$case.err"
   $null=$p.Handle
   @{pid=$p.Id;start=$p.StartTime;deadlineSeconds=60;input=(Get-FileHash $path).Hash}|ConvertTo-Json|Set-Content "$dir/$case-owner.json"
   if(!$p.WaitForExit(60000)){$p.Kill();throw "Dialog timeout $arch/$case"};$p.Refresh()
   if($null -eq $p.ExitCode -or ($case -eq 'candidate' -and $p.ExitCode) -or ($case -eq 'previous' -and !$p.ExitCode)){throw "Unexpected dialog result $arch/$case $($p.ExitCode)"}
   if($case -eq 'previous' -and !(Select-String -Quiet -SimpleMatch -Path "$dir/$case.err" -Pattern 'attached(control)==enabled')){throw "Negative missed unsupported separator: $arch"}
  }
 }
 '0'|Set-Content "$root/exit-code.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit-code.txt";exit 1}
