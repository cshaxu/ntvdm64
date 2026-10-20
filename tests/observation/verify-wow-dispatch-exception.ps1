[CmdletBinding()]
param([Parameter(Mandatory)][string]$ProviderBuildRoot,
      [Parameter(Mandatory)][string]$BuildRoot,
      [ValidateSet('1122ab55d55b4d15699715ce1416b092b5887772')][string]$ReferenceRevision)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Require fresh build-owned evidence directory'
}
$null=New-Item -ItemType Directory $root
$graph=Get-Content "$ProviderBuildRoot/build.ninja" -Raw
$flags=[regex]::Match($graph,'(?m)^cflags = (.*)\r?$').Groups[1].Value.Trim()
$release=[regex]::Match($graph,'(?m)^release_cflags = (.*)\r?$').Groups[1].Value.Trim()
if(!$flags.Contains('/DCPU_40_STYLE') -or !$flags.Contains('/MT') -or !$release){throw 'Require selected CCPU40 provider graph'}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds,[int]$expected=0){
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;executable=$file;deadline=$p.StartTime.AddSeconds($seconds).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name-process.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$root/$name-timeout.txt";throw "Timeout $name"}
    $p.WaitForExit();$p.Refresh();$p.ExitCode|Set-Content "$root/$name.exit"
    if($p.ExitCode -ne $expected){throw "Failed $name actual=$($p.ExitCode) expected=$expected"}
}
try {
    Run-Owned 'extract-wndproc' 'powershell.exe' ('-NoProfile -ExecutionPolicy Bypass -File "'+$repo+'/tests/observation/extract-wow-wndproc-fixture.ps1" -BuildRoot "'+$root+'"') 30
    if((Get-Content "$root/extract-wndproc.out" -Raw) -notmatch '^WOW_WNDPROC_MIRROR_BYTE_EXACT_SELECTED_BODY'){throw 'Original WndProc body extraction failed'}
    $sources=@('src/mvdm/wow32/wow32.c','src/mvdm/wow32/wmdisp32.c','src/wow32-dll/source/wow_user_thunk_scope.c','tests/adapter-mvdm-host-out/wow/wow_dispatch_exception_fixture.c')
    $objects=@()
    foreach($source in $sources){
        $name=[IO.Path]::GetFileNameWithoutExtension($source)
        $object="$root/$name.obj";$objects+=('"'+$object+'"')
        $input="$repo/$source"
        if($ReferenceRevision -and $name -eq 'wow32'){
            $body=(& git -C $repo show "${ReferenceRevision}:$source") -join "`n"
            if($LASTEXITCODE){throw 'Missing immutable dispatcher reference'}
            $input="$root/reference-wow32.c";[IO.File]::WriteAllText($input,$body)
        }
        $extra=if($name -eq 'wow_user_thunk_scope'){' /Dwow_user_invoke_thunk=fixture_unused_original_invoke'}elseif($name -eq 'wow32'){' /DWOWSysErrorBox=wow_user_sys_error_box'}else{''}
        # Package unrelated original global callback tables too, so OPT:REF
        # can discard DLL-initialization graphs without fake service stubs.
        $include=if($name -eq 'wow_dispatch_exception_fixture'){' /I"'+$root+'"'}else{''}
        $arguments='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe '+$flags+' '+$release+$extra+$include+' /Gy /Gw /GL /Fo"'+$object+'" "'+$input+'""'
        Run-Owned $name 'cmd.exe' $arguments 120
    }
    # MSVC requires symbols in the complete original TU to resolve before
    # removing unreferenced COMDATs. This explicit, closed list covers only
    # initialization/legacy exports. A new unresolved dependency fails; never
    # derive permissive aliases from a failed link. The map must then prove
    # the excluded owners were discarded, not merely left unexecuted.
    $unreached=@(
        '_AddProcessSharedList@0','_RegisterWowBaseHandlers@4','_SetFakeDialogClass@4',
        '_W32LocalUnlock@8','_ExitVDM@8','_CleanseSharedList@0','_WU32ICBStoreHandle@8',
        '_W32GlobalFree16@4','_NtvdmGetWindowsDirectoryA','_W32LocalReAlloc@20',
        '_InitVisRgn@0','_DBGNotifyDebugged','_getMSW','_WK32InitWowIsKnownDLL@4',
        '_W32LockResource@8','_ShowStartGlass@4','_InitThunkTableOffsets@0',
        '_W32GlobalAlloc16@8','_mvdm_softpc_fast_bop_dispatch_offset@0',
        '_W32Win16WndProcEx@24','_W32InitHookState@4','_Sim32pGetVDMPointer',
        '_W32GetExpWinVer@4','_W32SizeofResource@8','_W32LocalSize@8',
        '_ntvdm_shadow_reg_open_key_ex@20','_InitCBFormats@0','_W32LocalLock@8',
        '_W32FindResource@16','_NtvdmGetSystemDirectoryA','_W32UnlockResource@8',
        '_CreateSmallHeap@0','_W32InitWOWSetupNames@0','_W32LocalFree@8',
        '_mvdm_softpc_wow_page_domain_flat_address_array',
        '_mvdm_softpc_fast_bop_callback_offset@0','_W32EmptyClipboard@0',
        '_W32LoadResource@8','_W32EditNextWord@20','_W32DDEFreeGlobalMem32@4',
        '_W32InitDlg@8','_W32LocalAlloc@12','_W32FreeResource@8',
        '_WK32InitializeHungAppSupport@0','_ntvdm_shadow_reg_close_key@4',
        '_UserRegisterWowHandlers@8','_InitParamMap@0')
    $unreachedData=@('_VdmLoadCritSec','_tableoffsets','_TimerList','_vpDebugWOW','_dwWOWBatchLimit')
    $aliases=@($unreached|ForEach-Object {'/alternatename:'+$_+'=_fixture_unreachable_dependency'})+
        @($unreachedData|ForEach-Object {'/alternatename:'+$_+'=_fixture_unreachable_data'})
    $arguments='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && link.exe /nologo /LTCG /OPT:REF /out:"'+$root+'/fixture.exe" /map:"'+$root+'/fixture.map" '+($objects -join ' ')+' '+($aliases -join ' ')+' kernel32.lib user32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib"'
    Run-Owned 'link' 'cmd.exe' $arguments 120
    $map=Get-Content "$root/fixture.map" -Raw
    if($map -match '\s_(W32Init@0|W32DllInitialize@12|LockWowSharedMemory@0|fixture_unused_original_invoke@8|wow_user_borrow_enter@4|wow_user_borrow_leave_required@4|fixture_unreachable_dependency|fixture_unreachable_data)\s'){
        throw 'Unrelated original service graph survived fixture link'
    }
    if($ReferenceRevision){
        foreach($case in @('ignore','close')){
            Run-Owned "negative-$case" "$root/fixture.exe" "--abort-$case" 30 1
            if((Get-Content "$root/negative-$case.out" -Raw) -notmatch 'DISPATCH_ABORT_WRONG phase=[56]'){throw 'Missing source-causal old dispatcher failure'}
        }
        @{reference=$ReferenceRevision;scope='both old dispatcher abort-target counterexamples, not a current pass'}|ConvertTo-Json|Set-Content "$root/reference.json"
        return
    }
    Run-Owned 'fixture' "$root/fixture.exe" '--verify' 30
    $output=Get-Content "$root/fixture.out" -Raw
    if($output -notmatch 'PASS selected W32Dispatch and W32Exception' -or
        $output -notmatch 'DISPATCH_SEARCH debugger-first-chance=1 child-exit=0' -or
        ([regex]::Matches($output,'DISPATCH_EXCEPTION phase=')).Count -ne 8 -or
        ([regex]::Matches($output,'HARD_ERROR_SCOPE mode=')).Count -ne 5){throw 'Missing boundary assertions'}
    @($sources|ForEach-Object {"$repo/$_"}) + @("$repo/src/mvdm/wow32/wmdisp32.c","$root/original-wndproc.inc","$ProviderBuildRoot/build.ninja") |
        Get-FileHash | Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
    '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
