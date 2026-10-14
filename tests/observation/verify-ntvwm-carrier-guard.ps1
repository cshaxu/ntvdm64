param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerWrapper)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
try {
 & $CompilerWrapper cl.exe /nologo /MT /O2 /W4 /WX /D_WIN32_WINNT=0x0A00 "/I$repo/src" "/Fo$root\" "/Fe$root/carrier-guard-test.exe" "$repo/tests/observation/ntvwm_carrier_guard_test.c" "$repo/src/ntvwm-exe/console_state.c" "$repo/src/common/console/members.c" /link user32.lib *> "$root/build.txt"
 if($LASTEXITCODE){throw 'Guard fixture build failed'}
 $p=Start-Process "$root/carrier-guard-test.exe" -ArgumentList ('"'+$root+'\result.txt"') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/stdout.txt" -RedirectStandardError "$root/stderr.txt"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime;deadlineSeconds=40}|ConvertTo-Json|Set-Content "$root/owner.json"
 if(!$p.WaitForExit(40000)){$p.Kill($true);throw 'Guard timeout'}
 if($p.ExitCode -ne 0){throw "Guard failed $($p.ExitCode)"}
 if(!(Select-String -Path "$root/result.txt" -SimpleMatch 'PASS exact HWND' -Quiet)){throw 'Missing guard assertions'}
 '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit.txt";throw}
