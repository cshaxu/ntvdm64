$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$provider=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32'
$out=Join-Path $repo 'build/t473-s10/candidate-validation/creation-negative-only'
if(Test-Path -LiteralPath $out){throw 'Fresh diagnostic output required'}
$null=New-Item -ItemType Directory $out
$ninja=(Get-Command ninja.exe -CommandType Application|Select-Object -First 1).Source
$driver=Join-Path $provider 'msvc-x86.cmd'
$header=Join-Path $out 'error_observation.h'
@'
#include <windows.h>
static DWORD wow_diag_failure(DWORD code, const char *source, unsigned line)
{
    DWORD saved = GetLastError();
    if (code == ERROR_INVALID_STATE) {
        char path[MAX_PATH], text[1024]; HANDLE file; DWORD length, written;
        length = GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH", path, sizeof(path));
        if (length && length < sizeof(path)) {
            file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ|FILE_SHARE_WRITE,
                NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (file != INVALID_HANDLE_VALUE) {
                length = (DWORD)wsprintfA(text,
                    "NegativeStateFailure pid=%lu tid=%lu source=%s line=%u\r\n",
                    GetCurrentProcessId(), GetCurrentThreadId(), source, line);
                (void)WriteFile(file, text, length, &written, NULL);
                CloseHandle(file);
            }
        }
    }
    SetLastError(saved);
    return code;
}
'@|Set-Content -LiteralPath $header
$normalHash=(Get-FileHash "$provider/wow32.dll").Hash
$objects=@{}
Push-Location $provider
try {
 foreach($name in @('wow_window_creation_binding','wow_window_words_binding','wow_user_object_bindings')){
  $source=Join-Path $repo ('src/wow32-dll/source/'+$name+'.c')
  $copy=Join-Path $out ($name+'.c')
  $text=[IO.File]::ReadAllText($source)
  $text=$text.Replace('SetLastError(ERROR_INVALID_STATE);','SetLastError(wow_diag_failure(ERROR_INVALID_STATE,__FILE__,__LINE__));')
  $text=[regex]::Replace($text,'\berror\s*=\s*ERROR_INVALID_STATE;','error=wow_diag_failure(ERROR_INVALID_STATE,__FILE__,__LINE__);')
  $text=$text.Replace('scope->failure = GetLastError();','scope->failure = wow_diag_failure(GetLastError(),__FILE__,__LINE__);')
  $includes=[regex]::Matches($text,'(?m)^#include[^\r\n]*\r?\n')
  if(!$includes.Count){throw 'Expected project-owned includes'}
  $at=$includes[-1].Index+$includes[-1].Length
  $text=$text.Insert($at,('#include "'+$header.Replace('\','/')+'"'+"`n"))
  [IO.File]::WriteAllText($copy,$text)
  $originalObject='obj/wow32-dll/'+$name+'.obj'
  $command=@(& $ninja -t commands $originalObject)[-1]
  if($LASTEXITCODE){throw 'Actual compiler command unavailable'}
  $newObject=(Join-Path $out ($name+'.obj')).Replace('\','/')
  $compile=$command.Replace($source.Replace('\','/'),$copy.Replace('\','/')).Replace($originalObject,$newObject)
  if($compile -eq $command -or $compile.Contains($originalObject)){throw 'Compile substitution failed'}
  $compile|Set-Content "$out/$name-command.txt"
  & cmd.exe /d /s /c ('call "'+$driver+'" '+$compile) *> "$out/$name-build.log"
  if($LASTEXITCODE){throw "Diagnostic compile failed: $name"}
  $objects[$originalObject]=$newObject
 }
 $link=@(& $ninja -t commands wow32.dll)[-1]
 if($LASTEXITCODE -or $link -notmatch '^link '){throw 'Actual DLL link unavailable'}
 foreach($object in $objects.Keys){$link=$link.Replace($object,$objects[$object])}
 $link=$link.Replace('/map:wow32.dll.map',('/map:'+($out+'/diagnostic.map').Replace('\','/'))).Replace('/out:wow32.dll',('/out:'+($out+'/WOW32.DLL').Replace('\','/')))
 $link+=' /implib:'+($out+'/diagnostic.lib').Replace('\','/')
 $link|Set-Content "$out/link-command.txt"
 & cmd.exe /d /s /c ('call "'+$driver+'" '+$link) *> "$out/link.log"
 if($LASTEXITCODE){throw 'Diagnostic link failed'}
 if((Get-FileHash "$provider/wow32.dll").Hash -ne $normalHash){throw 'Production provider changed'}
 Get-FileHash $header,$PSCommandPath,"$out/WOW32.DLL","$provider/build.ninja"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/inputs.json"
 @{kind='diagnostic-only';publishable=$false;guestChanged=$false;productionProviderChanged=$false;normalProviderSha256=$normalHash;observation='Observe only explicit creation failure branches; preserve arguments, return and last error; normal SetLastError success paths are unchanged'}|ConvertTo-Json|Set-Content "$out/result.json"
} finally {Pop-Location}
