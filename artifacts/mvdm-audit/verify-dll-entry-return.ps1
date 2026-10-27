[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$WowBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32'))
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)){throw 'Fresh build-owned evidence directory required'}
$null=New-Item -ItemType Directory -Path $root
$wow=(Resolve-Path -LiteralPath $WowBuildRoot).Path
$bridge=Join-Path $wow 'wow32-dll-entry-bridge.c'
$object=Join-Path $wow 'obj/wow32-dll-entry-bridge.obj'
$driver=Join-Path $wow 'msvc-x86.cmd'
$source=Join-Path $PSScriptRoot 'dll-entry-return-probe.c'
$text=Get-Content -LiteralPath $bridge -Raw
if($text -notmatch 'extern int __stdcall W32DllInitialize'){throw 'Expected audited wide-return declaration'}
$candidate=Join-Path $root 'candidate-bridge.c'
[IO.File]::WriteAllText($candidate,$text.Replace('extern int __stdcall W32DllInitialize','extern unsigned char __stdcall W32DllInitialize'),[Text.UTF8Encoding]::new($false))
$runs=@()
function Owned([string]$Name,[string]$Image,[string]$Arguments){
 $p=Start-Process -FilePath $Image -ArgumentList $Arguments -WorkingDirectory $root -WindowStyle Hidden -RedirectStandardOutput "$root/$Name.stdout.txt" -RedirectStandardError "$root/$Name.stderr.txt" -PassThru
 try{
  $start=$p.StartTime.ToUniversalTime()
  if(!$p.WaitForExit(60000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned probe deadline: $Name"}
  $p.WaitForExit()
  $script:runs+=@{name=$Name;pid=$p.Id;started=$start.ToString('o');ended=$p.ExitTime.ToUniversalTime().ToString('o');exit=$p.ExitCode;image=$Image;command=$Arguments}
  if($p.ExitCode){throw "Probe failed: $Name"}
 }finally{$p.Dispose()}
}
foreach($case in @('actual','candidate')){
 $provider=if($case -eq 'actual'){'"'+$object+'" /DEXPECT_LEGACY_WIDE_RETURN'}else{'"'+$candidate+'"'}
 $exe=Join-Path $root "$case.exe"
 $line='/d /s /c ""'+$driver+'" cl.exe /nologo /MT /W4 /Fo"'+$root+'/" /Fe"'+$exe+'" "'+$source+'" '+$provider+'"'
 Owned "$case-build" $env:ComSpec $line
 Owned $case $exe ' '
}
$actual=Get-Content "$root/actual.stdout.txt" -Raw
$fixed=Get-Content "$root/candidate.stdout.txt" -Raw
if($actual -notmatch 'false_attach=a55a0000.*cleanups=2' -or $fixed -notmatch 'attach=00000001 false_attach=00000000.*cleanups=2'){throw 'Unexpected BOOLEAN return/cleanup witness'}
$inputs=@(Get-FileHash -LiteralPath $bridge,$object,$driver,$source,$candidate,$PSCommandPath)
@{result='pass';runs=$runs;inputs=$inputs;actual=$actual;candidate=$fixed;scope='Controlled valid BOOLEAN provider at actual selected bridge; not original heap failure or real DLL loader acceptance';product_changed=$false}|ConvertTo-Json -Depth 6|Set-Content "$root/result.json"
