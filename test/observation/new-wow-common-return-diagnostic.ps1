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
  $source="$repo/src/wow32-dll/source/$name.c"
  $text=[IO.File]::ReadAllText($source)
  if($name -eq 'wow_user_task_lifecycle'){
   $old='WOW_NATIVE_CALL(BOOL, DestroyWindow, (HWND w), (w), FALSE)'
   if(([regex]::Matches($text,[regex]::Escape($old))).Count -ne 1){throw 'Destroy boundary changed'}
   $new=@"
#define WOW_DIAG_DESTROY_STORAGE
#include "$header"
BOOL WINAPI wow_native_DestroyWindow(HWND w)
{
    wow_user_native_call call;
    BOOL result, restored;
    DWORD error = GetLastError();
    if (!wow_user_native_call_begin(&call, w)) return FALSE;
    SetLastError(error);
    __try { result = DestroyWindow(w); }
    __finally {
        error = GetLastError();
        restored = wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    wow_diag_destroy.raw = w;
    wow_diag_destroy.full = call.target_window;
    wow_diag_destroy.result = result;
    return restored ? result : FALSE;
}
"@
   $text=$text.Replace($old,$new)
  }else{
   $old='ULONG WINAPI wow_user_invoke_thunk(LPFNW32 thunk, PVDMFRAME frame)'
   if(([regex]::Matches($text,[regex]::Escape($old))).Count -ne 1){throw 'Thunk boundary changed'}
   $text=$text.Replace($old,"#define WOW_DIAG_FAILURE_REPORT`n#include `"$header`"`n"+$old)
   $old='    trace_thunk(frame, thunk);'
   if(([regex]::Matches($text,[regex]::Escape($old))).Count -ne 1){throw 'Thunk admission changed'}
   $text=$text.Replace($old,"    wow_diag_failure_report(thunk, frame);`n"+$old)
  }
  [IO.File]::WriteAllText("$out/$name.c",$text)
  $object="obj/wow32-dll/$name.obj"
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
 Get-FileHash "$repo/src/wow32-dll/source/wow_user_task_lifecycle.c","$repo/src/wow32-dll/source/wow_user_thunk_scope.c",$header,$PSCommandPath,"$out/WOW32.DLL","$out/wow_user_task_lifecycle.c","$out/wow_user_thunk_scope.c"|Export-Csv -NoTypeInformation "$out/inputs.csv"
 @{kind='diagnostic-only';publishable=$false;source='Two project-owned exact copies with failure-only observation';guestChanged=$false}|ConvertTo-Json|Set-Content "$out/result.json"
}catch{$_|Out-String|Set-Content "$out/failure.txt";exit 1}finally{Pop-Location}
