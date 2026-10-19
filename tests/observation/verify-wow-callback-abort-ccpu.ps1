[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuildRoot,
      [Parameter(Mandatory)][string]$ProviderBuildRoot,
      [Parameter(Mandatory)][string]$BuildRoot,
      [ValidateSet('1122ab55d55b4d15699715ce1416b092b5887772')][string]$ReferenceRevision)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$worker=(Resolve-Path $WorkerBuildRoot).Path
$graph=Get-Content "$worker/build.ninja" -Raw
$providerGraph=Get-Content "$ProviderBuildRoot/build.ninja" -Raw
function Flags($body){[regex]::Match($body,'(?m)^cflags = (.*)\r?$').Groups[1].Value.Trim().Replace('$:',':')}
$flags=Flags $graph;$providerFlags=Flags $providerGraph
if(!$flags.Contains('/DCPU_40_STYLE') -or !$flags.Contains('/MT') -or !$providerFlags.Contains('/DMVDM_WOW32_PROVIDER')){throw 'Require actual CCPU40/MT worker and WOW32 graphs'}
# Reuse the selected original CPU fixture's real library/resource closure.
# No new permissive unresolved aliases or replacement CPU providers.
$dependencies=[regex]::Match($graph,'(?m)^build cpu40-descriptor-domain-fixture.exe: memory_test_link (.*)\r?$').Groups[1].Value.Trim() -split ' '
if($dependencies.Count -lt 30 -or $dependencies[0] -ne 'obj/tests/cpu40_descriptor_domain_fixture.obj'){throw 'Changed original CPU fixture closure needs review'}
$inputs=@($PSCommandPath,"$worker/build.ninja","$ProviderBuildRoot/build.ninja",
 "$repo/tests/mvdm-host/wow_callback_abort_ccpu_fixture.c",
 "$repo/tests/mvdm-host/wow_callback_abort_tls_fixture.c",
 "$repo/src/wow32-dll/source/wow_callback_frame_lease.c","$repo/src/wow32-dll/include/wow_callback_frame_lease.h",
 "$repo/src/mvdm/softpc.new/base/ccpu386/c_main.c","$repo/src/mvdm/softpc.new/base/ccpu386/ntthread.c",
 "$repo/src/mvdm/softpc.new/base/ccpu386/c_xtrn.c")+@($dependencies[1..($dependencies.Count-1)]|ForEach-Object {"$worker/$_"})
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$expected=0){
 $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');executable=$file;arguments=$arguments;deadline=$p.StartTime.AddSeconds(180).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name-process.json"
 if(!$p.WaitForExit(180000)){& taskkill.exe /PID $p.Id /T /F *> "$root/$name-timeout.txt";throw "Owned deadline $name"}
 $p.WaitForExit();$p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit"
 if($p.ExitCode -ne $expected){throw "Failed $name actual=$($p.ExitCode) expected=$expected"}
}
try{
 $targets=$dependencies[1..($dependencies.Count-1)] -join ' '
 Run-Owned prerequisites cmd.exe ('/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cd /d "'+$worker+'" && ninja.exe '+$targets+'"')
 Get-FileHash $inputs|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $cpuSource="$repo/src/mvdm/softpc.new/base/ccpu386/c_main.c";$fixtureExtra=''
 if($ReferenceRevision){
  foreach($name in @('c_main','ntthread','c_xtrn')){
   $body=(& git -C $repo show "${ReferenceRevision}:src/mvdm/softpc.new/base/ccpu386/$name.c") -join "`n"
   if($LASTEXITCODE){throw 'Missing immutable CPU owner source'}
   [IO.File]::WriteAllText("$root/reference-$name.c",$body)
  }
  $cpuSource="$root/reference-c_main.c"
  $fixtureExtra=' /DWOW_TEST_NTTHREAD_SOURCE=\"'+$root.Replace('\','/')+'/reference-ntthread.c\" /DWOW_TEST_XTRN_SOURCE=\"'+$root.Replace('\','/')+'/reference-c_xtrn.c\"'
 }
 $sources=@(
  @{name='fixture';source="$repo/tests/mvdm-host/wow_callback_abort_ccpu_fixture.c";flags=$flags+$fixtureExtra},
  @{name='tls';source="$repo/tests/mvdm-host/wow_callback_abort_tls_fixture.c";flags=$flags+$fixtureExtra},
  @{name='cpu';source=$cpuSource;flags=$flags},
  @{name='control';source="$repo/src/wow32-dll/source/wow_callback_frame_lease.c";flags=$providerFlags})
 foreach($item in $sources){
  $line='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe '+$item.flags+' /O2 /Gy /Gw /Fo"'+$root+'/'+$item.name+'.obj" "'+$item.source+'""'
  Run-Owned $item.name cmd.exe $line
 }
 $objects=@('fixture.obj','tls.obj','cpu.obj','control.obj'|ForEach-Object {'"'+$root+'/'+$_+'"'})+
  @($dependencies[1..($dependencies.Count-1)]|ForEach-Object {'"'+$worker+'/'+$_+'"'})
 # Same duplicate-symbol disposition as memory_test_link, not force:unresolved.
 $line='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && link.exe /nologo /OPT:REF /force:multiple /map:"'+$root+'/fixture.map" /out:"'+$root+'/fixture.exe" '+($objects -join ' ')+' kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"'
 Run-Owned link cmd.exe $line
 $map=Get-Content "$root/fixture.map" -Raw
 foreach($owner in @(@('ccpu386SimulatePtr','tls.obj'),@('call_cpu_function','fixture.obj'),@('c_cpu_simulate','cpu.obj'))){
  if($map -notmatch ('(?m)\s_'+$owner[0]+'\s+[^\r\n]+'+[regex]::Escape($owner[1]))){throw "Original owner not supplied by inspected fixture TU: $($owner[0])"}
 }
 if($ReferenceRevision){
  Run-Owned negative-simulate "$root/fixture.exe" '--simulate' 1
  Run-Owned negative-interface "$root/fixture.exe" '--interface' 1
  if((Get-Content "$root/negative-simulate.out" -Raw) -notmatch 'CCPU_ABORT_WRONG escaped-level' -or
     (Get-Content "$root/negative-interface.out" -Raw) -notmatch 'CCPU_INTERFACE_WRONG stale-interface'){throw 'Missing causal old-owner failures'}
 }else{
  Run-Owned verify "$root/fixture.exe" '--verify'
  $output=Get-Content "$root/verify.out" -Raw
  if(([regex]::Matches($output,'CCPU_ABORT_OK thread=')).Count -ne 3 -or
      $output -notmatch 'CCPU_INTERFACE_OK' -or $output -notmatch 'CCPU_CONTROL_OK'){throw 'Missing real CPU/thread/control assertions'}
 }
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Inputs changed during actual CPU proof'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";1|Set-Content "$root/exit.txt";throw}
