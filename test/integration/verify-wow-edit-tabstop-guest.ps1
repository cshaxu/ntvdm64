[CmdletBinding()]
param([Parameter(Mandatory)][string]$ProbePath,
      [Parameter(Mandatory)][string]$OutputRoot,
      [Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$WowMap,
      [Parameter(Mandatory)][string]$WowDll)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $out)) { throw 'Require a fresh build-owned evidence directory' }
$null=New-Item -ItemType Directory -Path $out
function Phase($x){ [IO.File]::AppendAllText((Join-Path $out 'tabstop-snapshot.phase.txt'),([DateTime]::UtcNow.ToString('o')+' '+$x+"`n")) }
Phase 'start'
$probe=(Resolve-Path -LiteralPath $ProbePath).Path
# Bind the raw build DLL to the staged manifest before using its map RVA.
$mapText=Get-Content $WowMap -Raw
$baseMatch=[regex]::Match($mapText,'Preferred load address is ([0-9A-Fa-f]+)')
$symbol=[regex]::Match($mapText,'(?m)^\s*[0-9A-Fa-f]+:[0-9A-Fa-f]+\s+_W32WordBreakProc\s+([0-9A-Fa-f]+)\s+wuman.obj')
if(!$baseMatch.Success -or !$symbol.Success){throw 'Expected exact word-break provider map'}
$rva=[Convert]::ToInt64($symbol.Groups[1].Value,16)-[Convert]::ToInt64($baseMatch.Groups[1].Value,16)
$manifest=@(Get-Content "$PackageRoot/product-system32-manifest.json" -Raw|ConvertFrom-Json)
$wow=@($manifest|Where-Object {$_.Name -ieq 'WOW32.DLL'})
if($wow.Count -ne 1 -or (Get-FileHash $WowDll).Hash -ne $wow[0].InputSha256 -or (Get-FileHash "$PackageRoot/$($wow[0].RelativePath)").Hash -ne $wow[0].Sha256){throw 'Build/package identity mismatch'}
@{mapSha256=(Get-FileHash $WowMap).Hash;buildWowSha256=(Get-FileHash $WowDll).Hash;package=$wow[0];rva=$rva}|ConvertTo-Json -Depth 4|Set-Content "$out/identity.json"
$runtime=$null;$scope=$null;$proc=$null;$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 Phase 'before-stage'
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path -FreshPackageIdentity
 Phase ('stage='+$runtime)
 $scope=New-IsolatedPackageScope $runtime
 Phase 'scope-ready'
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory -Path $tests -Force
 Copy-Item -LiteralPath $probe -Destination (Join-Path $tests 'TABTEST.EXE')
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$(Get-PackageBinaryRoot $runtime);$env:SystemRoot\System32";$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 $log=Join-Path $out 'tabstop-snapshot.observer.txt'
 foreach($arg in @((Join-Path (Get-PackageBinaryRoot $runtime) 'run16.exe'),$tests,$log,'--wait',"$tests/TABTEST.EXE",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
 Phase 'before-observer'
 $proc=[Diagnostics.Process]::Start($start);$null=$proc.Handle
 Phase ('observer='+$proc.Id)
 Phase 'before-add-type'
 Add-Type -TypeDefinition @"
using System; using System.Runtime.InteropServices;
public static class TabstopReader {
 [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenProcess(uint access, bool inherit, int pid);
 [DllImport("kernel32.dll", SetLastError=true)] public static extern bool ReadProcessMemory(IntPtr h, IntPtr address, byte[] bytes, UIntPtr count, out UIntPtr got);
 [DllImport("kernel32.dll")] public static extern bool CloseHandle(IntPtr h);
}
"@
 Phase 'add-type-done'
 $report=Join-Path $tests 'TABTEST.BIN';$snapshots=@()
 foreach($stage in 1,2) {
  $required=if($stage -eq 1){138}else{184};$deadline=[DateTime]::UtcNow.AddSeconds(30)
  while(!(Test-Path -LiteralPath $report) -or (Get-Item -LiteralPath $report).Length -lt $required) {
   if($proc.HasExited -or [DateTime]::UtcNow -gt $deadline){throw "Guest did not reach stage $stage"};Start-Sleep -Milliseconds 100
  }
  Phase ('report-stage='+$stage)
  $workerPath=(Join-Path (Get-PackageBinaryRoot $runtime) 'ntvdm.exe')
  Phase 'before-cim'
  $workers=@(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" | Where-Object {$_.ExecutablePath -eq $workerPath})
  if($workers.Count -ne 1){throw "Expected one owned worker, got $($workers.Count)"}
  Phase 'after-cim'
  $worker=[Diagnostics.Process]::GetProcessById($workers[0].ProcessId)
  $dll=@($worker.Modules|Where-Object {$_.ModuleName -ieq 'WOW32.DLL'})
  if($dll.Count -ne 1 -or $dll[0].FileName -ine (Join-Path (Get-PackageBinaryRoot $runtime) 'WOW32.DLL')){throw 'Unexpected WOW32 module'}
  $address=[IntPtr]::Add($dll[0].BaseAddress,$rva);$h=[TabstopReader]::OpenProcess(0x410,$false,$worker.Id)
  if($h -eq [IntPtr]::Zero){throw 'OpenProcess failed'}
  try { $bytes=New-Object byte[] 4;$n=[UIntPtr]::Zero
   if(![TabstopReader]::ReadProcessMemory($h,$address,$bytes,[UIntPtr]4,[ref]$n) -or $n.ToUInt64() -ne 4){throw 'ReadProcessMemory failed'}
  } finally {[void][TabstopReader]::CloseHandle($h)}
  $snapshots+=@{stage=$stage;pid=$worker.Id;image=$workerPath;module=$dll[0].FileName;moduleBase=$dll[0].BaseAddress.ToInt64();rva=$rva;wordBreakProc=[BitConverter]::ToUInt32($bytes,0);recordSize=(Get-Item -LiteralPath $report).Length;nonAtomic=$true}
  $snapshots|ConvertTo-Json|Set-Content (Join-Path $out 'tabstop-snapshot.memory.json')
  [IO.File]::WriteAllText((Join-Path $tests ("TABGO$stage")), 'x')
 }
 if(!$proc.WaitForExit(45000)){throw 'Observer supervisor exceeded bounded interval'}
 if($snapshots.Count -ne 2 -or $snapshots[0].wordBreakProc -ne $snapshots[1].wordBreakProc){throw 'Tab-stop changed word-break global'}
 Phase 'observer-ended'
 $proc.Refresh();$code=$proc.ExitCode
 $result=Join-Path $tests 'TABTEST.BIN'
 if(!(Test-Path -LiteralPath $result -PathType Leaf)){throw 'Guest report is missing'}
 $observation=Get-Content -LiteralPath $log -Raw
 if($observation -notmatch '(?m)^result=exited\r?$' -or $observation -notmatch '(?m)^exit=0x00000000\r?$'){throw 'Guest did not exit successfully'}
 $data=[IO.File]::ReadAllBytes($result)
 if($data.Length -lt 230 -or $data.Length % 46){throw 'Invalid guest record extent'}
 $records=@()
 for($i=0;$i -lt $data.Length;$i+=46){
  if([Text.Encoding]::ASCII.GetString($data,$i,4) -ne 'CTL1'){throw 'Invalid guest record magic'}
  $row=@{phase=[BitConverter]::ToUInt16($data,$i+4);errors=[BitConverter]::ToUInt16($data,$i+6);tabstop=[BitConverter]::ToUInt32($data,$i+38);reset=[BitConverter]::ToUInt32($data,$i+42)}
  if($row.errors){throw 'Guest control reported failure'}
  $records+=$row
 }
 $last=$records[-1]
 if($last.phase -ne 3 -or $last.tabstop -ne 1 -or $last.reset -ne 1){throw 'Guest did not finish successful tab-stop/reset controls'}
 $records|ConvertTo-Json|Set-Content "$out/guest-records.json"
 Copy-Item -LiteralPath $result -Destination (Join-Path $out 'tabstop-snapshot.report.bin')
 Copy-Item -LiteralPath (Join-Path (Get-PackageBinaryRoot $runtime) 'WOW32.DLL') -Destination (Join-Path $out 'staged-WOW32.dll')
 @{observerExit=$code;probeSha256=(Get-IsolatedFileSha256 $probe);stagedWow32Sha256=(Get-IsolatedFileSha256 (Join-Path (Get-PackageBinaryRoot $runtime) 'WOW32.DLL'));runtime=$runtime;observerLog=$log}|ConvertTo-Json|Set-Content (Join-Path $out 'tabstop-snapshot.run.json')
 if($code){throw "Observer exit $code"}
} catch { Phase 'catch'
 $_|Out-String|Set-Content (Join-Path $out 'tabstop-snapshot.failure.txt');throw } finally {
 Phase 'cleanup'
 foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
 if($proc){$proc.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}} finally {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
