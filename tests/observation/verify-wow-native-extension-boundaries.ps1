param(
 [Parameter(Mandatory)][string]$ProviderBuildRoot,
 [Parameter(Mandatory)][string]$BuildRoot
)
# Supplementary unit/original-frame group. Run in an owned hidden background
# supervisor; actual generic-thunk/guest callbacks are a separate mandatory gate.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$provider=(Resolve-Path $ProviderBuildRoot).Path
$graph=Get-Content "$provider/build.ninja" -Raw
$flags=[regex]::Match($graph,'(?m)^cflags = (.*)\r?$').Groups[1].Value.Trim()
$release=[regex]::Match($graph,'(?m)^release_cflags = (.*)\r?$').Groups[1].Value.Trim()
if(!$flags.Contains('/DCPU_40_STYLE') -or !$flags.Contains('/MT') -or !$release){throw 'Require formal x86/MT/CCPU40 graph'}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$shell=(Get-Process -Id $PID).Path
function Run-Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 $inputs=@($PSCommandPath,"$provider/build.ninja","$repo/src/mvdm/wow32/wcall16.c",
  "$repo/src/wow32-dll/source/wow_user_native_extension.c","$repo/src/wow32-dll/include/wow_user_native_extension.h",
  "$repo/tests/adapter-mvdm-host-out/wow/wow_native_extension_boundary_fixture.c",
  "$repo/tests/adapter-mvdm-host-out/wow/wow_public_callback_rejection_fixture.c",
  "$repo/tests/adapter-mvdm-host-out/wow/wow_original_callback_frame_fixture.c",
  "$repo/tests/adapter-mvdm-host-out/wow/wow_callback_abort_fixture.c",
  "$repo/src/wow32-dll/source/wow_callback_frame_lease.c",
  "$repo/src/wow32-dll/include/wow_callback_frame_lease.h",
  "$repo/tests/observation/verify-wow-original-callback-frame.ps1","$repo/src/mvdm/wow32/wmmedia.c")
 Get-FileHash $inputs|Export-Csv -NoTypeInformation "$root/source-sha256.csv"
 foreach($case in @('extension','mm-body','rejection')){
  $sources=if($case -ne 'rejection'){@('src/wow32-dll/source/wow_user_native_extension.c','tests/adapter-mvdm-host-out/wow/wow_native_extension_boundary_fixture.c')}else{@('src/mvdm/wow32/wcall16.c','tests/adapter-mvdm-host-out/wow/wow_public_callback_rejection_fixture.c')}
  if($case -eq 'mm-body'){$sources+='src/mvdm/wow32/wmmedia.c'}
  $objects=@();$i=0
  foreach($source in $sources){
   $extra=if($case -eq 'rejection'){' /DCallBack16=wow_test_reject_callback'}elseif($case -eq 'mm-body' -and $source -like '*wmmedia.c'){' /DWMM32CallProc32=wow_original_WMM32CallProc32'}elseif($case -eq 'mm-body' -and $source -like '*fixture.c'){' /DWOW_EXTENSION_ORIGINAL_MM'}else{''}
   $object="$root/$case-$i.obj";$objects+=('"'+$object+'"')
   $line='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe '+$flags+' '+$release+' /Gy'+$extra+' /Fo"'+$object+'" "'+$repo+'/'+$source+'""'
   Run-Owned "$case-compile-$i" cmd.exe $line 120;$i++
  }
  $line='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && link.exe /nologo /MACHINE:X86 /OPT:REF /out:"'+$root+'/'+$case+'.exe" '+($objects -join ' ')+' kernel32.lib user32.lib legacy_stdio_definitions.lib"'
  Run-Owned "$case-link" cmd.exe $line 120
  Run-Owned "$case-run" "$root/$case.exe" '--verify' 30
 }
 if((Get-Content "$root/extension-run.stdout.txt" -Raw) -notmatch 'WOW_NATIVE_EXTENSION_BOUNDARY errors=0 .*unwind=1 reject=1 result_error=1 native_loader=1'){throw 'Missing paired scope/resource/exception assertions'}
 if((Get-Content "$root/mm-body-run.stdout.txt" -Raw) -notmatch 'WOW_ORIGINAL_MM_EXTENSION errors=0 actual_body=1 five_args=1 directory_order=1 zero_target=1 exception=1'){throw 'Missing actual original MM body/argument/zero-target assertions'}
 if((Get-Content "$root/rejection-run.stdout.txt" -Raw) -notmatch 'WOW_PUBLIC_CALLBACK_REJECTION errors=0 .*invalid_size_untouched=1 no_guest_entry=1'){throw 'Missing original public-export refusal assertions'}
 Run-Owned original-frame $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-original-callback-frame.ps1" -ProviderBuildRoot "'+$provider+'" -BuildRoot "'+$root+'/original-frame"') 400
 foreach($input in Import-Csv "$root/source-sha256.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Boundary source changed during group'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
