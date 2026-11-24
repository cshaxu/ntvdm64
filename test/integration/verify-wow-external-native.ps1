[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[switch]$BuildOnly,[switch]$CrossWorkers,[switch]$PendingSend,
 [string]$PackageRoot,[string]$ManifestPath)
$ErrorActionPreference='Stop'
if($CrossWorkers -and $PendingSend){throw 'Select one whole message group'}
if([bool]$PackageRoot -ne [bool]$ManifestPath -or ($BuildOnly -and $PackageRoot)){throw 'Candidate package and manifest must be paired for a runtime group'}
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
   $root.StartsWith((Join-Path $repo 'build/.ninja'),[StringComparison]::OrdinalIgnoreCase) -or
   $root.StartsWith((Join-Path $repo 'build/.ccache'),[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path $root)){throw 'Fresh disposable repository build root required'}
$null=New-Item -ItemType Directory $root
Set-Location $repo
$pwsh=(Get-Command pwsh.exe).Source
$steps=[Collections.Generic.List[object]]::new()
$started=[DateTime]::UtcNow
function Run-Owned([string]$Name,[string]$File,[string[]]$Arguments,[int]$Seconds,[int]$ExpectedExit=0){
 $stdout="$root/$Name.stdout.log";$stderr="$root/$Name.stderr.log"
 $start=@{FilePath=$File;WorkingDirectory=$repo;WindowStyle='Hidden';PassThru=$true;
  RedirectStandardOutput=$stdout;RedirectStandardError=$stderr}
 if($Arguments.Count){$start.ArgumentList=$Arguments}
 $process=Start-Process @start
 $null=$process.Handle
 $row=[ordered]@{name=$Name;pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');file=$File;arguments=$Arguments;deadlineSeconds=$Seconds;stdout=$stdout;stderr=$stderr;exit=$null}
 $steps.Add($row);$steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 $last=[DateTime]::UtcNow;$bytes=-1L
 while(!$process.WaitForExit(1000)){
  $now=(Get-Item $stdout).Length+(Get-Item $stderr).Length
  if($now -ne $bytes){$bytes=$now;$last=[DateTime]::UtcNow}
  if(([DateTime]::UtcNow-$process.StartTime.ToUniversalTime()).TotalSeconds -gt $Seconds -or
     ([DateTime]::UtcNow-$started).TotalMinutes -gt 60 -or
     ([DateTime]::UtcNow-$last).TotalMinutes -gt 15 -or $bytes -gt 64MB){
   & taskkill.exe /PID $process.Id /T /F | Out-Null;$process.WaitForExit();throw "Owned deadline/log budget: $Name"
  }
 }
 $row.exit=$process.ExitCode;$row.end=[DateTime]::UtcNow.ToString('o')
 $steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if($process.ExitCode -ne $ExpectedExit){throw "$Name failed ($($process.ExitCode), expected $ExpectedExit); preserved $stdout / $stderr"}
 Write-Output "PASS $Name"
}
try{
 foreach($arch in @('x86','x64')){
  $target=if($PendingSend){'wow-message-pending'}elseif($CrossWorkers){'wow-message-cross'}else{'wow-message-external'}
  Run-Owned "build-$arch" $env:ComSpec @('/d','/s','/c',('""'+$repo+'/tools/build-fixtures.cmd" '+$arch+' '+$target+'"')) 1200
 }
 if($PendingSend){foreach($arch in @('x86','x64')){
  $control=Join-Path $repo "build/.ninja/msvc-$arch-release/message-probes/MSGDEATH.exe"
  Run-Owned "native-death-$arch" $control @() 30
  $bits=if($arch -eq 'x86'){32}else{64}
  $observed=Get-Content "$root/native-death-$arch.stdout.log" -Raw
  foreach($kind in @('same','cross')){
   if($observed -notmatch "(?m)^case=$kind bits=$bits status=1 error=0 result=447A007C destroyed=1\r?`$"){throw 'Native destruction contract differs from actual guest assertions'}
  }
 }}
 if(!$BuildOnly){
  $guestManifest=Get-Content assets/winnt-guest-manifest.json -Raw|ConvertFrom-Json
  if((Get-FileHash assets/winnt.zip).Hash -ne $guestManifest.archive_sha256){throw 'Guest ZIP differs from approved manifest'}
  $expectedNames=@((Get-Content assets/release/release-manifest.json -Raw|ConvertFrom-Json).name)
  if($PackageRoot){
   $package=(Resolve-Path -LiteralPath $PackageRoot).Path
   if(!$package.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Candidate must be build-owned'}
   $ManifestPath=(Resolve-Path -LiteralPath $ManifestPath).Path
   $manifestHash=Get-FileHash -LiteralPath $ManifestPath
    if([IO.Path]::GetExtension($ManifestPath) -ieq '.json'){
     $manifest=@(Get-Content -LiteralPath $ManifestPath -Raw|ConvertFrom-Json)
     # Windows PowerShell 5.1 yields one array object for a top-level JSON
     # array, while PowerShell 7 enumerates it.  Flatten only that parser
     # representation; do not relax the current product identity contract.
     if($manifest.Count -eq 1 -and $manifest[0] -is [array]){$manifest=@($manifest[0])}
     if($manifest.Count -ne $expectedNames.Count -or @($manifest.name|Sort-Object -Unique).Count -ne $manifest.Count -or @(Compare-Object ($manifest.name|Sort-Object) ($expectedNames|Sort-Object)).Count){throw 'Need coherent current candidate JSON manifest'}
     foreach($image in $manifest){if((Get-FileHash "$package/SYSTEM32/$($image.name)").Hash -ne $image.sha256){throw 'Candidate JSON identity mismatch'}}
    }else{
    $manifest=@(Import-Csv -LiteralPath $ManifestPath)
    if($manifest.Count -lt $expectedNames.Count -or @($manifest.Path|Sort-Object -Unique).Count -ne $manifest.Count){throw 'Need coherent candidate CSV manifest'}
    $system32Root=[IO.Path]::GetFullPath((Join-Path $package 'SYSTEM32'))+'\'
    foreach($image in $manifest){
     $path=[IO.Path]::GetFullPath($image.Path)
     if(!$path.StartsWith($system32Root,[StringComparison]::OrdinalIgnoreCase) -or !(Test-Path -LiteralPath $path) -or (Get-FileHash -LiteralPath $path).Hash -ne $image.Hash){throw 'Candidate CSV identity mismatch'}
    }
   }
   $manifestHash|Export-Csv -NoTypeInformation "$root/candidate-manifest.csv"
  }else{
   [IO.Compression.ZipFile]::ExtractToDirectory((Join-Path $repo 'assets/winnt.zip'),"$root/media")
   $package="$root/media/WINNT"
   $manifest=Get-Content assets/release/release-manifest.json -Raw|ConvertFrom-Json
   if($manifest.Count -ne $expectedNames.Count -or @($manifest.name|Sort-Object -Unique).Count -ne $manifest.Count){throw 'Need coherent current release manifest'}
   foreach($image in $manifest){
    $source=Join-Path $repo "assets/release/$($image.name)"
    if((Get-FileHash $source).Hash -ne $image.sha256){throw 'Release identity mismatch'}
    Copy-Item $source "$package/SYSTEM32/$($image.name)" -Force
   }
  }
  $observer=Join-Path $repo 'build/.ninja/msvc-x64-release/message-probes/console-startup-observer.exe'
  $driver=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
  $localHost=Join-Path $repo 'build/.ninja/msvc-x86-release/message-probes/MSGHOSTLOCAL.dll'
  $externalHost=Join-Path $repo 'build/.ninja/msvc-x86-release/message-probes/MSGHOST.dll'
  $crossHost=Join-Path $repo 'build/.ninja/msvc-x86-release/message-probes/MSGHOSTCROSS.dll'
  $coordinator=Join-Path $repo 'build/.ninja/msvc-x64-release/message-probes/MSGXHOST.exe'
  $pendingHost=Join-Path $repo 'build/.ninja/msvc-x86-release/message-probes/MSGHOST_pending.dll'
  $script=Join-Path $PSScriptRoot 'verify-win16-message-probe.ps1'
  $kinds=if($PendingSend){@('local','pending')}elseif($CrossWorkers){@('local','cross')}else{@('local','x86','x64')}
  foreach($kind in $kinds){
   $name=$kind;$hostImage=if($kind -eq 'local'){$localHost}elseif($kind -eq 'pending'){$pendingHost}elseif($kind -eq 'cross'){$crossHost}else{$externalHost}
   $arguments=@('-NoProfile','-ExecutionPolicy','Bypass','-File',"`"$script`"",'-PackageRoot',"`"$package`"",'-Observer',"`"$observer`"",'-CompilerDriver',"`"$driver`"",'-BuildRoot',"`"$root/$name`"",'-StructuredPayloads','-ThreadClientView','-NativeHost',"`"$hostImage`"")
   if($kind -eq 'pending'){$arguments+='-PendingSend'}
   elseif($kind -eq 'cross'){$arguments+=@('-CrossWorkerCoordinator',"`"$coordinator`"")}
   elseif($kind -ne 'local'){$peer=Join-Path $repo "build/.ninja/msvc-$kind-release/message-probes/MSGPEER.exe";$arguments+=@('-ExternalNativePeer',"`"$peer`"")}
   Run-Owned $name $pwsh $arguments 120
   if(!(Test-Path "$root/$name/exit.txt") -or (Get-Content "$root/$name/exit.txt").Trim() -ne '0'){throw "Missing actual result: $name"}
   if($kind -eq 'local' -and ((Get-FileHash "$root/$name/MSGSEND.EXE").Hash -ne 'B0692BB2A0948B2AAE4C4E98E703CEF39200C8FFEEABFFF8AA9038DC9CDF86BB' -or
      (Get-FileHash "$root/$name/MSGRECV.EXE").Hash -ne '356AB86E785A1DF9F137B77D60B0D606CC450010F24F95729F44C99F9534DEF8')){throw 'Default authored NE images changed from P14'}
  }
  if($PendingSend){
   $badHost=Join-Path $repo 'build/.ninja/msvc-x86-release/message-probes/MSGHOST_nonblock.dll'
   $arguments=@('-NoProfile','-ExecutionPolicy','Bypass','-File',"`"$script`"",'-PackageRoot',"`"$package`"",'-Observer',"`"$observer`"",'-CompilerDriver',"`"$driver`"",'-BuildRoot',"`"$root/nonblock-negative`"",'-StructuredPayloads','-ThreadClientView','-PendingSend','-NativeHost',"`"$badHost`"")
   Run-Owned 'nonblock-negative' $pwsh $arguments 120 1
   if((Get-Content "$root/nonblock-negative/native-failures.txt" -Raw) -notmatch 'FAIL reverse callback before blocked send returned'){throw 'No-SMTO_BLOCK control did not demonstrate forbidden ordering'}
  }
 }
 @{passed=$true;buildOnly=[bool]$BuildOnly;crossWorkers=[bool]$CrossWorkers;pendingSend=[bool]$PendingSend;cases=$(if($BuildOnly){0}elseif($CrossWorkers -or $PendingSend){6}else{9});start=$started.ToString('o');end=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$root/result.json"
}catch{
 @{passed=$false;error=$_.ToString();start=$started.ToString('o');end=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$root/result.json"
 Write-Error $_;exit 1
}
