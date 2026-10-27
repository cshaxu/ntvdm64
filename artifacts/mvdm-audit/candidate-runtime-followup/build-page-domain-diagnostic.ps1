$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$provider=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$out=Join-Path $repo 'build/t473-s10/candidate-validation/page-domain-diagnostic'
if(Test-Path -LiteralPath $out){throw 'Fresh comparison output required'}
$null=New-Item -ItemType Directory $out
$ninja=(Get-Command ninja.exe -CommandType Application|Select-Object -First 1).Source
$driver=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
$source=Join-Path $repo 'src/ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c'
$text=[IO.File]::ReadAllText($source)
$start=$text.IndexOf('static int page_domain_publish_handle(')
$end=$text.IndexOf('int mvdm_softpc_wow_page_domain_publish_handle(', $start)
if($start -lt 0 -or $end -lt $start){throw 'Expected complete publish function'}
$function=$text.Substring($start,$end-$start)
$rejects=[regex]::Matches($function,'return 0;').Count
if($rejects -lt 10){throw 'Expected known rejection branches'}
$helper=@"
#include <stdio.h>
static int page_domain_diagnostic_rejection(USHORT index, ULONG window,
    ULONG cls, unsigned source_line)
{
    DWORD saved=GetLastError(), length, written; HANDLE file;
    char path[MAX_PATH], line[1024]; int count;
    length=GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH",path,sizeof(path));
    if(length && length<sizeof(path)) {
        file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(file!=INVALID_HANDLE_VALUE) {
            count=sprintf_s(line,sizeof(line),"PagePublishReject line=%u tid=%lu index=%04X window=%08lX class=%08lX active=%d owner=%p current=%p delta=%08lX cr0=%08lX cr3=%08lX directory=%08lX menu=%08lX wnd=%08lX desktop=%08lX cpl=%lu\r\n",
                source_line,GetCurrentThreadId(),index,window,cls,domain.active,
                domain.owner,session_thread_current(),domain.client_delta,
                c_getCR0(),c_getCR3(),domain.directory,
                index && index<0xffffu ? domain.guest_menus[index]:0u,
                index && index<0xffffu ? domain.guest_windows[index]:0u,
                domain.desktop_handle,c_getCPL());
            if(count>0)(void)WriteFile(file,line,(DWORD)count,&written,NULL);
            CloseHandle(file);
        }
    }
    SetLastError(saved);return 0;
}

"@
$observed=$function.Replace('return 0;','return page_domain_diagnostic_rejection(index, window, class_server, __LINE__);')
$comparison=$text.Substring(0,$start)+$helper+$observed+$text.Substring($end)
$copy=Join-Path $out 'mvdm_softpc_wow_page_domain.c'
[IO.File]::WriteAllText($copy,$comparison)
$normal=@('obj/adapter-softpc/mvdm_softpc_wow_page_domain.obj','softpc-bindings.lib','ntvdm.exe','ntvdm.lib')|ForEach-Object{Get-FileHash (Join-Path $provider $_)}
$normal|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/normal-inputs.json"
Push-Location $provider
try {
 $compile=@(& $ninja -t commands obj/adapter-softpc/mvdm_softpc_wow_page_domain.obj)[-1]
 if($LASTEXITCODE){throw 'Actual DPMI compiler command unavailable'}
 $compile=$compile.Replace($source.Replace('\','/'),$copy.Replace('\','/')).Replace('obj/adapter-softpc/mvdm_softpc_wow_page_domain.obj',($out+'/modesw.obj').Replace('\','/'))
 if($compile.Contains('obj/adapter-softpc/mvdm_softpc_wow_page_domain.obj') -or !$compile.Contains($copy.Replace('\','/'))){throw 'Compile substitution failed'}
 $library=@(& $ninja -t commands softpc-bindings.lib)[-1]
 if($LASTEXITCODE -or $library -notmatch '^lib(?:\.exe)? '){throw 'Actual DPMI library command unavailable'}
 $library=$library.Replace('obj/adapter-softpc/mvdm_softpc_wow_page_domain.obj',($out+'/modesw.obj').Replace('\','/')).Replace('/out:softpc-bindings.lib',('/out:'+($out+'/dpmi-comparison.lib').Replace('\','/')))
 if($library.Contains('/out:softpc-bindings.lib')){throw 'Library output substitution failed'}
 $link=@(& $ninja -t commands ntvdm.exe)[-1]
 if($LASTEXITCODE -or $link -notmatch '^(?:cmd\.exe /d /s /c ")?link(?:\.exe)? ' -or !$link.Contains('softpc-bindings.lib')){throw 'Actual worker link unavailable'}
 $link=$link.Replace('softpc-bindings.lib',($out+'/dpmi-comparison.lib').Replace('\','/')).Replace('/out:ntvdm.exe',('/out:'+($out+'/ntvdm.exe').Replace('\','/')))
 $link=$link.Replace('ntvdm.exe.map',($out+'/ntvdm.exe.map').Replace('\','/'))
 $link=[regex]::Replace($link,'(?i)/implib:\S+',('/implib:'+($out+'/comparison.lib').Replace('\','/')))
 if($link.Contains('/out:ntvdm.exe') -or !$link.Contains(($out+'/ntvdm.exe').Replace('\','/'))){throw 'Worker output substitution failed'}
 foreach($step in @(@('compile',$compile),@('library',$library),@('link',$link))){
  $step[1]|Set-Content "$out/$($step[0])-command.txt"
  & cmd.exe /d /s /c ('call "'+$driver+'" '+$step[1]) *> "$out/$($step[0]).log"
  if($LASTEXITCODE){throw "Comparison $($step[0]) failed"}
 }
 foreach($record in $normal){if((Get-FileHash $record.Path).Hash -ne $record.Hash){throw 'Normal build cache changed'}}
 Get-FileHash $source,$copy,$PSCommandPath,"$out/ntvdm.exe","$provider/build.ninja"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/inputs.json"
 @{kind='diagnostic-only';publishable=$false;guestChanged=$false;productionCacheChanged=$false;variable='Only rejection paths of page_domain_publish_handle log source line and current host state; all state and returns preserved';comparisonTo='current source'}|ConvertTo-Json|Set-Content "$out/result.json"
} finally {Pop-Location}
