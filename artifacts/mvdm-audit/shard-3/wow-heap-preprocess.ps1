$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64';$root="$repo\build\t473-s10\shard-3-wowheap-r82"
$worker="$repo\build\.ninja\msvc-x86-release\wow32";$driver="$worker\msvc-x86.cmd"
if(Test-Path $root){throw 'Fresh output required'}
New-Item -ItemType Directory $root|Out-Null
$before=Get-FileHash @("$worker/obj/wheap.obj","$worker/wow32.dll","$repo/assets/release/WOW32.DLL")
$commands=@(& ninja -C $worker -t commands obj/wheap.obj)
if($LASTEXITCODE){throw 'Ninja command query failed'}
$selected=@($commands|Where-Object {$_ -match '/Foobj/wheap\.obj\b'})
if($selected.Count -ne 1){throw 'Expected one exact heap compile command'}
$command=$selected[0].Replace('/Foobj/wheap.obj',('/P /Fi"'+$root+'/wheap.i"'))
$batch="@echo off`r`ncall `"$driver`" $command`r`nexit /b %errorlevel%`r`n"
$file=Join-Path $root 'wheap.cmd';[IO.File]::WriteAllText($file,$batch,[Text.Encoding]::ASCII)
$proc=Start-Process -FilePath $env:ComSpec -ArgumentList ('/d /s /c ""'+$file+'""') -WorkingDirectory $worker -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/wheap.stdout.txt" -RedirectStandardError "$root/wheap.stderr.txt"
$proc.WaitForExit();$proc.WaitForExit();$code=$proc.ExitCode;$proc.Dispose()
if($code -ne 0){throw "Preprocess failed: $code"}
foreach($r in $before){if((Get-FileHash $r.Path).Hash -ne $r.Hash){throw 'A cached or released image changed'}}
'PASS WOW heap preprocessed with selected flags; cached and released hashes unchanged'
