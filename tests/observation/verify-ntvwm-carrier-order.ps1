param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerWrapper,[Parameter(Mandatory)][string]$FontInclude)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build root required'}
$null=New-Item -ItemType Directory $root
& $CompilerWrapper cl.exe /nologo /MT /O2 /GL /Gy /W4 "/I$repo/src" "/I$FontInclude" "/Fo$root\" "/Fe$root/order.exe" "$repo/tests/app/ntvwm_carrier_order_test.c" /link /LTCG /opt:ref user32.lib *> "$root/build.txt"
if($LASTEXITCODE){throw 'Order fixture build failed'}
$p=Start-Process "$root/order.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/result.txt" -RedirectStandardError "$root/stderr.txt"
$null=$p.Handle
@{pid=$p.Id;start=$p.StartTime;deadlineSeconds=20}|ConvertTo-Json|Set-Content "$root/owner.json"
if(!$p.WaitForExit(20000)){$p.Kill();throw 'Order fixture timeout'}
if($p.ExitCode -ne 0){throw "Order fixture failed $($p.ExitCode)"}
'0'|Set-Content "$root/exit.txt"
