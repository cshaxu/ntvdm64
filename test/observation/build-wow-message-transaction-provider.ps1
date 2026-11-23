[CmdletBinding()]
param([Parameter(Mandatory)][string]$ProviderBuildRoot,
 [Parameter(Mandatory)][string]$BuildRoot,[switch]$P21Control)
$ErrorActionPreference='Stop'
if($P21Control){
 $repo=(Resolve-Path "$PSScriptRoot/../..").Path
 $provider=(Resolve-Path $ProviderBuildRoot).Path;$root=[IO.Path]::GetFullPath($BuildRoot)
 if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
  $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
  $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh disposable diagnostic root required'}
 $normal=(Get-FileHash "$provider/wow32.dll").Hash
 if($normal -ne '59E008EC1F09A2228FCF87FDB346F205473EB7B6DEE55ED39983C866A96C263C'){throw 'P21 control requires its exact unmodified provider objects'}
 $null=New-Item -ItemType Directory $root
 New-Item -ItemType File "$root/.message-control-owned"|Out-Null
 $line='/d /s /c ""'+$repo+'/tools/build-wow-message-control.cmd" "'+$provider+'" "'+$root+'""'
 try {
  $p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/build.stdout" -RedirectStandardError "$root/build.stderr"
  try {
   $null=$p.Handle
   @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=360}|ConvertTo-Json|Set-Content "$root/build.owner.json"
   if(!$p.WaitForExit(360000)){$p.Kill($true);$p.WaitForExit();throw 'Historical message producer deadline'}
   $p.WaitForExit();$p.ExitCode|Set-Content "$root/build.exit"
   if($p.ExitCode){throw 'Native historical message diagnostic failed'}
  } finally {$p.Dispose()}
  if((Get-FileHash "$provider/wow32.dll").Hash -ne $normal){throw 'Normal provider mutated'}
  $link=Get-Content "$root/link.cmd" -Raw
  $objects=@([regex]::Matches($link,'(?<!\S)obj/[^\s"]+\.(obj|res)')|ForEach-Object {Join-Path $provider $_.Value}|Select-Object -Unique)
  @($PSCommandPath,"$repo/tools/build-wow-message-control.cmd","$PSScriptRoot/wow_message_transaction_provider.c","$root/message_original_owner.c","$provider/build.ninja","$root/message.obj")+$objects|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
  @{kind='diagnostic-only';publishable=$false;target='MSGSEND';fault='original message callback BeforeCallback/AfterCallback';normalProvider=$normal;control=$true;diagnosticProvider=(Get-FileHash "$root/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$root/diagnostic-manifest.json"
  0|Set-Content "$root/exit.txt"
 }catch{$_|Out-String|Set-Content "$root/error.txt";1|Set-Content "$root/exit.txt";throw}
 return
}
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$provider=(Resolve-Path $ProviderBuildRoot).Path;$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh disposable diagnostic root required'}
$normal=(Get-FileHash "$provider/wow32.dll").Hash
$null=New-Item -ItemType Directory $root
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-message-transaction "'+$provider+'""'
try {
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/build.stdout" -RedirectStandardError "$root/build.stderr"
try {
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$root/build.owner.json"
 if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'Message producer deadline'}
 $p.WaitForExit();$p.ExitCode|Set-Content "$root/build.exit"
 if($p.ExitCode){throw 'Native message diagnostic failed'}
} finally {$p.Dispose()}
$cache="$provider/diagnostics/message-transaction"
Copy-Item "$cache/WOW32.DLL","$cache/WOW32.map","$cache/WOW32.lib","$cache/message_original_owner.c" $root
if((Get-FileHash "$provider/wow32.dll").Hash -ne $normal){throw 'Normal provider mutated'}
$objects=@(Get-Content "$cache/objects.txt"|Where-Object {$_})
@($PSCommandPath,"$PSScriptRoot/wow_message_transaction_provider.c","$root/message_original_owner.c","$provider/build.ninja")+$objects|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
@{kind='diagnostic-only';publishable=$false;target='MSGSEND';fault='original message callback BeforeCallback/AfterCallback';normalProvider=$normal;control=$false;diagnosticProvider=(Get-FileHash "$root/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$root/diagnostic-manifest.json"
0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";1|Set-Content "$root/exit.txt";throw}
