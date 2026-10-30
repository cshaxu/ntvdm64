param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$ProviderBuildRoot)
# Disposable copy of the selected original WOW32 object; no production change.
# Never change cached graphs, original sources, guest media or release files.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$out=[IO.Path]::GetFullPath($BuildRoot);$provider=(Resolve-Path $ProviderBuildRoot).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned diagnostic output required'}
$null=New-Item -ItemType Directory $out
$ninja=(Get-Command ninja.exe).Source
$driver="$provider/msvc-x86.cmd"
$replacements=@{}
Push-Location $provider
try{
 foreach($name in @('wow32')){
  $source="$repo/src/mvdm/wow32/wow32.c"
  $text=[IO.File]::ReadAllText($source)
  $text=$text.Replace("`r`n","`n")
  $needle='if (!CreateSmallHeap())'
  if(([regex]::Matches($text,[regex]::Escape($needle))).Count -ne 1){throw 'Original attach failure edge changed'}
  $entry="BOOLEAN`nW32DllInitialize("
  if(!$text.Contains($entry)){throw 'Original DLL initializer boundary changed'}
  $marker=@'
static BOOL wow_fixture_attach_failure(void)
{
    char path[MAX_PATH]; DWORD length, written; HANDLE file;
    length = GetEnvironmentVariableA("MVDM_TEST_WOW_ATTACH_MARKER", path, sizeof(path));
    if (length && length < sizeof(path)) {
        file = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            WriteFile(file, "attach-failure", 14, &written, NULL);
            CloseHandle(file);
        }
    }
    return TRUE;
}

'@
  $text=$text.Replace($entry,$marker+$entry)
  $text=$text.Replace($needle,'if (wow_fixture_attach_failure())')
  [IO.File]::WriteAllText("$out/$name.c",$text)
  $object="obj/wow32.obj"
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
 Get-FileHash $source,$PSCommandPath,"$out/WOW32.DLL","$out/wow32.c"|Export-Csv -NoTypeInformation "$out/inputs.csv"
 @{kind='diagnostic-only';publishable=$false;source='Original selected WOW32 copy with forced small-heap attach failure';guestChanged=$false}|ConvertTo-Json|Set-Content "$out/result.json"
}catch{$_|Out-String|Set-Content "$out/failure.txt";exit 1}finally{Pop-Location}
