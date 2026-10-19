param([Parameter(Mandatory)][string]$ProviderBuildRoot,
      [Parameter(Mandatory)][string]$BuildRoot,[string]$Ninja='ninja.exe')
# Run as an owned background job after the complete normal build. No graph
# copy or product object mutation; only one full test TU replaces wcall16.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$provider=(Resolve-Path $ProviderBuildRoot).Path;$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$commands=@(& $Ninja -C $provider -t commands wow32.dll)
if($LASTEXITCODE){throw 'Cannot read actual provider commands'}
$compile=$commands|Where-Object {$_ -match '/wcall16\.c$'}|Select-Object -Last 1
$link=$commands|Where-Object {$_ -match '^link /nologo /machine:x86 /dll '}|Select-Object -Last 1
if(!$compile -or !$link){throw 'Missing original callback or provider link'}
$source="$PSScriptRoot/wow_callback_abort_provider.c"
$diagnosticCompile=$compile -replace '/Fo\S+\s+\S+\.c$',('/Fo"'+$root+'/callback.obj" "'+$source+'"')
$diagnosticLink=$link.Replace('obj/wcall16.obj',('"'+$root+'/callback.obj"')).Replace('/map:wow32.dll.map',('/map:"'+$root+'/WOW32.map"')).Replace('/out:wow32.dll',('/out:"'+$root+'/WOW32.DLL" /implib:"'+$root+'/WOW32.lib"'))
if($compile -eq $diagnosticCompile -or $link -eq $diagnosticLink -or $diagnosticLink.Contains('obj/wcall16.obj') -or $diagnosticLink.Contains('/out:wow32.dll')){throw 'Diagnostic replacement failed'}
$normal=(Get-FileHash "$provider/wow32.dll").Hash
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
try{
 foreach($step in @(@('compile',$diagnosticCompile),@('link',$diagnosticLink))){
  $name=$step[0];$command=$step[1];$command|Set-Content "$root/$name-command.txt"
  $p=Start-Process cmd.exe -ArgumentList ('/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cd /d "'+$provider+'" && '+$command+'"') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
  $null=$p.Handle
  @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$command;deadline=$p.StartTime.AddSeconds(180).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name-process.json"
  if(!$p.WaitForExit(180000)){& taskkill.exe /PID $p.Id /T /F;throw 'Diagnostic owned deadline'}
  $p.WaitForExit();$p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit";if($p.ExitCode){throw "$name failed"}
 }
 if((Get-FileHash "$provider/wow32.dll").Hash -ne $normal){throw 'Normal provider changed'}
 $objects=@([regex]::Matches($link,'(?<!\S)obj/[^\s"]+\.(obj|res)')|ForEach-Object {Join-Path $provider $_.Value}|Select-Object -Unique)
 @($PSCommandPath,$source,"$repo/src/mvdm/wow32/wcall16.c","$provider/build.ninja")+$objects|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 @{kind='diagnostic-only';publishable=$false;target='SHLPROBE';fault='actual CallBack16 resolver Preparation/Returned';normalProvider=$normal;diagnosticProvider=(Get-FileHash "$root/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$root/diagnostic-manifest.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";1|Set-Content "$root/exit.txt";throw}
