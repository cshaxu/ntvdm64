param(
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('all','extension','mm-body','rejection','original-frame')]
 [string]$Phase = 'all'
)
# Supplementary unit/original-frame group. Run in an owned hidden background
# supervisor; actual generic-thunk/guest callbacks are a separate mandatory gate.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$provider="$repo/build/.ninja/msvc-x86-release/wow32"
$shell=(Get-Process -Id $PID).Path
function Run-Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 $inputs=@($PSCommandPath,"$repo/src/mvdm-host/wow32/wcall16.c",
  "$repo/src/compat/wow_user_native_extension.c","$repo/src/compat/include/wow_user_native_extension.h",
  "$repo/src/compat/include/wow_native_extension_provider.h","$repo/test/mvdm-host/wow_native_extension_provider.h",
  "$repo/test/adapter-mvdm-host-out/wow/wow_native_extension_boundary_fixture.c",
  "$repo/test/adapter-mvdm-host-out/wow/wow_public_callback_rejection_fixture.c",
  "$repo/test/adapter-mvdm-host-out/wow/wow_original_callback_frame_fixture.c",
  "$repo/test/adapter-mvdm-host-out/wow/wow_callback_abort_fixture.c",
  "$repo/src/compat/wow_callback_frame_lease.c",
  "$repo/src/compat/include/wow_callback_frame_lease.h",
  "$repo/test/observation/verify-wow-original-callback-frame.ps1","$repo/src/mvdm-host/wow32/wmmedia.c")
 Get-FileHash $inputs|Export-Csv -NoTypeInformation "$root/source-sha256.csv"
 & "$repo/tools/build-fixtures.cmd" x86 wow-extension-contracts *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native extension build failed'}
 Get-FileHash "$provider/build.ninja"|Export-Csv -NoTypeInformation "$root/build-identity.csv"
 $cases=if($Phase -eq 'all'){@('extension','mm-body','rejection')}elseif($Phase -eq 'original-frame'){@()}else{@($Phase)}
 foreach($case in $cases){
  Copy-Item "$provider/extension-$case.exe" "$root/$case.exe"
  Run-Owned "$case-run" "$root/$case.exe" '--verify' 30
 }
 if($Phase -in @('all','extension') -and (Get-Content "$root/extension-run.stdout.txt" -Raw) -notmatch 'WOW_NATIVE_EXTENSION_BOUNDARY errors=0 .*unwind=1 reject=1 result_error=1 native_loader=1 tid32_native_sync=1 mm_private_exports=11'){throw 'Missing paired scope/resource/exception assertions'}
 if($Phase -in @('all','mm-body') -and (Get-Content "$root/mm-body-run.stdout.txt" -Raw) -notmatch 'WOW_ORIGINAL_MM_EXTENSION errors=0 actual_body=1 five_args=1 directory_order=1 zero_target=1 exception=1 tid32_through_original=1'){throw 'Missing actual original MM body/argument/zero-target assertions'}
 if($Phase -in @('all','rejection') -and (Get-Content "$root/rejection-run.stdout.txt" -Raw) -notmatch 'WOW_PUBLIC_CALLBACK_REJECTION errors=0 .*invalid_size_untouched=1 no_guest_entry=1'){throw 'Missing original public-export refusal assertions'}
 if($Phase -in @('all','original-frame')){Run-Owned original-frame $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-original-callback-frame.ps1" -BuildRoot "'+$root+'/original-frame"') 400}
 foreach($input in Import-Csv "$root/source-sha256.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Boundary source changed during group'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
