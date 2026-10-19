[CmdletBinding()]
param([Parameter(Mandatory)][string]$ProviderBuildRoot,
 [Parameter(Mandatory)][string]$BuildRoot,[switch]$P21Control)
# Diagnostic whole-TU replacement, never a copied Ninja/CMake tree. Build the
# sealed P21 control BEFORE changing its validated provider's object closure.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$provider=(Resolve-Path $ProviderBuildRoot).Path;$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh disposable diagnostic root required'}
$normal=(Get-FileHash "$provider/wow32.dll").Hash
if($P21Control -and $normal -ne '59E008EC1F09A2228FCF87FDB346F205473EB7B6DEE55ED39983C866A96C263C'){throw 'P21 control requires its exact unmodified provider objects'}
$null=New-Item -ItemType Directory $root
$commands=@(& ninja.exe -C $provider -t commands wow32.dll)
if($LASTEXITCODE){throw 'Cannot inspect real provider commands'}
$compile=$commands|Where-Object {$_ -match '/wmdisp32\.c$'}|Select-Object -Last 1
$link=$commands|Where-Object {$_ -match '^link /nologo /machine:x86 /dll '}|Select-Object -Last 1
if(!$compile -or !$link){throw 'Actual message TU/link closure missing'}
$owner=if($P21Control){
 $s=(& git -C $repo show '4b8a7c8ae6eaaca3350d640650dd9d57201fd581:src/mvdm/wow32/wmdisp32.c') -join "`n"
 if($LASTEXITCODE){throw 'Cannot read immutable P21 owner'};$s
}else{[IO.File]::ReadAllText("$repo/src/mvdm/wow32/wmdisp32.c")}
[IO.File]::WriteAllText("$root/message_original_owner.c",$owner,[Text.UTF8Encoding]::new($false))
$source="$PSScriptRoot/wow_message_transaction_provider.c"
$compile=$compile -replace '/Fo\S+\s+\S+\.c$',('/I"'+$root+'" /Fo"'+$root+'/message.obj" "'+$source+'"')
$diagnosticLink=$link.Replace('obj/wmdisp32.obj',('"'+$root+'/message.obj"')).Replace('/map:wow32.dll.map',('/map:"'+$root+'/WOW32.map"')).Replace('/out:wow32.dll',('/out:"'+$root+'/WOW32.DLL" /implib:"'+$root+'/WOW32.lib"'))
if($diagnosticLink -eq $link -or $diagnosticLink.Contains('obj/wmdisp32.obj') -or $compile -notmatch 'wow_message_transaction_provider'){throw 'Diagnostic selection did not replace only its actual TU'}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
try{
 foreach($step in @(@('compile',$compile),@('link',$diagnosticLink))){
  $name=$step[0];$command=$step[1];$command|Set-Content "$root/$name-command.txt"
  $p=Start-Process cmd.exe -ArgumentList ('/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cd /d "'+$provider+'" && '+$command+'"') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
  $null=$p.Handle
  @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$command;deadlineSeconds=180}|ConvertTo-Json|Set-Content "$root/$name-process.json"
  if(!$p.WaitForExit(180000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned diagnostic deadline'}
  $p.WaitForExit();$p.ExitCode|Set-Content "$root/$name.exit";if($p.ExitCode){throw "$name failed"}
 }
 if((Get-FileHash "$provider/wow32.dll").Hash -ne $normal){throw 'Normal provider mutated'}
 $objects=@([regex]::Matches($link,'(?<!\S)obj/[^\s"]+\.(obj|res)')|ForEach-Object {Join-Path $provider $_.Value}|Select-Object -Unique)
 @($PSCommandPath,$source,"$root/message_original_owner.c","$provider/build.ninja")+$objects|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 @{kind='diagnostic-only';publishable=$false;target='MSGSEND';fault='original message callback BeforeCallback/AfterCallback';normalProvider=$normal;control=[bool]$P21Control;diagnosticProvider=(Get-FileHash "$root/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$root/diagnostic-manifest.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";1|Set-Content "$root/exit.txt";throw}
