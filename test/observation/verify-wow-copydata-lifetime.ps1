param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned source evidence required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds,$expected=0){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned source deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code -ne $expected){throw "$name actual $code, expected $expected"}
}
try{
 & "$repo/tools/build-fixtures.cmd" x86 wow-copydata-lifetime *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native COPYDATA build failed'}
 $native="$repo/build/.ninja/fixtures-x86"
 Copy-Item "$native/wow-copydata-lifetime.exe" "$root/unit.exe"
 Get-FileHash "$native/copydata_exact_body.h","$native/queue_selector_intervals.h","$root/unit.exe"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 Owned body "$root/unit.exe" '' 30
 $output=Get-Content "$root/body.stdout.txt" -Raw
 if($output -notmatch 'COPYDATA_LIFETIME errors=0 scenarios=59 directions=2 stages=4 nested=1 immutable_release=1 exception=2 thread_status=1 hook_roles=2 filter_nested_different=1 embedded_headers=2 filter_output_rollback=1 guest_proof=0' -or
    $output -notmatch 'FILTER_SELECTOR cases=14 entry_values=2 modes=7 errors=0 guest_proof=0' -or
    $output -notmatch 'QUEUE_SELECTOR cases=18 owners=3 modes=3 errors=0 full_queue_proof=0 guest_proof=0'){
    throw 'Incomplete original COPYDATA and filter-selector source proof'
 }
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
