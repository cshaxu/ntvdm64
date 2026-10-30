$ErrorActionPreference='Stop'
$repo=(Resolve-Path '.').Path
$out="$repo/build/t473-s11/wow-cleanup-provider"
$provider="$repo/build/.ninja/msvc-x86-release/wow32"
if(Test-Path $out){throw 'fresh output required'}
New-Item -ItemType Directory $out | Out-Null
$helper=@'
static void wow_fixture_cleanup(char kind, BOOL state)
{
    char path[MAX_PATH], record[13]; DWORD size, written, id, i; HANDLE file;
    size = GetEnvironmentVariableA("MVDM_TEST_WOW_NOTIFICATION", path, sizeof(path));
    if (!size || size >= sizeof(path)) return;
    id = GetCurrentProcessId();
    for (i = 0; i < 8; ++i) record[i] = "0123456789ABCDEF"[(id >> (28 - i * 4)) & 15];
    record[8] = ':'; record[9] = kind; record[10] = ':';
    record[11] = state ? '1' : '0'; record[12] = '\n';
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) { WriteFile(file, record, sizeof(record), &written, NULL); CloseHandle(file); }
}
'@
$items=@(
 @{name='wow_bitmap_bindings'; file='wow_bitmap_bindings.c'; object='obj/wow32-dll/wow_bitmap_bindings.obj';
   begin='    assert(wow_bitmap_current == NULL);'; first="    wow_fixture_cleanup('B', lock_initialized != 0);";
   end='        DeleteCriticalSection(&wow_bitmap_lock);'; last="`n    wow_fixture_cleanup('b', lock_initialized != 0);"},
 @{name='wow_registry_binding'; file='wow_registry_binding.c'; object='obj/wow32-dll/wow_registry_binding.obj';
   begin='    if(!wow_registry_ready())return;'; first="    wow_fixture_cleanup('R', wow_registry_tokens != NULL);";
   end="        entry=next;`n    }"; last="`n    wow_fixture_cleanup('r', wow_registry_tokens != NULL);"}
)
$ninja=(Get-Command ninja.exe).Source;$driver="$provider/msvc-x86.cmd";$replacements=@{}
Push-Location $provider
try {
 foreach($item in $items){
  $source="$repo/src/wow32-dll/source/$($item.file)"
  $text=[IO.File]::ReadAllText($source).Replace("`r`n","`n")
  $includeEnd=$text.IndexOf("`n`n")
  if($includeEnd -lt 0){throw 'header boundary unavailable'}
  $text=$text.Insert($includeEnd+2,$helper+"`n")
  if(([regex]::Matches($text,[regex]::Escape($item.begin))).Count -ne 1 -or
     ([regex]::Matches($text,[regex]::Escape($item.end))).Count -ne 1){throw 'cleanup insertion boundary changed'}
  $text=$text.Replace($item.begin,$item.first+"`n"+$item.begin).Replace($item.end,$item.end+$item.last)
  [IO.File]::WriteAllText("$out/$($item.file)",$text)
  $cmd=@(& $ninja -t commands $item.object)[-1]
  if($LASTEXITCODE){throw 'compile command unavailable'}
  $cmd=$cmd.Replace($source.Replace('\','/'),"$out/$($item.file)".Replace('\','/')).Replace($item.object,"$out/$($item.name).obj".Replace('\','/'))
  [IO.File]::WriteAllText("$out/$($item.name).cmd","@echo off`r`ncall `"$driver`" $cmd`r`nexit /b %errorlevel%`r`n")
  & cmd.exe /d /c "$out/$($item.name).cmd" *> "$out/$($item.name).log"
  if($LASTEXITCODE){throw "compile failed $($item.name)"}
  $replacements[$item.object]="$out/$($item.name).obj".Replace('\','/')
 }
 $old="$repo/build/t473-s11/wow-missing-export-provider-reserved"
 $link=Get-Content "$old/link.cmd" -Raw
 foreach($name in $replacements.Keys){$link=$link.Replace($name,$replacements[$name])}
 $link=$link.Replace("$old/diagnostic.map","$out/diagnostic.map").Replace("$old/WOW32.DLL","$out/WOW32.DLL").Replace("$old/diagnostic.lib","$out/diagnostic.lib")
 [IO.File]::WriteAllText("$out/link.cmd",$link)
 & cmd.exe /d /c "$out/link.cmd" *> "$out/link.log"
 if($LASTEXITCODE){throw 'link failed'}
 Get-FileHash "$out/WOW32.DLL","$out/wow_bitmap_bindings.c","$out/wow_registry_binding.c",$PSCommandPath|Export-Csv -NoTypeInformation "$out/inputs.csv"
} finally {Pop-Location}
