$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$root=Join-Path $repo 'build/t473-s10/shard-0-cvidc-header-r92'
$worker=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$driver=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
if(Test-Path $root){throw 'Fresh output required'}
New-Item -ItemType Directory $root | Out-Null
$targets=@('obj/cvidc/ev_glue.obj','obj/cvidc/accessfn.obj','obj/video/v7_video.obj')
$before=Get-FileHash (@($targets | ForEach-Object { Join-Path $worker $_ })+@("$worker/ntvdm.exe"))
$commands=@(& ninja -C $worker -t commands @targets)
if($LASTEXITCODE -ne 0 -or $commands.Count -ne $targets.Count){throw 'Cannot obtain the three exact compile commands'}
foreach($target in $targets){
    $name=$target.Replace('/','_').Replace('.obj','')
    $matched=@($commands | Where-Object { $_ -match [regex]::Escape('/Fo'+$target)+'\b' })
    if($matched.Count -ne 1){throw "Ambiguous command: $target"}
    $command=$matched[0].Replace('/Fo'+$target,('/P /Fi"'+$root+'/'+$name+'.i"'))
    $batch="@echo off`r`ncall `"$driver`" $command`r`nexit /b %errorlevel%`r`n"
    $file=Join-Path $root "$name.cmd"
    [IO.File]::WriteAllText($file,$batch,[Text.Encoding]::ASCII)
    $p=Start-Process -FilePath $env:ComSpec -ArgumentList ('/d /s /c ""'+$file+'""') -WorkingDirectory $worker -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
    [void]$p.WaitForExit(60000); $p.WaitForExit(); $code=$p.ExitCode; $p.Dispose()
    if($code -ne 0){throw "Preprocessor failed: $target ($code)"}
}
foreach($input in $before){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw "Cached production input changed: $($input.Path)"}}
'PASS actual three compiler commands; production object and worker hashes unchanged'
