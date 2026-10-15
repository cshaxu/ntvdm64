param(
    [Parameter(Mandatory=$true)][string]$ProviderBuildRoot,
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [Parameter(Mandatory=$true)][ValidateSet('hook','menu')][string]$Case,
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
$source=if($Case -eq 'hook'){'wow_hook_callback'}else{'wow_window_creation_binding'}
$compile=$commands | Where-Object {$_ -match ($source+'\.c$')} | Select-Object -Last 1
if(!$compile){throw 'Missing production compile rule'}
$fixtureName=if($Case -eq 'hook'){'wow_hook_callback_fixture.c'}else{'wow_creation_menu_fixture.c'}
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
$selected=if($Case -eq 'hook'){' "'+$provider+'\obj\wow32-dll\wow_hook_callback.obj"'}else{''}
Invoke-CompileCommand ('link /nologo /OPT:REF /out:"'+$out+'\fixture.exe" "'+$out+'\fixture.obj"'+$selected+' kernel32.lib user32.lib')
$start=[DateTime]::UtcNow
$p=Start-Process "$out/fixture.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/stdout.txt" -RedirectStandardError "$out/stderr.txt"
$ownedHandle=$p.Handle
@{pid=$p.Id;start=$start.ToString('o');deadline=$start.AddSeconds(30).ToString('o')} | ConvertTo-Json | Set-Content "$out/process.json"
if(!$p.WaitForExit(30000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned creation fixture exceeded deadline'}
$p.Refresh()
$p.ExitCode | Set-Content "$out/fixture-exit.txt"
$output=@(Get-Content "$out/stdout.txt";Get-Content "$out/stderr.txt")
$output | Write-Output
if($null -eq $p.ExitCode -or $p.ExitCode -ne 0){throw 'Creation fixture failed'}
$marker=if($Case -eq 'hook'){'^WOW_HOOK_CREATION_GUARD errors=0 enrollment=6 accepted=1 rollback=4$'}else{'^WOW_CREATION_MENU errors=0 loads=4$'}
if(!($output -match $marker)){throw 'Creation fixture completion assertions missing'}
"WOW_CREATION_BOUNDARY_OK case=$Case"
