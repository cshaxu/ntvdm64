param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerWrapper,[Parameter(Mandatory)][string]$FontInclude)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build root required'}
$null=New-Item -ItemType Directory $root
& $CompilerWrapper cl.exe /nologo /MT /O2 /GL /Gy /W4 "/I$repo/src" "/I$FontInclude" "/Fo$root\" "/Fe$root/close.exe" "$repo/tests/app/ntvwm_close_priority_test.c" "$repo/src/worker-base/shutdown_close.c" /link /LTCG /opt:ref user32.lib *> "$root/build.txt"
if($LASTEXITCODE){throw 'Close-priority fixture build failed'}
$p=Start-Process "$root/close.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/result.txt" -RedirectStandardError "$root/stderr.txt"
$null=$p.Handle
@{pid=$p.Id;start=$p.StartTime;deadlineSeconds=60}|ConvertTo-Json|Set-Content "$root/owner.json"
if(!$p.WaitForExit(60000)){$p.Kill();$p.WaitForExit();throw 'Close-priority fixture timeout'}
if($p.ExitCode){throw "Close-priority fixture failed $($p.ExitCode)"}
@{passed=$true;pid=$PID;utc=[DateTime]::UtcNow}|ConvertTo-Json|Set-Content "$root/result.json"
