# Research-only comparison: original source is read-only and never a product link input.
$ErrorActionPreference='Stop'
Set-Location (Resolve-Path "$PSScriptRoot/../../..")
$root=$PWD.Path;$out="$root/build/t478-s8";$cache="$root/build/.ninja/msvc-x86-release/worker"
$null=New-Item -ItemType Directory $out -Force
$started=Get-Date
$wrapper="$out/compiler.cmd"
@('@echo off','call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul','if errorlevel 1 exit /b %errorlevel%','%*') | Set-Content $wrapper -Encoding ASCII
$g=Get-Content "$cache/build.ninja" -Raw
$m=[regex]::Match($g,'(?m)^build obj/suballoc/suballoc.obj:.*\r?\n\s+cflags = (.+)$')
if(!$m.Success){$m=[regex]::Match($g,'(?m)^cflags = (.+)$')}
if(!$m.Success){throw 'Production suballocator flags absent'}
$flags=$m.Groups[1].Value.Replace('$:',':')+' /O2'
$sources=@(@('current',"$root/src/mvdm-host/suballoc/suballoc.c"),@('opennt','O:/repos.external/OpenNT/base/mvdm/suballoc/suballoc.c'),@('opennt45','O:/repos.external/OpenNT-4.5/nt/private/mvdm/suballoc/suballoc.c'))
foreach($pair in $sources){
 $name=$pair[0];$source=$pair[1]
 ($flags+' /EP "'+$source+'"') | Set-Content "$out/$name.rsp" -Encoding ASCII
 & $wrapper cl "@$out/$name.rsp" > "$out/$name.i" 2> "$out/$name.err"
 if($LASTEXITCODE){throw "Preprocess failed: $name"}
}
($flags+' /Fo"'+$out+'/fixture.obj" "'+$root+'/tests/mvdm/suballoc/suballoc_lifecycle_fixture.c"') | Set-Content "$out/fixture.rsp" -Encoding ASCII
& $wrapper cl "@$out/fixture.rsp" > "$out/compile.txt" 2>&1
if($LASTEXITCODE){throw 'Fixture compile failed'}
& $wrapper link /nologo "/OUT:$out/suballoc-fixture.exe" "/MAP:$out/suballoc-fixture.map" "$out/fixture.obj" "$cache/original-mvdm-host-suballoc.lib" kernel32.lib ntdll.lib legacy_stdio_definitions.lib > "$out/link.txt" 2>&1
if($LASTEXITCODE){throw 'Fixture link failed'}
foreach($case in @('normal','failure-audit')){
 $args=if($case -eq 'normal'){@()}else{@('--failure-audit')}
 $psi=[Diagnostics.ProcessStartInfo]::new("$out/suballoc-fixture.exe")
 $psi.UseShellExecute=$false;$psi.CreateNoWindow=$true;$psi.RedirectStandardOutput=$true;$psi.RedirectStandardError=$true
 foreach($arg in $args){$psi.ArgumentList.Add($arg)}
 $p=[Diagnostics.Process]::Start($psi);$stdout=$p.StandardOutput.ReadToEndAsync();$stderr=$p.StandardError.ReadToEndAsync()
 if(!$p.WaitForExit(10000)){$p.Kill();throw 'Fixture timeout'}
 $code=$p.ExitCode;($stdout.Result+$stderr.Result) | Set-Content "$out/$case.txt"
 if($code){throw "Fixture $case failed: $code"}
 $p.Dispose()
}
@{pid=$PID;started=$started.ToString('o');ended=(Get-Date).ToString('o');exit_code=0} | ConvertTo-Json | Set-Content "$out/run.json"
Get-Content "$out/normal.txt","$out/failure-audit.txt"
