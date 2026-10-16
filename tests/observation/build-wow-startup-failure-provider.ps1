param(
 [Parameter(Mandatory)][string]$ProviderBuildRoot,
 [Parameter(Mandatory)][string]$BuildRoot,
 [switch]$ShellException,
 [string]$Ninja='ninja.exe',
 [string]$VsDevCmd='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
# Invoke only as a recorded background job after the normal provider build.
# This generates a diagnostic DLL, not an accepted or publishable package.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$provider=(Resolve-Path $ProviderBuildRoot).Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build-owned output'}
$null=New-Item -ItemType Directory $out
try {
 $commands=@(& $Ninja -C $provider -t commands wow32.dll)
 if($LASTEXITCODE){throw 'Cannot inspect selected provider graph'}
 $pattern=if($ShellException){'/wow_shell_callback\.c$'}else{'/kernel/queue\.c$'}
 $compile=$commands|Where-Object {$_ -match $pattern}|Select-Object -Last 1
 $link=$commands|Where-Object {$_ -match '^link /nologo /machine:x86 /dll '}|Select-Object -Last 1
 if(!$compile -or !$link){throw 'Missing original queue or DLL link command'}
 $source=if($ShellException){"$PSScriptRoot/wow_shell_exception_provider.c"}else{"$PSScriptRoot/wow_startup_failure_provider.c"}
 $originalObject=if($ShellException){'obj/wow32-dll/wow_shell_callback.obj'}else{'obj/opennt-user/queue.obj'}
 $compileDiagnostic=$compile -replace '/Fo\S+\s+\S+\.c$', ('/Fo"'+$out+'/diagnostic.obj" "'+$source+'"')
 $linkDiagnostic=$link.Replace($originalObject,('"'+$out+'/diagnostic.obj"')).Replace('/map:wow32.dll.map',('/map:"'+$out+'/WOW32.map"')).Replace('/out:wow32.dll',('/out:"'+$out+'/WOW32.DLL" /implib:"'+$out+'/WOW32.lib"'))
 if($compileDiagnostic -eq $compile -or $linkDiagnostic -eq $link -or $linkDiagnostic.Contains($originalObject) -or $linkDiagnostic.Contains('/out:wow32.dll')){throw 'Diagnostic substitution failed'}
 $normalHash=(Get-FileHash "$provider/wow32.dll").Hash
 foreach($step in @(@('compile',$compileDiagnostic),@('link',$linkDiagnostic))){
  $name=$step[0];$command=$step[1]
  $command|Set-Content "$out/$name-command.txt"
  & cmd.exe /d /s /c ('call "'+$VsDevCmd+'" -arch=x86 -host_arch=x64 >nul && cd /d "'+$provider+'" && '+$command) *> "$out/$name.log"
  if($LASTEXITCODE){throw "$name failed"}
 }
 if((Get-FileHash "$provider/wow32.dll").Hash -ne $normalHash){throw 'Normal provider was modified'}
 if(!(Test-Path "$out/WOW32.DLL")){throw 'No diagnostic DLL'}
 $objects=@([regex]::Matches($link,'(?<!\S)obj/[^\s"]+\.(obj|res)')|ForEach-Object {Join-Path $provider $_.Value}|Select-Object -Unique)
 @($source,"$repo/src/opennt-host/windows/core/ntuser/kernel/queue.c","$repo/src/wow32-dll/source/wow_shell_callback.c","$provider/build.ninja","$provider/wow32.dll","$out/WOW32.DLL")+$objects|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/inputs.json"
 $target=if($ShellException){'SHLPROBE'}else{'GENPROBE'}
 $fault=if($ShellException){'exception after original Shell callback return'}else{'second original InitTask allocation'}
 @{kind='diagnostic-only';publishable=$false;target=$target;fault=$fault;normalProvider=$normalHash;diagnosticProvider=(Get-FileHash "$out/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$out/diagnostic-manifest.json"
 '0'|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/error.txt";'1'|Set-Content "$out/exit.txt";throw}
