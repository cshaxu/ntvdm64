$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$worker=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$root=Join-Path $repo 'build/t473-s10/shard-3-redir-map-r81'
$driver=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
if(Test-Path $root){throw 'Fresh output required'}
New-Item -ItemType Directory $root | Out-Null
$current=Get-FileHash @("$worker/VDMREDIR.dll","$repo/assets/release/VDMREDIR.DLL")
$lines=@(& ninja -C $worker -t commands VDMREDIR.dll)
if($LASTEXITCODE){throw 'Ninja command query failed'}
$commands=@($lines | Where-Object {$_ -match '^link\.exe /nologo /dll /entry:mvdm_vdmredir_dll_entry@12 '})
if($commands.Count -ne 1){throw 'Expected exactly one selected redirector link command'}
$command=$commands[0].Replace('/implib:VDMREDIR.dll.lib',('/implib:"'+$root+'/VDMREDIR.dll.lib"')).Replace('/out:VDMREDIR.dll',('/out:"'+$root+'/VDMREDIR.dll" /map:"'+$root+'/VDMREDIR.dll.map"'))
if($command -eq $commands[0]){throw 'Private link outputs were not substituted'}
$batch="@echo off`r`ncall `"$driver`" $command`r`nexit /b %errorlevel%`r`n"
$file=Join-Path $root 'link.cmd';[IO.File]::WriteAllText($file,$batch,[Text.Encoding]::ASCII)
$proc=Start-Process -FilePath $env:ComSpec -ArgumentList ('/d /s /c ""'+$file+'""') -WorkingDirectory $worker -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/link.stdout.txt" -RedirectStandardError "$root/link.stderr.txt"
$proc.WaitForExit();$proc.WaitForExit();$exit=$proc.ExitCode;$proc.Dispose()
if($exit -ne 0){throw "Private link failed: $exit"}
foreach($file in $current){if((Get-FileHash $file.Path).Hash -ne $file.Hash){throw 'Existing DLL changed during private link'}}
'PASS private DLL/map linked; existing product DLL hashes unchanged'
