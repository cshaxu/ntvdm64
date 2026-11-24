param(

    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [Parameter(Mandatory=$true)][ValidateSet('hook','menu','timer','idle','common-dialog')][string]$Case,
    [switch]$LogicOnly
)
# Invoke from an owned background supervisor. These are bounded contract
# fixtures; they do not substitute for the real CCPU and application gates.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path

$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Creation fixture output must remain below repository build.'
}
New-Item -ItemType Directory -Force -Path $out | Out-Null
& "$repo/tools/build-fixtures.cmd" x86 "wow-creation-$Case" > "$out/build.log" 2>&1
if($LASTEXITCODE){throw 'Native creation fixture build failed; inspect build.log'}
Copy-Item "$repo/build/.ninja/msvc-x86-release/wow32/creation-$Case.exe" "$out/fixture.exe" -Force
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
$marker=switch($Case){'hook'{'^WOW_HOOK_CREATION_GUARD errors=0 enrollment=6 accepted=1 rollback=4$'} 'timer'{'^WOW_TIMER_CALLBACK_GUARD errors=0 calls=30 nested=1 exception=1 denied=2$'} 'idle'{'^WOW_IDLE_HOOK_GUARD errors=0 calls=5 nested=1 exception=1 denied=2 foreign=2 retired=1 worker_shared=1$'} 'common-dialog'{'^WOW_COMMDLG_EXIT_GUARD errors=0 entries=3 resource_branch=1 nested=1 exception=1 denied=3 early_order=1 rejected_free=1$'} default{'^WOW_CREATION_MENU errors=0 loads=6$'}}
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
