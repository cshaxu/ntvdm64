$ErrorActionPreference='Stop'
$repo=(Resolve-Path '.').Path
$out="$repo/build/t473-s11/wow-notification-provider-pid"
$provider="$repo/build/.ninja/msvc-x86-release/wow32"
if(Test-Path $out){throw 'fresh output required'}
New-Item -ItemType Directory $out | Out-Null
$source="$repo/src/mvdm/wow32/wow32.c"
$text=[IO.File]::ReadAllText($source).Replace("`r`n","`n")
$entry="BOOLEAN`nW32DllInitialize("
$target='    UNREFERENCED_PARAMETER(Context);'
if(([regex]::Matches($text,[regex]::Escape($entry))).Count -ne 1 -or
   ([regex]::Matches($text,[regex]::Escape($target))).Count -ne 1){throw 'entry changed'}
$helper=@'
static void wow_fixture_notification(ULONG reason)
{
    char path[MAX_PATH], record[11]; DWORD size, written, id, i;
    HANDLE file;
    size = GetEnvironmentVariableA("MVDM_TEST_WOW_NOTIFICATION", path, sizeof(path));
    if (!size || size >= sizeof(path)) return;
    id = GetCurrentProcessId();
    for (i = 0; i < 8; ++i) record[i] = "0123456789ABCDEF"[(id >> (28 - i * 4)) & 15];
    record[8] = ':'; record[9] = (char)('0' + reason); record[10] = '\n';
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        WriteFile(file, record, sizeof(record), &written, NULL);
        CloseHandle(file);
    }
}

'@
$text=$text.Replace($entry,$helper+$entry).Replace($target,$target+"`n    wow_fixture_notification(Reason);")
[IO.File]::WriteAllText("$out/wow32.c",$text)
$driver="$provider/msvc-x86.cmd";$ninja=(Get-Command ninja.exe).Source
Push-Location $provider
try {
 $command=@(& $ninja -t commands obj/wow32.obj)[-1]
 if($LASTEXITCODE){throw 'compile command unavailable'}
 $command=$command.Replace($source.Replace('\','/'),"$out/wow32.c".Replace('\','/')).Replace('obj/wow32.obj',"$out/wow32.obj".Replace('\','/'))
 [IO.File]::WriteAllText("$out/compile.cmd","@echo off`r`ncall `"$driver`" $command`r`nexit /b %errorlevel%`r`n")
 & cmd.exe /d /c "$out/compile.cmd" *> "$out/compile.log"
 if($LASTEXITCODE){throw 'compile failed'}
 $link=@(& $ninja -t commands wow32.dll)[-1]
 if($LASTEXITCODE -or $link -notmatch '^link '){throw 'link command unavailable'}
 $link=$link.Replace('obj/wow32.obj',"$out/wow32.obj".Replace('\','/')).Replace('/map:wow32.dll.map',"/map:$out/diagnostic.map").Replace('/out:wow32.dll',"/out:$out/WOW32.DLL")+" /implib:$out/diagnostic.lib"
 [IO.File]::WriteAllText("$out/link.cmd","@echo off`r`ncall `"$driver`" $link`r`nexit /b %errorlevel%`r`n")
 & cmd.exe /d /c "$out/link.cmd" *> "$out/link.log"
 if($LASTEXITCODE){throw 'link failed'}
 Get-FileHash $source,$PSCommandPath,"$out/wow32.c","$out/WOW32.DLL"|Export-Csv -NoTypeInformation "$out/inputs.csv"
} finally {Pop-Location}
