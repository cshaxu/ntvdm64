param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$ProviderBuildRoot)
# Generated project-owned source copies and two substituted objects only.
# Never change cached graphs, original sources, guest media or release files.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot);$provider=(Resolve-Path $ProviderBuildRoot).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned diagnostic output required'}
$null=New-Item -ItemType Directory $out
$ninja=(Get-Command ninja.exe).Source
$header=(Resolve-Path "$PSScriptRoot/wow_common_return_diagnostic.h").Path.Replace('\','/')
$driver="$provider/msvc-x86.cmd"
$replacements=@{}
Push-Location $provider
try{
 foreach($name in @('wow_user_task_lifecycle','wow_user_thunk_scope')){
  $source="$repo/src/compat/$name.c"
  $text=[IO.File]::ReadAllText($source)
  if($name -eq 'wow_user_task_lifecycle'){
   $pattern='(?m)^#define WOW_NATIVE_MAPPED_DESTROY_WINDOW[^\r\n]*(?:\r?\n[^\r\n]*\\)*\r?\n\}'
   $matches=[regex]::Matches($text,$pattern)
   if($matches.Count -ne 1){throw 'Destroy boundary changed'}
   $body=$matches[0].Value
   $old='    return restored ? result : failed; \'
   if(([regex]::Matches($body,[regex]::Escape($old))).Count -ne 1){throw 'Destroy return changed'}
   $trace=@(
      '    wow_diag_destroy.raw = w; \',
      '    wow_diag_destroy.full = native; \',
      '    wow_diag_destroy.result = result; \',
      $old) -join "`n"
   $text=$text.Replace($body,"#define WOW_DIAG_DESTROY_STORAGE`n#include `"$header`"`n"+$body.Replace($old,$trace))
  }else{
   $old='ULONG WINAPI wow_user_invoke_thunk(LPFNW32 thunk, PVDMFRAME frame)'
   if(([regex]::Matches($text,[regex]::Escape($old))).Count -ne 1){throw 'Thunk boundary changed'}
   $text=$text.Replace($old,"#include <wowkrn.h>`n#include `"wkfileio.h`"`n#define WOW_DIAG_FAILURE_REPORT`n#include `"$header`"`n"+$old)
   $old='    trace_thunk(frame, thunk);'
   if(([regex]::Matches($text,[regex]::Escape($old))).Count -ne 1){throw 'Thunk admission changed'}
   $text=$text.Replace($old,"    wow_diag_failure_report(thunk, frame);`n"+$old)
  }
  [IO.File]::WriteAllText("$out/$name.c",$text)
  $object="obj/compat/$name.obj"
  $command=@(& $ninja -t commands $object)[-1]
  if($LASTEXITCODE){throw 'Compile command unavailable'}
  $command=$command.Replace($source.Replace('\','/'),"$out/$name.c".Replace('\','/')).Replace($object,"$out/$name.obj".Replace('\','/'))
  $script="@echo off`r`ncall `"$driver`" $command`r`nexit /b %errorlevel%`r`n"
  [IO.File]::WriteAllText("$out/$name.cmd",$script)
  & cmd.exe /d /c "$out/$name.cmd" *> "$out/$name.build.log"
  if($LASTEXITCODE){throw "Diagnostic compile failed: $name"}
  $replacements[$object]="$out/$name.obj".Replace('\','/')
 }
 $link=@(& $ninja -t commands wow32.dll)[-1]
 if($LASTEXITCODE -or $link -notmatch '^link '){throw 'Actual provider link command unavailable'}
 foreach($name in $replacements.Keys){$link=$link.Replace($name,$replacements[$name])}
 $link=$link.Replace('/map:wow32.dll.map',"/map:$out/diagnostic.map").Replace('/out:wow32.dll',"/out:$out/WOW32.DLL")
 $link+=" /implib:$out/diagnostic.lib"
 [IO.File]::WriteAllText("$out/link.cmd","@echo off`r`ncall `"$driver`" $link`r`nexit /b %errorlevel%`r`n")
 & cmd.exe /d /c "$out/link.cmd" *> "$out/link.log"
 if($LASTEXITCODE){throw 'Diagnostic link failed'}
 Get-FileHash "$repo/src/compat/wow_user_task_lifecycle.c","$repo/src/compat/wow_user_thunk_scope.c",$header,$PSCommandPath,"$out/WOW32.DLL","$out/wow_user_task_lifecycle.c","$out/wow_user_thunk_scope.c"|Export-Csv -NoTypeInformation "$out/inputs.csv"
 @{kind='diagnostic-only';publishable=$false;source='Two project-owned exact copies with failure-only observation';guestChanged=$false}|ConvertTo-Json|Set-Content "$out/result.json"
}catch{$_|Out-String|Set-Content "$out/failure.txt";exit 1}finally{Pop-Location}
