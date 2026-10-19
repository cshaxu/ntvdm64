[CmdletBinding()]
param([Parameter(Mandatory)][string]$EvidenceRoot,
 [string]$VsDevCmd='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat')
# Raw USER observation, not production/WOW acceptance. A protected alias is
# recorded without a dereference; a completed observation does not prove capture.
# Invoke as an owned hidden background process; all outputs are disposable.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($EvidenceRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
 throw 'Fresh disposable evidence root required'
}
$null=New-Item -ItemType Directory $root
$steps=[Collections.Generic.List[object]]::new();$observations=@();$unsafe=@()
function Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds){
 $options=@{FilePath=$file;WorkingDirectory=$repo;WindowStyle='Hidden';PassThru=$true;
  RedirectStandardOutput="$root/$name.out";RedirectStandardError="$root/$name.err"}
 if($arguments){$options.ArgumentList=$arguments}
 $p=Start-Process @options;$null=$p.Handle
 $row=[ordered]@{name=$name;pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');file=$file;arguments=$arguments;deadlineSeconds=$seconds;exit=$null}
 $steps.Add($row);$steps|ConvertTo-Json -Depth 4|Set-Content "$root/steps.json"
 if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F|Out-Null;$p.WaitForExit();throw "Owned deadline $name"}
 $p.WaitForExit();$p.Refresh();$row.exit=$p.ExitCode;$row.elapsedSeconds=([DateTime]::UtcNow-$p.StartTime.ToUniversalTime()).TotalSeconds
 $p.Dispose();$steps|ConvertTo-Json -Depth 4|Set-Content "$root/steps.json"
 if($row.exit){throw "$name failed $($row.exit)"}
}
try{
 $hostIdentity=[ordered]@{os=[Environment]::OSVersion.Version.ToString();
  user32Version=(Get-Item "$env:SystemRoot/System32/user32.dll").VersionInfo.FileVersion;
  user32Sha256=(Get-FileHash "$env:SystemRoot/System32/user32.dll").Hash;
  wowUser32Version=(Get-Item "$env:SystemRoot/SysWOW64/user32.dll").VersionInfo.FileVersion;
  wowUser32Sha256=(Get-FileHash "$env:SystemRoot/SysWOW64/user32.dll").Hash}
 $hostIdentity|ConvertTo-Json|Set-Content "$root/host.json"
 $sources=@("$PSScriptRoot/wow_message_capture_control.c",$PSCommandPath,
  "$repo/tests/build/wow-message-probes.cmake")
 $sources|Get-FileHash|Export-Csv -NoTypeInformation "$root/source-inputs.csv"
 foreach($arch in @('x86','x64')){
  $build="$repo/build/.ninja/msvc-$arch-release"
  if(!(Test-Path "$build/build.ninja")){throw 'Configured stable Ninja tree required'}
  $commands=@(& ninja.exe -C $build -t commands message-probes/MSGCAPTURE.exe)
  if($LASTEXITCODE){throw 'Cannot recover actual native probe graph'}
  $text=$commands -join "`n"
  if($text -notmatch 'wow_message_capture_control\.c' -or $text -notmatch '/O2' -or
   $text -notmatch '[-/]MT\s' -or $text -notmatch '/W4' -or $text -notmatch '/WX' -or
   $text -match 'src[/\\](mvdm|wow32-dll)[/\\]'){
   throw 'Require native-only Release/MT strict probe graph'
  }
  $commands|Set-Content "$root/$arch-commands.txt"
  $line='/d /s /c "call "'+$VsDevCmd+'" -arch='+$arch+' -host_arch=x64 >nul && cmake --build "'+$build+'" --target wow_capture_native_control"'
  Owned "build-$arch" 'cmd.exe' $line 1200
  $exe="$build/message-probes/MSGCAPTURE.exe"
  $bytes=[IO.File]::ReadAllBytes($exe);$pe=[BitConverter]::ToInt32($bytes,60)
  if([BitConverter]::ToUInt16($bytes,$pe+4) -ne $(if($arch -eq 'x86'){0x14c}else{0x8664})){throw 'Probe width mismatch'}
  Get-FileHash $exe,"$build/build.ninja","$build/CMakeCache.txt"|Export-Csv -NoTypeInformation "$root/$arch-artifact-inputs.csv"
  foreach($mode in @('default','extended')){
   $argsLine=if($mode -eq 'extended'){'--extended'}else{''}
   Owned "$arch-$mode" $exe $argsLine 180
   $output=Get-Content "$root/$arch-$mode.out" -Raw
   $rows=@([regex]::Matches($output,'(?m)^message=([0-9A-F]+) bits=(\d+) alias=(\d+) data_alias=(\d+).*capture_write=(\d+) full_return=1\r?$'))
   $expected=if($mode -eq 'extended'){10}else{4}
   if($output -notmatch '(?m)^errors=0\r?$' -or $rows.Count -ne $expected){throw 'Incomplete native capture observations'}
   $details=@([regex]::Matches($output,'(?m)^detail case=(\d+) message=([0-9A-F]+) wparam=(\d+) nested_write=(\d+) nested_required=(\d+)\r?$'))
   if($mode -eq 'extended' -and $details.Count -ne 10){throw 'Missing extended case identities'}
   for($i=0;$i -lt $rows.Count;++$i){
    $row=$rows[$i];$message=$row.Groups[1].Value
    if([int]$row.Groups[2].Value -ne $(if($arch -eq 'x86'){32}else{64})){throw 'Reported width mismatch'}
    # The old three output controls are retained strict positives, not just
    # a successful process exit. COPYDATA is input-only and never written.
    if($i -lt 3 -and ($row.Groups[3].Value -ne '0' -or $row.Groups[5].Value -ne '1')){throw 'Retained output capture regressed'}
    $detail=if($mode -eq 'extended'){$details[$i]}else{$null}
    if($detail -and ([int]$detail.Groups[1].Value -ne $i -or $detail.Groups[2].Value -ne $message)){throw 'Extended case order mismatch'}
    $private=$row.Groups[3].Value -eq '0' -and $row.Groups[5].Value -eq '1'
    $nested=$detail -and $detail.Groups[5].Value -eq '1'
    $nestedPrivate=!$nested -or ($row.Groups[4].Value -eq '0' -and $detail.Groups[4].Value -eq '1')
    $copyout=[regex]::Match($row.Value,'output_at_reply=(-?\d+)')
    if(!$copyout.Success){throw 'Missing early reply output observation'}
    $fact=[ordered]@{arch=$arch;mode=$mode;index=$i;message=$message;inputOnly=($message -eq '004A');privateOutputStorage=$private;nestedRequired=[bool]$nested;privateNestedStorage=[bool]$nestedPrivate;outputAtReply=[int]$copyout.Groups[1].Value}
    $observations+=$fact
    if($mode -eq 'extended' -and !$fact.inputOnly -and (!$private -or !$nestedPrivate)){$unsafe+=$fact}
   }
  }
 }
 foreach($input in Import-Csv "$root/source-inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Source changed during observation group'}}
 $observations|ConvertTo-Json -Depth 4|Set-Content "$root/observations.json"
 @{nativeObservationComplete=$true;host=$hostIdentity;retainedDefaultCases=8;extendedCases=20;runtimeAcceptance=$false;
  allExtendedOutputsHavePrivateStorage=($unsafe.Count -eq 0);unprovedDirectOutput=$unsafe;
  limit='Observed current USER build only; alias is not silently declared safe or a product pass'}|
  ConvertTo-Json -Depth 5|Set-Content "$root/result.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";1|Set-Content "$root/exit.txt";exit 1}
