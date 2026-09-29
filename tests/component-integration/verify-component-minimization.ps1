[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$graph=Get-Content -LiteralPath (Join-Path $BuildRoot 'build.ninja') -Raw
if($graph -match 'src/product-package|obj/product-package|NTKVM_CONPTY_TEST_RELEASE'){
    throw 'Retired package owner or test-only release selected in production graph'
}
if($graph -notmatch 'src/ntvdm-exe/package_layout.c'){
    throw 'Worker package binding missing'
}
foreach($name in 'mvdm_base_vdm_environment','mvdm_crt_redirect','nt_thread_alert_compat','wow_hard_error_dialog'){
    if(!(Test-Path (Join-Path $repo "src/ntvdm-exe/win32/$name.h")) -or
        (Test-Path (Join-Path $repo "src/opennt-abi/host-compat/include/$name.h"))){
        throw "Worker-private declaration not consolidated: $name"
    }
}
$map=Get-Content -LiteralPath (Join-Path $BuildRoot 'ntkvm.exe.map') -Raw
foreach($symbol in 'run16_native_capture_begin','run16_native_capture_read','run16_native_capture_end',
    'run16_native_view_wait','frontend_service_launch','frontend_service_wait','ntkvm_conpty_release',
    'run16_native_launch_start'){
    if($map -match [regex]::Escape($symbol)){throw "Obsolete production symbol: $symbol"}
}
foreach($symbol in 'run16_native_screen_apply','run16_native_cells_write','ntkvm_conpty_launch',
    'run16_native_view_begin','run16_native_view_present'){
    if($map -match [regex]::Escape($symbol)){throw "Displaced frontend backend remains: $symbol"}
}
$nativeMap=Get-Content -LiteralPath (Join-Path $BuildRoot 'ntcon.exe.map') -Raw
foreach($symbol in 'ntcon_screen_apply','ntcon_cells_write'){
    if($nativeMap -notmatch [regex]::Escape($symbol)){throw "Native worker provider missing: $symbol"}
}
$imports=& dumpbin.exe /nologo /imports (Join-Path $BuildRoot 'ntmon.exe')
if($LASTEXITCODE -or ($imports -match 'USER32.dll|wsprintfW')){
    throw 'NTMon still imports presentation DLL solely for formatting'
}
Write-Output 'PASS component consolidation, absent obsolete symbols, retained providers and NTMon imports'
