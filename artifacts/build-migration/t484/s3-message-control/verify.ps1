$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$start=[DateTime]::UtcNow.ToString('o')
try {
$root=$PSScriptRoot+'/retry'
New-Item -ItemType Directory -Force $root|Out-Null
$provider=(Resolve-Path build/.ninja/msvc-x86-release/wow32).Path
$bad="$root/rejected"
New-Item -ItemType Directory -Force $bad|Out-Null
New-Item -ItemType File -Force "$bad/.message-control-owned"|Out-Null
& cmd.exe /d /c tools\build-wow-message-control.cmd $provider $bad
if($LASTEXITCODE -ne 2){throw 'Historical identity gate failed'}
if((Get-Content "$bad/normal.sha256" -Raw).Trim() -ne (Get-FileHash "$provider/wow32.dll").Hash){throw 'Native hash did not run'}
if(Test-Path "$bad/compile.cmd"){throw 'Rejected control prepared compilation'}
$cmd=Get-Content tools/build-wow-message-control.cmd -Raw
$a=$cmd.IndexOf('setlocal EnableDelayedExpansion');$b=$cmd.IndexOf('endlocal',$a)
if($a -lt 0 -or $b -lt $a){throw 'Cannot inspect native command replacement'}
$compile='cl /nologo /c /TC /O2 /DWIN32 /Foobj/wmdisp32.obj O:/repo/src/mvdm-host/wow32/wmdisp32.c'
$link='link /nologo /machine:x86 /dll obj/a.obj obj/wmdisp32.obj obj/b.res /map:wow32.dll.map /out:wow32.dll kernel32.lib'
$dir="$root/parser space";New-Item -ItemType Directory -Force $dir|Out-Null
$lines=@('@echo off','setlocal EnableExtensions DisableDelayedExpansion','set "NTVDM_ROOT=O:\repos.hobby\ntvdm64"',('set "output='+$dir+'"'),('set "compile='+$compile+'"'),('set "link='+$link+'"'),$cmd.Substring($a,$b-$a),'endlocal')
[IO.File]::WriteAllText("$root/parser.cmd",($lines -join "`r`n")+"`r`n")
& cmd.exe /d /c "$root/parser.cmd"
if($LASTEXITCODE){throw 'Native command replacement failed'}
$expected=$compile -replace '/Fo\S+\s+\S+\.c$',('/I"'+$dir+'" /Fo"'+$dir+'\message.obj" "O:\repos.hobby\ntvdm64\test\observation\wow_message_transaction_provider.c"')
if((Get-Content "$dir/compile.cmd" -Raw).Trim() -ne '@'+$expected){throw 'Compile command differs from historical replacement'}
$expected=$link.Replace('obj/wmdisp32.obj',('"'+$dir+'\message.obj"')).Replace('/map:wow32.dll.map',('/map:"'+$dir+'\WOW32.map"')).Replace('/out:wow32.dll',('/out:"'+$dir+'\WOW32.DLL" /implib:"'+$dir+'\WOW32.lib"'))
if((Get-Content "$dir/link.cmd" -Raw).Trim() -ne '@'+$expected){throw 'Link command differs from historical replacement'}
& test/observation/build-wow-message-transaction-provider.ps1 -ProviderBuildRoot $provider -BuildRoot "$root/current"
if($LASTEXITCODE){throw 'Current producer failed'}
$source=Get-Content build/t484-s3-addon/deliver.ps1 -Raw
$a=$source.IndexOf(' $rows=Get-Content');$b=$source.IndexOf(' $args=@{}')
& ([scriptblock]::Create($source.Substring($a,$b-$a)))
@{pid=$PID;start=$start;finished=[DateTime]::UtcNow.ToString('o');exit=0;currentProducer='pass';wrongProvider='rejected';commandReplacement='matches old recipe, path with space';historicalFullBuild='not run: exact object closure absent'}|ConvertTo-Json|Set-Content "$root/result.json"
} catch {
@{pid=$PID;start=$start;exit=1;error=$_.ToString()}|ConvertTo-Json|Set-Content "$PSScriptRoot/result.json"
throw
}