[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
[IO.File]::WriteAllBytes("$root/input.bin",[byte[]](0,13,10,255,128,65))
$before=(Get-FileHash "$root/input.bin").Hash
$tool="$repo/build/.ninja/build-tools/prepare-test-inputs.cmd"
function Case($name,$target,$source,$leaf,$success){
 $line='/d /s /c ""'+$tool+'" "'+$target+'" "'+$source+'" "'+$leaf+'""'
 $p=Start-Process $env:ComSpec -ArgumentList $line -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit(30000)){taskkill.exe /PID $p.Id /T /F;throw 'Preparation timeout'}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt"
 if(($code -eq 0) -ne $success){throw "Unexpected $name result $code"}
}
Case copy "$root/out" "$root/input.bin" 'renamed.bin' $true
if((Get-FileHash "$root/out/renamed.bin").Hash -ne $before){throw 'Binary copy changed'}
Case duplicate "$root/out" "$root/input.bin" 'renamed.bin' $false
Case escape "$root/out" "$root/input.bin" '../escaped.bin' $false
Case missing "$root/out" "$root/absent.bin" 'missing.bin' $false
Case directory "$root/out" "$root/out" 'directory.bin' $false
Case outside $repo "$root/input.bin" 'unowned.bin' $false
if((Test-Path "$root/escaped.bin") -or (Get-FileHash "$root/input.bin").Hash -ne $before -or (Get-FileHash "$root/out/renamed.bin").Hash -ne $before){throw 'Failure changed source or existing output'}
'PASS binary rename, no overwrite, no traversal, missing/directory/outside output rejected'|Set-Content "$root/result.txt"
0|Set-Content "$root/exit.txt"
