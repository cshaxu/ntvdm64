param(
    [Parameter(Mandatory=$true)][string]$ProviderBuildRoot,
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [Parameter(Mandatory=$true)][ValidateSet('hook','menu','timer','idle','common-dialog')][string]$Case,
    [switch]$LogicOnly,
    [string]$Ninja='ninja.exe',
    [string]$VsDevCmd='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
# Invoke from an owned background supervisor. These are bounded contract
# fixtures; they do not substitute for the real CCPU and application gates.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$provider=(Resolve-Path $ProviderBuildRoot).Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Creation fixture output must remain below repository build.'
}
New-Item -ItemType Directory -Force -Path $out | Out-Null
$vs=(Resolve-Path $VsDevCmd).Path
function Invoke-CompileCommand([string]$command) {
    & cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cd /d "'+$provider+'" && '+$command) >> "$out/build.log" 2>&1
    if($LASTEXITCODE){throw 'Creation fixture compile/link failed; inspect build.log'}
}
$commands=& $Ninja -C $provider -t commands wow32.dll
if($LASTEXITCODE){throw 'Unable to read production compile graph'}
if($Case -eq 'common-dialog') {
    Invoke-CompileCommand ('"'+$Ninja+'" obj/wow32-dll/wow_task_cleanup_entry.obj obj/wow32-dll/wow_common_dialog_callback.obj')
}
$source=switch($Case){'hook'{'wow_hook_callback'} 'timer'{'wow_timer_callback'} 'idle'{'wow_idle_hook_binding'} 'common-dialog'{'wow_common_dialog_callback'} default{'wow_window_creation_binding'}}
$compile=$commands | Where-Object {$_ -match ($source+'\.c$')} | Select-Object -Last 1
if(!$compile){throw 'Missing production compile rule'}
$fixtureName=switch($Case){'hook'{'wow_hook_callback_fixture.c'} 'timer'{'wow_timer_callback_fixture.c'} 'idle'{'wow_idle_hook_fixture.c'} 'common-dialog'{'wow_common_dialog_callback_fixture.c'} default{'wow_creation_menu_fixture.c'}}
$fixture=$compile -replace ('/Fo\S+\s+\S+'+$source+'\.c$'), ('/Fo"'+$out+'\fixture.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\'+$fixtureName+'"')
if($fixture -eq $compile){throw 'Fixture substitution failed'}
if($Case -eq 'menu') {
    $text=Get-Content "$repo/src/wow32-dll/source/wow_window_creation_binding.c" -Raw
    if(!$text.Contains('BOOL WINAPI wow_window_creation_menu(')){throw 'Missing production menu boundary'}
    # Extract the exact standalone boundary, not a prefix containing unrelated
    # static callers. No algorithm is copied or rewritten in the fixture.
    $start=$text.IndexOf('BOOL WINAPI wow_window_creation_menu(')
    $end=$text.IndexOf('static void trace_creation(', $start)
    if($start -lt 0 -or $end -le $start){throw 'Menu boundary delimiters changed'}
    $headers="#include `"wow_window_creation_binding.h`"`n#include `"wow_menu_resource_bindings.h`"`n#include <stdio.h>`n"
    [IO.File]::WriteAllText("$out/creation-menu.inc",$headers+$text.Substring($start,$end-$start))
}
Invoke-CompileCommand ($fixture+' /Gy /I"'+$out+'"')
$selected=if($Case -ne 'menu'){' "'+$provider+'\obj\wow32-dll\'+$source+'.obj"'}else{''}
if($Case -eq 'common-dialog') { $selected += ' "'+$provider+'\obj\wow32-dll\wow_task_cleanup_entry.obj"' }
Invoke-CompileCommand ('link /nologo /OPT:REF /out:"'+$out+'\fixture.exe" "'+$out+'\fixture.obj"'+$selected+' kernel32.lib user32.lib')
$start=[DateTime]::UtcNow
$arguments=if($LogicOnly -and $Case -eq 'idle'){'--logic-only'}else{''}
$startOptions=@{FilePath="$out/fixture.exe";WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$out/stdout.txt";RedirectStandardError="$out/stderr.txt"}
if($arguments){$startOptions.ArgumentList=$arguments}
$p=Start-Process @startOptions
$ownedHandle=$p.Handle
@{pid=$p.Id;start=$start.ToString('o');deadline=$start.AddSeconds(30).ToString('o')} | ConvertTo-Json | Set-Content "$out/process.json"
if(!$p.WaitForExit(30000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned creation fixture exceeded deadline'}
$p.Refresh()
$p.ExitCode | Set-Content "$out/fixture-exit.txt"
$output=@(Get-Content "$out/stdout.txt";Get-Content "$out/stderr.txt")
$output | Write-Output
if($null -eq $p.ExitCode -or $p.ExitCode -ne 0){throw 'Creation fixture failed'}
$marker=switch($Case){'hook'{'^WOW_HOOK_CREATION_GUARD errors=0 enrollment=6 accepted=1 rollback=4$'} 'timer'{'^WOW_TIMER_CALLBACK_GUARD errors=0 calls=30 nested=1 exception=1 denied=2$'} 'idle'{'^WOW_IDLE_HOOK_GUARD errors=0 calls=5 nested=1 exception=1 denied=2 foreign=2 retired=1 worker_shared=1$'} 'common-dialog'{'^WOW_COMMDLG_EXIT_GUARD errors=0 entries=3 resource_branch=1 nested=1 exception=1 denied=3 early_order=1 rejected_free=1$'} default{'^WOW_CREATION_MENU errors=0 loads=4$'}}
if(!($output -match $marker)){throw 'Creation fixture completion assertions missing'}
if($LogicOnly -and $Case -ne 'idle'){throw 'LogicOnly is valid only for the idle fixture'}
if($LogicOnly -and !($output -match '^WOW_IDLE_NATIVE_CHAIN unavailable=logic-only$')) {
    throw 'Explicit logic-only idle boundary marker missing'
}
if(!$LogicOnly -and $Case -eq 'idle' -and !($output -match '^WOW_IDLE_NATIVE_CHAIN errors=0 next=[1-9][0-9]* registered_and_unregistered=1 direct_not_forwarded=1$')) {
    throw 'Real native idle-chain forwarding assertions missing'
}
if($Case -eq 'common-dialog' -and !($output -match '^WOW_COMMDLG_RESOURCE_FAILURE errors=0 fatal_edges=3 skipped_free=0$')) {
    throw 'Common-dialog resource failure assertions missing'
}
if($Case -eq 'common-dialog' -and !($output -match '^WOW_COMMDLG_RESOURCE_ASSOCIATION errors=0 bound_before_callback=1 consumed_before_release=1 failures_fatal=2$')) {
    throw 'Common-dialog resource-owner registration/consumption assertions missing'
}
if($Case -eq 'common-dialog' -and !($output -match '^WOW_UNINITIALIZED_CLEANUP errors=0 exact_td=1 foreign_rejected=1 committed_rejected=1 retired_rejected=1$')) {
    throw 'Uninitialized original TD identity assertions missing'
}
"WOW_CREATION_BOUNDARY_OK case=$Case"
