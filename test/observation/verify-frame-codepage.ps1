param([Parameter(Mandatory)][string]$EvidenceRoot)
$ErrorActionPreference='Stop'
Set-Location (Resolve-Path "$PSScriptRoot/../..")
$root=[IO.Path]::GetFullPath($EvidenceRoot,(Get-Location).Path)
if(!$root.StartsWith("$((Get-Location).Path)\build\",[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw "Fresh build-owned evidence directory required"}
New-Item -ItemType Directory $root|Out-Null
$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
$observer="$PWD/build/.ninja/fixtures-x64/console-startup-observer.exe"
$results=@()
foreach($arch in @('x86','x64')){
 $component=if($arch -eq 'x86'){'worker'}else{'frontend'}
 $build="$PWD/build/.ninja/msvc-$arch-release/$component"
 $exe="$build/frontend-frame-codepage-test.exe"
 $log="$root/$arch-observer.txt";$report="$root/$arch-frame.txt"
 $args='"'+$exe+'" "'+$build+'" "'+$log+'" --observation-timeout-ms 30000 "'+$report+'"'
 $p=Start-Process $observer -ArgumentList $args -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$arch-run.stdout" -RedirectStandardError "$root/$arch-run.stderr"
 if(!$p.WaitForExit(60000)){& taskkill.exe /PID $p.Id /T /F;throw 'Frame observer timeout'}
 $p.WaitForExit();$text=Get-Content $report -Raw
 $ok=$p.ExitCode -eq 0 -and $text -match 'FRAME-CODEPAGE checks=\d+ failures=0' -and (Get-Content $log) -contains 'exit=0x00000000'
 $results+=@{architecture=$arch;pid=$p.Id;exit=$p.ExitCode;pass=$ok;report=$text}
 $results|ConvertTo-Json|Set-Content "$root/runtime-result.json"
 if(!$ok){throw "Frame $arch failed"}
}
