[CmdletBinding()]
param(
    [ValidateSet('x86')]
    [string]$Architecture = 'x86',
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$ParentImportLibrary = '',
    # Explicit developer-only wrapper. The formal graph remains direct MSVC.
    [string]$CompilerCache = '',
    [ValidateRange(1, 32)]
    [int]$ParallelJobs = 8
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function ConvertTo-NinjaPath([string]$Path) {
    $value = $Path.Replace('\', '/')
    if ($value.Length -ge 2 -and $value[1] -eq ':') {
        return $value.Substring(0, 1) + '$' + ':' + $value.Substring(2)
    }
    return $value
}

function Get-SelectedWow32Sources([string]$Manifest) {
    $selected = [System.Collections.Generic.List[string]]::new()
    $disabled = $false
    foreach ($line in Get-Content -LiteralPath $Manifest) {
        if ($line -match '^\s*!IF\s+0\s*$') { $disabled = $true; continue }
        if ($disabled -and $line -match '^\s*!ENDIF\s*$') { $disabled = $false; continue }
        if (!$disabled -and $line -match '^\s*([A-Za-z0-9_]+\.c)\s*(?:\\|$)') {
            $selected.Add($Matches[1])
        }
    }
    $unique = @($selected | Select-Object -Unique)
    if ($unique.Count -ne 77) {
        throw "WOW32 provider must select exactly 77 C bodies; found $($unique.Count)."
    }
    return $unique
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$wow = Join-Path $root 'src/mvdm/wow32'
$adapterWow = Join-Path $root 'src/wow32-dll/source'
$manifest = Join-Path $wow 'sources'
$definition = Join-Path $wow 'wow32.def'
$resource = Join-Path $wow 'wow32.rc'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'

if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) {
    throw 'MSVC Build Tools and Ninja are required.'
}
foreach ($path in @($manifest, $definition, $resource)) {
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing original WOW32 carrier: $path" }
}
if ([string]::IsNullOrWhiteSpace($ParentImportLibrary)) {
    throw 'T404 S5 formal WOW32 composition requires -ParentImportLibrary from the architecture-named parent product.'
}
$parentImportLibraryPath = [IO.Path]::GetFullPath($ParentImportLibrary)
if (!(Test-Path -LiteralPath $parentImportLibraryPath -PathType Leaf)) {
    throw "Parent import library does not exist: $parentImportLibraryPath"
}
$providerSupportSources = @(
    (Join-Path $adapterWow 'wow_callback_frame_lease.c'),
    (Join-Path $adapterWow 'wow_sim32_pointer_compat.c'),
    (Join-Path $adapterWow 'wow_private_user_compat.c'),
    (Join-Path $adapterWow 'wow_bitmap_bindings.c'),
    (Join-Path $adapterWow 'wow_window_words_binding.c'),
    (Join-Path $adapterWow 'wow_window_procedure_entry.c'),
    (Join-Path $adapterWow 'wow_user_object_bindings.c'),
    (Join-Path $adapterWow 'wow_user_discovery.c'),
    (Join-Path $adapterWow 'wow_hook_native_binding.c'),
    (Join-Path $adapterWow 'wow_menu_client_bindings.c'),
    (Join-Path $adapterWow 'wow_menu_native_bindings.c'),
    (Join-Path $adapterWow 'wow_cleanup_bindings.c'),
    (Join-Path $adapterWow 'wow_user_borrow_scope.c'),
    (Join-Path $adapterWow 'wow_class_client_bindings.c'),
    (Join-Path $adapterWow 'wow_task_event_binding.c'),
    (Join-Path $adapterWow 'wow_task_profile_binding.c'),
    (Join-Path $adapterWow 'wow_task_termination.c'),
    (Join-Path $adapterWow 'wow_task_parent.c'),
    (Join-Path $adapterWow 'wow_launch_identity.c'),
    (Join-Path $adapterWow 'wow_native_process.c'),
    (Join-Path $adapterWow 'wow_user_message_bridge.c'),
    (Join-Path $adapterWow 'wow_user_task_lifecycle.c'),
    (Join-Path $adapterWow 'wow_input_group_lifetime.c'),
    (Join-Path $adapterWow 'wow_dialog_creation_binding.c'),
    (Join-Path $adapterWow 'wow_dialog_keyboard_binding.c'),
    # T447 S3 shared retained state and original-control dependency bindings.
    # These are one provider group, not optional native/recovered fallbacks.
    (Join-Path $adapterWow 'wow_dialog_call_bindings.c'),
    (Join-Path $adapterWow 'wow_dialog_procedure_bindings.c'),
    (Join-Path $adapterWow 'wow_dialog_finalization_binding.c'),
    (Join-Path $adapterWow 'wow_dialog_native_bindings.c'),
    (Join-Path $adapterWow 'wow_dialog_state_bindings.c'),
    (Join-Path $adapterWow 'wow_default_message_bindings.c'),
    (Join-Path $adapterWow 'wow_default_native_bindings.c'),
    (Join-Path $adapterWow 'wow_default_state_bindings.c'),
    (Join-Path $adapterWow 'wow_default_window_bindings.c'),
    (Join-Path $adapterWow 'wow_dialog_context_bindings.c'),
    (Join-Path $adapterWow 'wow_edit_call_bindings.c'),
    (Join-Path $adapterWow 'wow_control_brush_bindings.c'),
    (Join-Path $adapterWow 'wow_edit_conversion_bindings.c'),
    (Join-Path $adapterWow 'wow_edit_drawing_bindings.c'),
    (Join-Path $adapterWow 'wow_control_draw_state.c'),
    (Join-Path $adapterWow 'wow_button_resources.c'),
    (Join-Path $adapterWow 'wow_button_lifetime.c'),
    (Join-Path $adapterWow 'wow_edit_entry_bindings.c'),
    (Join-Path $adapterWow 'wow_edit_lookaside_bindings.c'),
    (Join-Path $adapterWow 'wow_edit_native_bindings.c'),
    (Join-Path $adapterWow 'wow_list_native_bindings.c'),
    (Join-Path $adapterWow 'wow_edit_state_bindings.c'),
    (Join-Path $adapterWow 'wow_control_extent_bindings.c'),
    (Join-Path $adapterWow 'wow_edit_thread_bindings.c'),
    (Join-Path $adapterWow 'wow_window_client_access_bindings.c'),
    (Join-Path $adapterWow 'wow_window_client_state_bindings.c'),
      (Join-Path $adapterWow 'wow_hook_callback.c'),
    (Join-Path $adapterWow 'wow_timer_callback.c'),
    (Join-Path $adapterWow 'wow_idle_hook_binding.c'),
    (Join-Path $adapterWow 'wow_shell_callback.c'),
    (Join-Path $adapterWow 'wow_registry_binding.c'),
    (Join-Path $adapterWow 'wow_task_cleanup_entry.c'),
    (Join-Path $adapterWow 'wow_common_dialog_callback.c'),
    (Join-Path $adapterWow 'wow_common_dialog_calls.c'),
      (Join-Path $adapterWow 'wow_message_lifetime.c'),
    (Join-Path $adapterWow 'wow_user_thunk_scope.c'),
    (Join-Path $adapterWow 'wow_user_native_extension.c'),
    (Join-Path $adapterWow 'wow_user_private_access.c'),
    (Join-Path $adapterWow 'wow_window_creation_binding.c'),
    (Join-Path $adapterWow 'wow_class_words_binding.c'),
    (Join-Path $adapterWow 'wow_clipboard_epoch.c'),
    (Join-Path $adapterWow 'wow_standard_class_bindings.c'),
    (Join-Path $adapterWow 'wow_user_registration_bridge.c'),
    (Join-Path $root 'src/opennt-host/windows/core/ntuser/rtl/chartran.c'),
    (Join-Path $root 'src/opennt-host/windows/core/ntuser/rtl/random.c'),
    (Join-Path $root 'src/opennt-host/windows/core/ntuser/rtl/help.c'),
    (Join-Path $adapterWow 'ntuser_rtl_compat.c'),
    (Join-Path $adapterWow 'wow_public_user_facade.c'),
    # This is the already-admitted narrow historical CRT spelling bridge.  It
    # has no parent-machine state, so the late-loaded DLL may own its copy.
    (Join-Path $root 'src/ntvdm-exe/win32/crt_compat.c')
)
foreach ($path in $providerSupportSources) {
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing admitted WOW32 provider support source: $path"
    }
}

$sources = @(Get-SelectedWow32Sources $manifest)
foreach ($name in $sources) {
    if (!(Test-Path -LiteralPath (Join-Path $wow $name) -PathType Leaf)) {
        throw "Selected WOW32 source is missing: $name"
    }
}

$headers = @(Get-ChildItem -LiteralPath $wow -Filter '*.h' -File)
if ($headers.Count -ne 104) { throw "WOW32 must retain 104 root headers; found $($headers.Count)." }

if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $build = Join-Path $root 'build/.ninja/msvc-x86-release/wow32'
} else {
    $build = [IO.Path]::GetFullPath($BuildRoot)
}
$compilerCommand = & "$PSScriptRoot/Initialize-NinjaTree.ps1" -BuildRoot $build -CompilerCache $CompilerCache
New-Item -ItemType Directory -Force -Path $build, (Join-Path $build 'obj'),
    (Join-Path $build 'obj\opennt-user'), (Join-Path $build 'obj\wow32-dll'),
    (Join-Path $build 'obj\adapter-win32') | Out-Null

# The original definition deliberately retains undecorated public aliases for
# two stdcall COMM exports.  Keep the original file immutable and derive only
# the x86 linker aliases needed by the selected source bodies.
$x86Definition = Join-Path $build 'wow32-x86.def'
$x86DefinitionLines = [Collections.Generic.List[string]]::new()
foreach ($line in Get-Content -LiteralPath $definition) {
    $x86DefinitionLines.Add($line)
}
$x86DefinitionLines.Add('')
$x86DefinitionLines.Add('GetCommHandle=GetCommHandle@4')
$x86DefinitionLines.Add('GetCommShadowMSR=GetCommShadowMSR@4')
[IO.File]::WriteAllLines($x86Definition, $x86DefinitionLines,
    [Text.UTF8Encoding]::new($false))

# Modern SDK import libraries intentionally omit several still-exported x86
# WOW compatibility entrypoints.  Keep the dependency explicit and build-local:
# these .def files must spell the names exported by the running product's
# SysWOW64 system DLLs, and the generated import libraries contain no provider
# implementation.  The original WOW32 C bodies retain their stdcall-decorated
# COFF references, so a separate build-local alias unit maps those references
# to the raw loader-visible spellings without changing source ABI or bodies.
$legacyUser32Definition = Join-Path $build 'legacy-wow-user32.def'
$legacyGdi32Definition = Join-Path $build 'legacy-wow-gdi32.def'
$legacyKernel32Definition = Join-Path $build 'legacy-wow-kernel32.def'
$legacyNtdllDefinition = Join-Path $build 'legacy-wow-ntdll.def'
$systemImportAliases = @(
    [pscustomobject]@{ Decorated = 'SetCursorContents@8'; Raw = 'SetCursorContents' },
    [pscustomobject]@{ Decorated = 'RegisterWowBaseHandlers@4'; Raw = 'RegisterWowBaseHandlers' },
    [pscustomobject]@{ Decorated = 'CsrIdentifyAlertableThread@0'; Raw = 'CsrIdentifyAlertableThread' },
    [pscustomobject]@{ Decorated = 'NtClose@4'; Raw = 'NtClose' },
    [pscustomobject]@{ Decorated = 'NtOpenThread@16'; Raw = 'NtOpenThread' },
    [pscustomobject]@{ Decorated = 'NtQueryInformationProcess@20'; Raw = 'NtQueryInformationProcess' },
    [pscustomobject]@{ Decorated = 'NtQueryInformationThread@20'; Raw = 'NtQueryInformationThread' },
    [pscustomobject]@{ Decorated = 'NtQueryPerformanceCounter@8'; Raw = 'NtQueryPerformanceCounter' },
    [pscustomobject]@{ Decorated = 'NtQuerySystemInformation@16'; Raw = 'NtQuerySystemInformation' },
    [pscustomobject]@{ Decorated = 'NtWaitForSingleObject@12'; Raw = 'NtWaitForSingleObject' },
    [pscustomobject]@{ Decorated = 'RtlAdjustPrivilege@16'; Raw = 'RtlAdjustPrivilege' },
    [pscustomobject]@{ Decorated = 'RtlCharToInteger@12'; Raw = 'RtlCharToInteger' },
    [pscustomobject]@{ Decorated = 'RtlDeleteCriticalSection@4'; Raw = 'RtlDeleteCriticalSection' },
    [pscustomobject]@{ Decorated = 'RtlEnterCriticalSection@4'; Raw = 'RtlEnterCriticalSection' },
    [pscustomobject]@{ Decorated = 'RtlEqualUnicodeString@12'; Raw = 'RtlEqualUnicodeString' },
    [pscustomobject]@{ Decorated = 'RtlFreeAnsiString@4'; Raw = 'RtlFreeAnsiString' },
    [pscustomobject]@{ Decorated = 'RtlImageNtHeader@4'; Raw = 'RtlImageNtHeader' },
    [pscustomobject]@{ Decorated = 'RtlInitUnicodeString@8'; Raw = 'RtlInitUnicodeString' },
    [pscustomobject]@{ Decorated = 'RtlInitializeCriticalSection@4'; Raw = 'RtlInitializeCriticalSection' },
    [pscustomobject]@{ Decorated = 'RtlLeaveCriticalSection@4'; Raw = 'RtlLeaveCriticalSection' },
    [pscustomobject]@{ Decorated = 'RtlMultiByteToUnicodeN@20'; Raw = 'RtlMultiByteToUnicodeN' },
    [pscustomobject]@{ Decorated = 'RtlRunEncodeUnicodeString@8'; Raw = 'RtlRunEncodeUnicodeString' },
    [pscustomobject]@{ Decorated = 'RtlRunDecodeUnicodeString@8'; Raw = 'RtlRunDecodeUnicodeString' },
    [pscustomobject]@{ Decorated = 'RtlUnicodeStringToAnsiString@12'; Raw = 'RtlUnicodeStringToAnsiString' },
    [pscustomobject]@{ Decorated = 'RtlUnicodeToMultiByteN@20'; Raw = 'RtlUnicodeToMultiByteN' },
    [pscustomobject]@{ Decorated = 'DbgBreakPoint@0'; Raw = 'DbgBreakPoint' }
)
@('LIBRARY USER32.DLL', 'EXPORTS', '    SetCursorContents') |
    Set-Content -LiteralPath $legacyUser32Definition -Encoding ascii
@('LIBRARY KERNEL32.DLL', 'EXPORTS',
  '    RegisterWowBaseHandlers') |
    Set-Content -LiteralPath $legacyKernel32Definition -Encoding ascii
@('LIBRARY NTDLL.DLL', 'EXPORTS',
  '    CsrIdentifyAlertableThread',
  '    NtClose', '    NtOpenThread',
  '    NtQueryInformationProcess', '    NtQueryInformationThread', '    NtQueryValueKey',
  '    NtQueryPerformanceCounter', '    NtQuerySystemInformation',
  '    NtWaitForSingleObject', '    RtlAdjustPrivilege', '    RtlCharToInteger',
  '    RtlDeleteCriticalSection', '    RtlEnterCriticalSection',
  '    RtlEqualUnicodeString',
  '    RtlFreeAnsiString', '    RtlImageNtHeader', '    RtlInitUnicodeString',
  '    RtlInitializeCriticalSection', '    RtlLeaveCriticalSection',
  '    RtlMultiByteToUnicodeN', '    RtlUnicodeStringToAnsiString',
  '    RtlUnicodeToMultiByteSize', '    RtlIsNameLegalDOS8Dot3',
  '    RtlRunEncodeUnicodeString', '    RtlRunDecodeUnicodeString',
  '    RtlUnicodeToMultiByteN', '    RtlUnicodeStringToInteger', '    RtlIntegerToUnicodeString', '    RtlRaiseStatus', '    DbgBreakPoint') |
    Set-Content -LiteralPath $legacyNtdllDefinition -Encoding ascii
$systemImportAliasSource = Join-Path $build 'wow32-system-import-abi-aliases.c'
$systemImportAliases += [pscustomobject]@{ Decorated = 'RtlUnicodeStringToInteger@12'; Raw = 'RtlUnicodeStringToInteger' }
$systemImportAliases += [pscustomobject]@{ Decorated = 'RtlIntegerToUnicodeString@12'; Raw = 'RtlIntegerToUnicodeString' }
$systemImportAliases += [pscustomobject]@{ Decorated = 'NtQueryValueKey@24'; Raw = 'NtQueryValueKey' }
$systemImportAliases += [pscustomobject]@{ Decorated = 'RtlRaiseStatus@4'; Raw = 'RtlRaiseStatus' }
$systemImportAliases += [pscustomobject]@{ Decorated = 'RtlUnicodeToMultiByteSize@12'; Raw = 'RtlUnicodeToMultiByteSize' }
$systemImportAliases += [pscustomobject]@{ Decorated = 'RtlIsNameLegalDOS8Dot3@12'; Raw = 'RtlIsNameLegalDOS8Dot3' }
$systemImportAliasLines = [Collections.Generic.List[string]]::new()
$systemImportAliasLines.Add('/* Generated build-only COFF aliases; no provider implementation. */')
foreach ($alias in $systemImportAliases) {
    $systemImportAliasLines.Add('#pragma comment(linker, "/alternatename:_' + $alias.Decorated + '=_' + $alias.Raw + '")')
    $systemImportAliasLines.Add('#pragma comment(linker, "/alternatename:__imp__' + $alias.Decorated + '=__imp__' + $alias.Raw + '")')
}
[IO.File]::WriteAllLines($systemImportAliasSource, $systemImportAliasLines,
    [Text.UTF8Encoding]::new($false))

# Original WOW32 calls its NTVDM/OEMUNI import surface with NT4 COFF names
# (`_Function@N`).  The standalone parent's import library deliberately
# exposes the loader names (for example, `ExitVDM` and `__imp_ExitVDM`).
# Bind both possible original COFF references to those actual import-library
# names; this supplies no function body or policy.
$parentImportAliasSource = Join-Path $build 'wow32-parent-import-abi-aliases.c'
$parentImportAliasLines = [Collections.Generic.List[string]]::new()
$parentImportAliasLines.Add('/* Generated build-only aliases to the parent NTVDM import surface. */')
foreach ($alias in @(
    [pscustomobject]@{ Decorated = 'opennt_support_current_teb@0'; Raw = 'opennt_support_current_teb' },
    [pscustomobject]@{ Decorated = 'NtCurrentPeb@0'; Raw = 'NtCurrentPeb' },
    [pscustomobject]@{ Decorated = 'ExitVDM@8'; Raw = 'ExitVDM' },
    [pscustomobject]@{ Decorated = 'OpenNtBaseClientWowStarted@4'; Raw = 'OpenNtBaseClientWowStarted' },
    [pscustomobject]@{ Decorated = 'OpenNtBaseClientBindWowTask@12'; Raw = 'OpenNtBaseClientBindWowTask' },
    [pscustomobject]@{ Decorated = 'OpenNtBaseClientRetireWowTask@8'; Raw = 'OpenNtBaseClientRetireWowTask' },
    [pscustomobject]@{ Decorated = 'OpenNtBaseClientFailWowTaskClose@8'; Raw = 'OpenNtBaseClientFailWowTaskClose' },
    [pscustomobject]@{ Decorated = 'NtvdmAuthorizeExit@4'; Raw = 'NtvdmAuthorizeExit' },
    [pscustomobject]@{ Decorated = 'host_CreateThread@24'; Raw = 'host_CreateThread' },
    [pscustomobject]@{ Decorated = 'host_ExitThread@4'; Raw = 'host_ExitThread' },
    [pscustomobject]@{ Decorated = 'opennt_exit_thread@4'; Raw = 'opennt_exit_thread' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_current@0'; Raw = 'wow_user_runtime_current' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_enter@4'; Raw = 'wow_user_runtime_enter' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_leave@4'; Raw = 'wow_user_runtime_leave' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_leave_for_callout@4'; Raw = 'wow_user_runtime_leave_for_callout' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_reenter_after_callout@4'; Raw = 'wow_user_runtime_reenter_after_callout' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_set_context@12'; Raw = 'wow_user_runtime_set_context' },
    [pscustomobject]@{ Decorated = 'wow_user_worker_active@0'; Raw = 'wow_user_worker_active' },
    # These parent exports are cdecl worker bodies. The original WOW32 DLL
    # references them under /Gz, so its @0 names must weak-alias to the
    # cdecl import-library spelling (including its leading underscore).
    [pscustomobject]@{ Decorated = 'mvdm_softpc_fast_bop_dispatch_offset@0'; Raw = '_mvdm_softpc_fast_bop_dispatch_offset' },
    [pscustomobject]@{ Decorated = 'mvdm_softpc_fast_bop_callback_offset@0'; Raw = '_mvdm_softpc_fast_bop_callback_offset' },
    [pscustomobject]@{ Decorated = 'OpenNtRtlNtStatusToDosError@4'; Raw = 'OpenNtRtlNtStatusToDosError' },
    [pscustomobject]@{ Decorated = 'GetNextVDMCommand@4'; Raw = 'GetNextVDMCommand' },
    [pscustomobject]@{ Decorated = 'RegisterWowExec@4'; Raw = 'RegisterWowExec' },
    [pscustomobject]@{ Decorated = 'GetCurrentDirectoryOem@8'; Raw = 'GetCurrentDirectoryOem' },
    [pscustomobject]@{ Decorated = 'SetCurrentDirectoryOem@4'; Raw = 'SetCurrentDirectoryOem' },
    [pscustomobject]@{ Decorated = 'GetEnvironmentVariableOem@12'; Raw = 'GetEnvironmentVariableOem' },
    [pscustomobject]@{ Decorated = 'SetEnvironmentVariableOem@8'; Raw = 'SetEnvironmentVariableOem' },
    [pscustomobject]@{ Decorated = 'SetFileAttributesOem@8'; Raw = 'SetFileAttributesOem' },
    [pscustomobject]@{ Decorated = 'GetFileAttributesOem@4'; Raw = 'GetFileAttributesOem' },
    [pscustomobject]@{ Decorated = 'DeleteFileOem@4'; Raw = 'DeleteFileOem' },
    [pscustomobject]@{ Decorated = 'MoveFileOem@8'; Raw = 'MoveFileOem' },
    [pscustomobject]@{ Decorated = 'MoveFileExOem@12'; Raw = 'MoveFileExOem' },
    [pscustomobject]@{ Decorated = 'GetFullPathNameOem@16'; Raw = 'GetFullPathNameOem' },
    [pscustomobject]@{ Decorated = 'GetTempFileNameOem@16'; Raw = 'GetTempFileNameOem' },
    [pscustomobject]@{ Decorated = 'RemoveFontResourceOem@4'; Raw = 'RemoveFontResourceOem' },
    [pscustomobject]@{ Decorated = 'CreateFileOem@28'; Raw = 'CreateFileOem' },
    [pscustomobject]@{ Decorated = 'GetVolumeInformationOem@32'; Raw = 'GetVolumeInformationOem' }
)) {
    $parentImportAliasLines.Add('#pragma comment(linker, "/alternatename:_' + $alias.Decorated + '=' + $alias.Raw + '")')
    $parentImportAliasLines.Add('#pragma comment(linker, "/alternatename:__imp__' + $alias.Decorated + '=__imp_' + $alias.Raw + '")')
}
# These three worker-local GDI aliases are cdecl rather than the original
# WOW32 stdcall import family above.  The parent .def deliberately exports
# their loader-visible names while binding them to x86 C bodies.  Supply the
# matching COFF aliases here; this is ABI glue only, not a provider body.
foreach ($raw in @(
    'mvdm_wow_gdi_handle_from_native',
    'mvdm_wow_gdi_handle_to_native',
    'mvdm_wow_gdi_handle_retire',
    'mvdm_softpc_system_find_file',
    'mvdm_softpc_profile_shadow_system_ini',
    'mvdm_softpc_system_copy_root',
    'mvdm_softpc_system_copy_system_directory',
    'NtvdmGetWindowsDirectoryA',
    'NtvdmGetSystemDirectoryA',
    'NtvdmGetWindowsDirectoryW',
    'NtvdmGetSystemDirectoryW',
    'mvdm_softpc_wow_page_domain_publish_system_color'
)) {
    $parentImportAliasLines.Add('#pragma comment(linker, "/alternatename:_' + $raw + '=' + $raw + '")')
    $parentImportAliasLines.Add('#pragma comment(linker, "/alternatename:__imp__' + $raw + '=__imp_' + $raw + '")')
}
[IO.File]::WriteAllLines($parentImportAliasSource, $parentImportAliasLines,
    [Text.UTF8Encoding]::new($false))

# The historical build manifest supplies DLLENTRY=W32DllInitialize.  A modern
# /MT DLL instead enters __DllMainCRTStartup and otherwise uses its empty
# default DllMain stub, which leaves the original process-attach heap and
# hook initialization unrun.  Preserve CRT startup and generate only the
# ABI-equivalent DllMain forwarding bridge; do not bypass CRT with /ENTRY and
# do not alter the original WOW32 body.
$dllEntryBridgeSource = Join-Path $build 'wow32-dll-entry-bridge.c'
$dllEntryBridgeLines = @(
    '/* Generated build-only CRT-to-original DLLENTRY bridge. */',
    'extern unsigned char __stdcall W32DllInitialize(void *, unsigned long, void *);',
    'extern void __stdcall wow_bitmap_bindings_shutdown(void);',
    'extern void __stdcall wow_registry_binding_shutdown(void);',
    'int __stdcall DllMain(void *instance, unsigned long reason, void *reserved)',
    '{',
    '    int result = W32DllInitialize(instance, reason, reserved);',
    '    if (reason == 0 && reserved == 0) { wow_registry_binding_shutdown(); wow_bitmap_bindings_shutdown(); }',
    '    return result;',
    '}'
)
[IO.File]::WriteAllLines($dllEntryBridgeSource, $dllEntryBridgeLines,
    [Text.UTF8Encoding]::new($false))

$environment = Join-Path $build 'msvc-x86.cmd'
@(
    '@echo off',
    'set "MVDM_T404_S5_CALLER_CWD=%CD%"',
    'if defined VSCMD_VER goto ready',
    ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%',
    ':ready',
    'cd /d "%MVDM_T404_S5_CALLER_CWD%"',
    '%*',
    'exit /b %errorlevel%'
) | Set-Content -LiteralPath $environment -Encoding ascii

$includeRoots = @(
    'src',
    'src/opennt-abi/host-compat/include',
    'src/wow32-dll/include',
    'src/ntvdm-exe/wow/include',
    'src/mvdm/wow32',
    'src/mvdm/inc',
    'src/mvdm/softpc.new/base/inc',
    'src/mvdm/softpc.new/host/inc',
    'src/opennt-host/public/sdk/inc',
    'src/opennt-abi/source/public/sdk/inc',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/internal/windows/inc',
    'src/opennt-abi/source/public/internal/shell/inc',
    'src/opennt-abi/source/private/windows/inc',
    'src/ntvdm-exe/softpc/include',
    'src/ntvdm-exe/command/include',
    'src/ntvdm-exe/monitor/include',
    'src/ntvdm-exe/vdd/include',
    'src/ntvdm-exe/session'
)
$includes = ($includeRoots | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }) -join ' '
$force = '/FI "' + (Join-Path $root 'src/opennt-abi/host-compat/include/nt.h') + '"'
# Original NT x86 i386mk.inc defaults to /Gz. Keep this DLL's
# internal ABI; worker-owned cdecl imports need explicit declarations, not a
# package-wide /Gd override or aliases that conceal different stack cleanup.
$cflags = '/nologo /TC /c /MT /W3 /Gz /showIncludes /D_NO_CRT_STDIO_INLINE /DWIN32 /DWINNT /DNTVDM /DCPU_40_STYLE /DNEW_CPU /DCCPU /DC_VID /DSPC386 /DSIM32 /DV7VGA /DANSI /DPROD ' + $force + ' ' + $includes
$adapterCflags = '/nologo /TC /c /MT /W3 /Gz /showIncludes /D_NO_CRT_STDIO_INLINE /DWIN32 /DWINNT /DNTVDM /DANSI ' + $force + ' ' + $includes

$ninja = [System.Collections.Generic.List[string]]::new()
$ninja.Add('ninja_required_version = 1.10')
$ninja.Add('root = ' + (ConvertTo-NinjaPath $root))
$ninja.Add('environment = ' + (ConvertTo-NinjaPath $environment))
$ninja.Add('release_cflags = /O2')
$ninja.Add('cflags = ' + $cflags.Replace('\', '/'))
$ninja.Add('adapter_cflags = ' + $adapterCflags.Replace('\', '/'))
# OpenNT ntuser/client/sources.inc and rtl/client/sources.inc select UNICODE.
# Keep the recovered USER island distinct from original WOW32's ANSI graph.
$ninja.Add('control_user_flags = /UANSI /DUNICODE')
$ninja.Add('rule cc')
$ninja.Add('  command = ' + $compilerCommand + ' $cflags $release_cflags /Fo$out $in')
$ninja.Add('  deps = msvc')
$ninja.Add('  description = CC $in')
$ninja.Add('rule rc')
$ninja.Add('  command = rc /nologo /fo$out $in')
$ninja.Add('  description = RC $in')
$ninja.Add('rule legacy_import_lib')
$ninja.Add('  command = lib /nologo /machine:x86 /def:$in /out:$out')
$ninja.Add('  description = IMPORTLIB $out')
$ninja.Add('rule wow_dll_link')
$ninja.Add('  command = link /nologo /machine:x86 /dll /map:$out.map /def:' + (ConvertTo-NinjaPath $x86Definition) + ' /out:$out $in ' + (ConvertTo-NinjaPath $parentImportLibraryPath) + ' kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib comdlg32.lib version.lib winmm.lib wsock32.lib oldnames.lib')
$ninja.Add('  description = LINK $out')
$ninja.Add('  restat = 1')

$objects = [System.Collections.Generic.List[string]]::new()
foreach ($source in $sources) {
    $object = 'obj/' + [IO.Path]::GetFileNameWithoutExtension($source) + '.obj'
    $objects.Add($object)
    $ninja.Add('build ' + $object + ': cc ' + (ConvertTo-NinjaPath (Join-Path $wow $source)))
    if ($source -eq 'wow32.c') {
        $ninja.Add('  cflags = $cflags /DWOWSysErrorBox=wow_user_sys_error_box')
    } elseif ($source -eq 'wkman.c') {
        $ninja.Add('  cflags = $cflags /DWOW_NATIVE_PROCESS_BINDING /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/wow_native_process.h')) + '"')
    } elseif ($source -eq 'wudlg.c') {
        $ninja.Add('  cflags = $cflags /DWOWSysErrorBox=wow_user_sys_error_box')
    } elseif ($source -eq 'wcommdlg.c') {
        $ninja.Add('  cflags = $cflags /DWOW_COMMON_DIALOG_BINDING /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/wow_common_dialog_calls.h')) + '"')
    } elseif ($source -in @('wuser.c','wuser31.c','wuwind.c')) {
        $hardErrorBinding = if ($source -eq 'wuser.c') {' /DWOWSysErrorBox=wow_user_sys_error_box'} else {''}
        $ninja.Add('  cflags = $cflags' + $hardErrorBinding)
    } elseif ($source -eq 'wkgthunk.c') {
        $ninja.Add('  cflags = $cflags /DWK32ICallProc32=wow_original_WK32ICallProc32 /DWOWYield16=wow_original_WOWYield16 /DWOWDirectedYield16=wow_original_WOWDirectedYield16 /DWOW_NATIVE_EXTENSION_BINDING /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/wow_user_native_extension.h')) + '"')
    } elseif ($source -eq 'wmmedia.c') {
        $ninja.Add('  cflags = $cflags /DWMM32CallProc32=wow_original_WMM32CallProc32')
    } elseif ($source -eq 'wshell.c') {
        $ninja.Add('  cflags = $cflags /DWOWShellExecute=wow_shell_execute /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/wow_registry_binding.h')) + '"')
    } elseif ($source -in @('wkernel.c','mapembed.c','wdde.c','wow32fax.c')) {
        $ninja.Add('  cflags = $cflags /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/wow_registry_binding.h')) + '"')
    }
}
$supportObjects = [System.Collections.Generic.List[string]]::new()
# T447 S3: compose the recovered collaboration cohort, not just its adapters.
# These bodies already share the audited control-client facade in the whole
# cohort verification graph. Distinct path-qualified object names prevent
# collisions with the original MVDM thunk objects and the existing subsets.
foreach ($controlBody in @('editec','edecrare','editsl','editml','edmlonce',
        'edmlrare','edslrare','combo','combodir','comboini','lb1','lboxctl1',
        'lboxctl2','lboxctl3','lboxmult','lboxrare','lboxvar','dlgbegin',
        'dlgend','dlgmgr','dlgmgrc','dlgmgr2','dmmnem','wow','winmgrc')) {
    $controlBodyPath = Join-Path $root ('src/opennt-host/windows/core/ntuser/client/' + $controlBody + '.c')
    if (!(Test-Path -LiteralPath $controlBodyPath -PathType Leaf)) {
        throw "Missing admitted original control body: $controlBodyPath"
    }
    $controlBodyObject = 'obj/opennt-user/control-body-' + $controlBody + '.obj'
    $objects.Add($controlBodyObject)
    $ninja.Add('build ' + $controlBodyObject + ': cc ' + (ConvertTo-NinjaPath $controlBodyPath))
    $bodyFlags = ' /DWOW_ORIGINAL_EDIT_CLIENT'
    if ($controlBody.StartsWith('dlg') -or $controlBody -eq 'dmmnem') {
        $bodyFlags += ' /DWOW_ORIGINAL_DIALOG_CLIENT /DWOW_ORIGINAL_DIALOG_MANAGER'
    }
    if ($controlBody -eq 'dlgmgr2') { $bodyFlags += ' /DWOW_ORIGINAL_DIALOG_KEYBOARD' }
    if ($controlBody -eq 'dlgend') { $bodyFlags += ' /DWOW_ORIGINAL_DIALOG_END' }
    if ($controlBody -eq 'dlgmgr') { $bodyFlags += ' /DWOW_ORIGINAL_DIALOG_DEFAULT' }
    $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags' + $bodyFlags)
}
foreach ($viewBody in @('winmgr','wow')) {
    $viewObject = 'obj/opennt-user/control-view-' + $viewBody + '.obj'
    $objects.Add($viewObject)
    $ninja.Add('build ' + $viewObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/opennt-host/windows/core/ntuser/rtl/' + $viewBody + '.c'))))
    $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT')
}
# Original USER owner, not an adapter implementation (OPENNT-HOST-032).
$ddeDataObject = 'obj/opennt-user/hdata.obj'
$objects.Add($ddeDataObject)
$ninja.Add('build ' + $ddeDataObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/hdata.c')))
foreach ($userOwner in @('clres', 'cldib', 'client', 'rtlinit', 'ntstubs', 'clmenu', 'clglobal')) {
    $object = 'obj/opennt-user/' + $userOwner + '.obj'
    $objects.Add($object)
    $ninja.Add('build ' + $object + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/opennt-host/windows/core/ntuser/client/' + $userOwner + '.c'))))
    if ($userOwner -eq 'clglobal') {
        # Original USER globals include WCHAR TEXT literals; retain that
        # unit's Unicode contract, distinct from the ANSI WOW thunk build.
        $ninja.Add('  cflags = $cflags $control_user_flags')
    }
}
$classLookupObject = 'obj/opennt-user/class.obj'
# rtlinit's existing capture implementation and original control allocator
# are disjoint registered subsets. Keep distinct object identities and retain
# the capture object above; changing its flags would remove that provider.
$controlLookasideObject = 'obj/opennt-user/control-lookaside.obj'
$objects.Add($controlLookasideObject)
$ninja.Add('build ' + $controlLookasideObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/rtlinit.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_LOOKASIDE')
# Original dialog initialization and shared EDIT DS callbacks use the same
# registered WOW table as LOCAL*. Do not replace these bodies in an adapter.
$dialogMemoryObject = 'obj/opennt-user/dialog-memory.obj'
# OPENNT-HOST-079: original modal template entry, separate from clres's
# bitmap slice. The existing modal entry owns context/task/borrow lifetimes.
$dialogTemplateObject = 'obj/opennt-user/dialog-template.obj'
$objects.Add($dialogTemplateObject)
$ninja.Add('build ' + $dialogTemplateObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/clres.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_DIALOG_CLIENT /DWOW_ORIGINAL_DIALOG_MANAGER /DWOW_ORIGINAL_DIALOG_TEMPLATE')
$objects.Add($dialogMemoryObject)
$ninja.Add('build ' + $dialogMemoryObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/client.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_DIALOG_EDIT')
# Original state transitions, not replacements in the binding units. The
# kernel ntstubs subset has its registered receive facade in the source;
# miscutil keeps precomp.h and receives only its finite original layouts.
$controlFnidObject = 'obj/opennt-user/control-fnid.obj'
$controlTextObject = 'obj/opennt-user/control-text.obj'
$objects.Add($controlTextObject)
$ninja.Add('build ' + $controlTextObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/ntstubs.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_CONTROL_TEXT')
$controlColorObject = 'obj/opennt-user/control-color.obj'
$objects.Add($controlColorObject)
$ninja.Add('build ' + $controlColorObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/ntstubs.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_CONTROL_COLOR')
$controlBrushObject = 'obj/opennt-user/control-brush.obj'
$controlBrushClientObject = 'obj/opennt-user/control-brush-client.obj'
$objects.Add($controlBrushClientObject)
$ninja.Add('build ' + $controlBrushClientObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/ntstubs.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_CONTROL_BRUSH_CLIENT')
$objects.Add($controlBrushObject)
$ninja.Add('build ' + $controlBrushObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/random.c')))
$ninja.Add('  cflags = $cflags $control_user_flags')
$controlStateObject = 'obj/opennt-user/control-state.obj'
$objects.Add($controlStateObject)
$ninja.Add('build ' + $controlStateObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/client.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_CONTROL_STATE')
$objects.Add($controlFnidObject)
$ninja.Add('build ' + $controlFnidObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/ntstubs.c')))
$ninja.Add('  cflags = $cflags $control_user_flags')
$dialogPointerObject = 'obj/opennt-user/dialog-pointer.obj'
$objects.Add($dialogPointerObject)
$ninja.Add('build ' + $dialogPointerObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/miscutil.c')))
# This must precede global include roots: MVDM also has a precomp.h, whose
# declarations belong to a different translation-unit contract.
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-state')) + '" $cflags $control_user_flags')
$controlRectObject = 'obj/opennt-user/control-rect.obj'
$objects.Add($controlRectObject)
$ninja.Add('build ' + $controlRectObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/rtl/rect.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-state')) + '" $cflags $control_user_flags')
$controlDrawObject = 'obj/opennt-user/control-draw.obj'
$defaultMessagesObject = 'obj/opennt-user/default-messages.obj'
$defaultClientObject = 'obj/opennt-user/default-client.obj'
$objects.Add($defaultClientObject)
$ninja.Add('build ' + $defaultClientObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/clmsg.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_DEFAULT_WINDOW')
$defaultBitsObject = 'obj/opennt-user/default-window-bits.obj'
$clientCpdObject = 'obj/opennt-user/client-callproc.obj'
$privateWordSetterObject = 'obj/opennt-user/private-word-setter.obj'
# The public private-word access facade calls these original client owners.
# Keep reader/client/server as distinct objects: selecting only the server
# leaves _GetWindowLong/_SetWindowLong unresolved in the product provider.
foreach ($accessOwner in @(
    @('private-word-reader', 'rtl/getset.c', '/DWOW_ORIGINAL_PRIVATE_WORD_GETTER'),
    @('private-word-client', 'client/ntstubs.c', '/DWOW_ORIGINAL_DIALOG_GETSET')
)) {
    $accessObject = 'obj/opennt-user/' + $accessOwner[0] + '.obj'
    $objects.Add($accessObject)
    $ninja.Add('build ' + $accessObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/opennt-host/windows/core/ntuser/' + $accessOwner[1]))))
    $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT ' + $accessOwner[2])
}
$objects.Add($privateWordSetterObject)
$ninja.Add('build ' + $privateWordSetterObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/getset.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_PRIVATE_WORD_SETTER')
$objects.Add($clientCpdObject)
$ninja.Add('build ' + $clientCpdObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/clmsg.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_CLIENT_CPD')
$objects.Add($defaultBitsObject)
$ninja.Add('build ' + $defaultBitsObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/client.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_DEF_WINDOW_BITS')
$defaultBordersObject = 'obj/opennt-user/default-borders.obj'
$objects.Add($defaultBordersObject)
$ninja.Add('build ' + $defaultBordersObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/rtl/winmgr.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-geometry')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_DEFAULT_GEOMETRY')
$objects.Add($defaultMessagesObject)
$ninja.Add('build ' + $defaultMessagesObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/server.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_DEFAULT_MESSAGES')
$objects.Add($controlDrawObject)
$ninja.Add('build ' + $controlDrawObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/rtl/draw.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-drawing')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_BUTTON_DRAWING')
foreach ($textPart in @('client', 'rtl')) {
    $textObject = 'obj/opennt-user/control-text-' + $textPart + '.obj'
    $objects.Add($textObject)
    $ninja.Add('build ' + $textObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/opennt-host/windows/core/ntuser/' + $textPart + '/drawtext.c'))))
    $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-drawing')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_MNEMONIC_DRAWING')
}
# T447 S3 original painting dependencies, not a substitute button WndProc.
$extentObject = 'obj/opennt-user/control-text-extent.obj'
$objects.Add($extentObject)
$ninja.Add('build ' + $extentObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/rtl/text.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-drawing')) + '" $cflags $control_user_flags')
foreach ($drawingPart in @('draw', 'mngrayc')) {
    $drawingObject = 'obj/opennt-user/control-client-' + $drawingPart + '.obj'
    $objects.Add($drawingObject)
    $ninja.Add('build ' + $drawingObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/opennt-host/windows/core/ntuser/client/' + $drawingPart + '.c'))))
    if ($drawingPart -eq 'mngrayc') {
        $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-draw-state')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_STATE_DRAWING /DWOW_ORIGINAL_BUTTON_ATLAS')
    } else {
        $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT')
    }
}
$atlasTableObject = 'obj/opennt-user/button-atlas-table.obj'
$buttonObject = 'obj/opennt-user/button-client.obj'
$objects.Add($buttonObject)
$ninja.Add('build ' + $buttonObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/btnctl.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_BUTTON_CLIENT')
$buttonBlitObject = 'obj/opennt-user/button-blit.obj'
$objects.Add($buttonBlitObject)
$ninja.Add('build ' + $buttonBlitObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/mngrayc.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/button-drawing')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_BUTTON_ATLAS /DWOW_ORIGINAL_BUTTON_BLIT_ONLY')
$objects.Add($atlasTableObject)
$ninja.Add('build ' + $atlasTableObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/inctlpan.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/include/button-resources')) + '" $cflags $control_user_flags')
  $probeObject = 'obj/opennt-user/probe.obj'
  $cleanupObject = 'obj/opennt-user/cleanup.obj'
  $handleFlagsObject = 'obj/opennt-user/handtabl.obj'
  $objects.Add($handleFlagsObject)
  $ninja.Add('build ' + $handleFlagsObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/handtabl.c')))
  $objects.Add($cleanupObject)
  $ninja.Add('build ' + $cleanupObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/cleanup.c')))
$objects.Add($probeObject)
$ninja.Add('build ' + $probeObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/base/ntos/ex/probe.c')))
$classRegisterObject = 'obj/opennt-user/class-register.obj'
$classRemoveObject = 'obj/opennt-user/class-remove.obj'
$objects.Add($classRemoveObject)
$ninja.Add('build ' + $classRemoveObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/class.c')))
$ninja.Add('  cflags = $cflags /DWOW_ORIGINAL_CLASS_REMOVAL')
$objects.Add($classRegisterObject)
$ninja.Add('build ' + $classRegisterObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/class.c')))
$ninja.Add('  cflags = $cflags /DWOW_ORIGINAL_CLASS_REGISTRATION')
$taskOrderObject = 'obj/opennt-user/taskman.obj'
$objects.Add($taskOrderObject)
$ninja.Add('build ' + $taskOrderObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/taskman.c')))
$classClientObject = 'obj/opennt-user/class-client.obj'
$inputGroupObject = 'obj/opennt-user/input.obj'
$objects.Add($inputGroupObject)
$ninja.Add('build ' + $inputGroupObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/input.c')))
$taskInitObject = 'obj/opennt-user/queue.obj'
$profileObject = 'obj/opennt-user/profile.obj'
$objects.Add($profileObject)
$ninja.Add('build ' + $profileObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/profile.c')))
$objects.Add($taskInitObject)
$ninja.Add('build ' + $taskInitObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/queue.c')))
$taskRegisterObject = 'obj/opennt-user/exitwin.obj'
$objects.Add($taskRegisterObject)
$ninja.Add('build ' + $taskRegisterObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/exitwin.c')))
$objects.Add($classClientObject)
$ninja.Add('build ' + $classClientObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/client.c')))
$ninja.Add('  cflags = $cflags /DWOW_ORIGINAL_CLASS_CLIENT')
$objects.Add($classLookupObject)
$ninja.Add('build ' + $classLookupObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/class.c')))
foreach ($source in $providerSupportSources) {
    $owner = if ($source.StartsWith($adapterWow, [StringComparison]::OrdinalIgnoreCase)) { 'wow32-dll' } else { 'adapter-win32' }
    $object = 'obj/' + $owner + '/' + [IO.Path]::GetFileNameWithoutExtension($source) + '.obj'
    $supportObjects.Add($object)
    $ninja.Add('build ' + $object + ': cc ' + (ConvertTo-NinjaPath $source))
    if ($source.StartsWith($adapterWow, [StringComparison]::OrdinalIgnoreCase) -or
        $source.StartsWith((Join-Path $root 'src/ntvdm-exe'), [StringComparison]::OrdinalIgnoreCase)) {
        $ninja.Add('  cflags = $adapter_cflags')
    }
}
$systemImportAliasObject = 'obj/wow32-system-import-abi-aliases.obj'
$ninja.Add('build ' + $systemImportAliasObject + ': cc ' + (ConvertTo-NinjaPath $systemImportAliasSource))
$ninja.Add('  cflags = $adapter_cflags')
$parentImportAliasObject = 'obj/wow32-parent-import-abi-aliases.obj'
$ninja.Add('build ' + $parentImportAliasObject + ': cc ' + (ConvertTo-NinjaPath $parentImportAliasSource))
$ninja.Add('  cflags = $adapter_cflags')
$dllEntryBridgeObject = 'obj/wow32-dll-entry-bridge.obj'
$ninja.Add('build ' + $dllEntryBridgeObject + ': cc ' + (ConvertTo-NinjaPath $dllEntryBridgeSource))
$ninja.Add('  cflags = $adapter_cflags')
$ninja.Add('build obj/wow32.res: rc ' + (ConvertTo-NinjaPath $resource))
$editMenuResource = 'obj/opennt-user/edit-menu.res'
$objects.Add($editMenuResource)
$ninja.Add('build ' + $editMenuResource + ': rc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/opennt-host/windows/core/ntuser/client/res.rc')))
$ninja.Add('build legacy-wow-user32.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyUser32Definition))
$ninja.Add('build legacy-wow-kernel32.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyKernel32Definition))
$ninja.Add('build legacy-wow-ntdll.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyNtdllDefinition))
$ninja.Add('build wow32.dll | wow32.lib: wow_dll_link ' + (($objects + $supportObjects + @($systemImportAliasObject, $parentImportAliasObject, $dllEntryBridgeObject, 'obj/wow32.res', 'legacy-wow-user32.lib', 'legacy-wow-kernel32.lib', 'legacy-wow-ntdll.lib')) -join ' ') + ' | ' + (ConvertTo-NinjaPath $parentImportLibraryPath))
$ninja.Add('default wow32.dll')

[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $ninja, [Text.UTF8Encoding]::new($false))
Write-Host "Generated T404 S5 WOW32 $Architecture provider graph: $build (bodies=$($sources.Count); carriers=$($headers.Count + 1); jobs=$ParallelJobs)"
