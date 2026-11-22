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
    if ($unique.Count -ne 72) {
        throw "WOW32 provider must select exactly 72 C bodies; found $($unique.Count)."
    }
    return $unique
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$wow = Join-Path $root 'src/mvdm-host/wow32'
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
    (Join-Path $root 'src/compat/wow_user_registration_state.c'),
    (Join-Path $root 'src/compat/wow_callback_frame_lease.c'),
    (Join-Path $root 'src/compat/wow_private_user_compat.c'),
    (Join-Path $root 'src/compat/wow_bitmap_bindings.c'),
    (Join-Path $root 'src/compat/wow_window_words_binding.c'),
    (Join-Path $root 'src/compat/wow_window_procedure_entry.c'),
    (Join-Path $root 'src/compat/wow_user_object_bindings.c'),
    (Join-Path $root 'src/compat/wow_user_discovery.c'),
    (Join-Path $root 'src/compat/wow_menu_client_bindings.c'),
    (Join-Path $root 'src/compat/wow_menu_native_bindings.c'),
    (Join-Path $root 'src/compat/wow_cleanup_bindings.c'),
    (Join-Path $root 'src/compat/wow_user_borrow_scope.c'),
    (Join-Path $root 'src/compat/wow_class_client_bindings.c'),
    (Join-Path $root 'src/compat/wow_task_profile_binding.c'),
    (Join-Path $adapterWow 'wow_task_termination.c'),
    (Join-Path $adapterWow 'wow_task_parent.c'),
    (Join-Path $root 'src/compat/wow_user_message_bridge.c'),
    (Join-Path $root 'src/compat/wow_user_task_lifecycle.c'),
    (Join-Path $root 'src/compat/wow_task_start_notification.c'),
    (Join-Path $root 'src/compat/wow_input_group_lifetime.c'),
    (Join-Path $root 'src/compat/wow_dialog_creation_binding.c'),
    (Join-Path $root 'src/compat/wow_dialog_keyboard_binding.c'),
    # T447 S3 shared retained state and original-control dependency bindings.
    # These are one provider group, not optional native/recovered fallbacks.
    (Join-Path $root 'src/compat/wow_dialog_call_bindings.c'),
    (Join-Path $root 'src/compat/wow_dialog_procedure_bindings.c'),
    (Join-Path $root 'src/compat/wow_dialog_finalization_binding.c'),
    (Join-Path $root 'src/compat/wow_dialog_native_bindings.c'),
    (Join-Path $root 'src/compat/wow_dialog_state_bindings.c'),
    (Join-Path $root 'src/compat/wow_default_message_bindings.c'),
    (Join-Path $root 'src/compat/wow_default_native_bindings.c'),
    (Join-Path $root 'src/compat/wow_default_state_bindings.c'),
    (Join-Path $root 'src/compat/wow_default_window_bindings.c'),
    (Join-Path $root 'src/compat/wow_dialog_context_bindings.c'),
    (Join-Path $root 'src/compat/wow_edit_call_bindings.c'),
    (Join-Path $root 'src/compat/wow_control_brush_bindings.c'),
    (Join-Path $root 'src/compat/wow_edit_conversion_bindings.c'),
    (Join-Path $root 'src/compat/wow_control_draw_state.c'),
    (Join-Path $root 'src/compat/wow_button_resources.c'),
    (Join-Path $root 'src/compat/wow_button_lifetime.c'),
    (Join-Path $root 'src/compat/wow_edit_entry_bindings.c'),
    (Join-Path $root 'src/compat/wow_edit_lookaside_bindings.c'),
    (Join-Path $root 'src/compat/wow_edit_native_bindings.c'),
    (Join-Path $root 'src/compat/wow_list_native_bindings.c'),
    (Join-Path $root 'src/compat/wow_edit_state_bindings.c'),
    (Join-Path $root 'src/compat/wow_control_extent_bindings.c'),
    (Join-Path $root 'src/compat/wow_edit_thread_bindings.c'),
    (Join-Path $root 'src/compat/wow_window_client_access_bindings.c'),
    (Join-Path $root 'src/compat/wow_window_client_state_bindings.c'),
      (Join-Path $root 'src/compat/wow_hook_callback.c'),
    (Join-Path $root 'src/compat/wow_timer_callback.c'),
    (Join-Path $root 'src/compat/wow_idle_hook_binding.c'),
    (Join-Path $root 'src/compat/wow_shell_callback.c'),
    (Join-Path $root 'src/compat/wow_task_cleanup_entry.c'),
    (Join-Path $root 'src/compat/wow_common_dialog_callback.c'),
    (Join-Path $root 'src/compat/wow_common_dialog_calls.c'),
    (Join-Path $root 'src/compat/wow_user_thunk_scope.c'),
    (Join-Path $root 'src/compat/wow_user_native_extension.c'),
    (Join-Path $root 'src/compat/wow_user_private_access.c'),
    (Join-Path $root 'src/compat/wow_window_creation_binding.c'),
    (Join-Path $root 'src/compat/wow_class_words_binding.c'),
    (Join-Path $root 'src/compat/wow_clipboard_epoch.c'),
    (Join-Path $root 'src/compat/wow_ole_descriptor_binding.c'),
    (Join-Path $root 'src/compat/wow_standard_class_bindings.c'),
    (Join-Path $root 'src/compat/wow_user_registration_bridge.c'),
    (Join-Path $root 'src/non-mvdm/windows/core/ntuser/rtl/chartran.c'),
    (Join-Path $root 'src/non-mvdm/windows/core/ntuser/rtl/random.c'),
    (Join-Path $root 'src/non-mvdm/windows/core/ntuser/rtl/help.c'),
    # This is the already-admitted narrow historical CRT spelling bridge.  It
    # has no parent-machine state, so the late-loaded DLL may own its copy.
    (Join-Path $root 'src/lib-win32/crt-compat/crt_compat.c')
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
if ($headers.Count -ne 94) { throw "WOW32 must retain the selected 94 root headers; found $($headers.Count)." }

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
# two stdcall COMM exports. Add their x86 linker aliases and the worker
# management export to the generated product definition.
$x86Definition = Join-Path $build 'wow32-x86.def'
$x86DefinitionLines = [Collections.Generic.List[string]]::new()
foreach ($line in Get-Content -LiteralPath $definition) {
    $x86DefinitionLines.Add($line)
}
$x86DefinitionLines.Add('')
$x86DefinitionLines.Add('GetCommHandle=GetCommHandle@4')
$x86DefinitionLines.Add('GetCommShadowMSR=GetCommShadowMSR@4')
$x86DefinitionLines.Add('W32TerminateTaskTicket')
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
  '    NlsAnsiCodePage DATA', '    RtlUnicodeToMultiByteN', '    RtlUnicodeStringToInteger', '    RtlIntegerToUnicodeString', '    RtlRaiseStatus', '    DbgBreakPoint') |
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
    [pscustomobject]@{ Decorated = 'NtvdmReportWowStartup@12'; Raw = 'NtvdmReportWowStartup' },
    [pscustomobject]@{ Decorated = 'OpenNtBaseClientFindWowTask@12'; Raw = 'OpenNtBaseClientFindWowTask' },
    [pscustomobject]@{ Decorated = 'OpenNtBaseClientRetireWowTask@8'; Raw = 'OpenNtBaseClientRetireWowTask' },
    [pscustomobject]@{ Decorated = 'OpenNtBaseClientFailWowTaskClose@8'; Raw = 'OpenNtBaseClientFailWowTaskClose' },
    [pscustomobject]@{ Decorated = 'NtvdmAuthorizeExit@4'; Raw = 'NtvdmAuthorizeExit' },
    [pscustomobject]@{ Decorated = 'host_CreateThread@24'; Raw = 'host_CreateThread' },
    [pscustomobject]@{ Decorated = 'host_ExitThread@4'; Raw = 'host_ExitThread' },
    [pscustomobject]@{ Decorated = 'opennt_exit_thread@4'; Raw = 'opennt_exit_thread' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_open_key@12'; Raw = 'ntvdm_shadow_reg_open_key' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_create_key@12'; Raw = 'ntvdm_shadow_reg_create_key' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_close_key@4'; Raw = 'ntvdm_shadow_reg_close_key' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_delete_key@8'; Raw = 'ntvdm_shadow_reg_delete_key' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_set_value@20'; Raw = 'ntvdm_shadow_reg_set_value' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_set_value_ex@24'; Raw = 'ntvdm_shadow_reg_set_value_ex' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_query_value@16'; Raw = 'ntvdm_shadow_reg_query_value' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_query_value_ex@24'; Raw = 'ntvdm_shadow_reg_query_value_ex' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_enum_key@16'; Raw = 'ntvdm_shadow_reg_enum_key' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_open_key_ex@20'; Raw = 'ntvdm_shadow_reg_open_key_ex' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_query_info_key@48'; Raw = 'ntvdm_shadow_reg_query_info_key' },
    [pscustomobject]@{ Decorated = 'ntvdm_shadow_reg_enum_value@32'; Raw = 'ntvdm_shadow_reg_enum_value' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_current@0'; Raw = 'wow_user_runtime_current' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_unbind@4'; Raw = 'wow_user_runtime_unbind' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_enter@4'; Raw = 'wow_user_runtime_enter' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_leave@4'; Raw = 'wow_user_runtime_leave' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_leave_for_callout@4'; Raw = 'wow_user_runtime_leave_for_callout' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_reenter_after_callout@4'; Raw = 'wow_user_runtime_reenter_after_callout' },
    [pscustomobject]@{ Decorated = 'wow_user_runtime_set_context@12'; Raw = 'wow_user_runtime_set_context' },
    [pscustomobject]@{ Decorated = 'wow_user_worker_active@0'; Raw = 'wow_user_worker_active' },
    # These parent exports are cdecl worker bodies. The original WOW32 DLL
    # references them under /Gz, so its @0 names must weak-alias to the
    # cdecl import-library spelling (including its leading underscore).
    # Name-decoration binding to the public stdcall adapter, not CPU40 cdecl.
    [pscustomobject]@{ Decorated = 'call_ica_hw_interrupt@12'; Raw = 'call_ica_hw_interrupt' },
    [pscustomobject]@{ Decorated = 'RtlNtStatusToDosError@4'; Raw = 'RtlNtStatusToDosError' },
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
    '#include "wow_common_dialog_callback.h"',
    '#include "wow_hook_callback.h"',
    '#include "wow_idle_hook_binding.h"',
    '#include "wow_standard_class_provider.h"',
    '#include "wow_task_cleanup_entry.h"',
    '#include "wow_timer_callback.h"',
    '#include "wow_native_extension_provider.h"',
    '#include "wow_thunk_provider.h"',
    'INT WINAPI wow_original_dispatch_exception(DWORD, PEXCEPTION_POINTERS, struct _TD *, DWORD, BOOL);',
    'VOID WINAPI wow_original_thunk_capture(wow_thunk_mode *);',
    'VOID WINAPI wow_original_thunk_restore(const wow_thunk_mode *);',
    'VOID WINAPI wow_original_thunk_frame(struct _VDMFRAME *, wow_thunk_frame *);',
    'static const wow_thunk_provider thunk_provider = {',
    '    wow_original_dispatch_exception, wow_original_thunk_capture,',
    '    wow_original_thunk_restore, wow_original_thunk_frame };',
    'BOOL WINAPI wow_original_kernel_resource_return(INT);',
    'BOOL WINAPI wow_original_callback_guarded(INT, union _PARM16 *, DWORD, DWORD *);',
    'DWORD WINAPI wow_original_WK32ICallProc32(struct _VDMFRAME *);',
    'ULONG WINAPI wow_original_WMM32CallProc32(struct _VDMFRAME *);',
    'VOID WINAPI wow_original_WOWYield16(void);',
    'VOID WINAPI wow_original_WOWDirectedYield16(WORD);',
    'static const wow_native_extension_provider extension_provider = {',
    '    wow_original_kernel_resource_return, wow_original_callback_guarded,',
    '    wow_original_WK32ICallProc32, wow_original_WMM32CallProc32,',
    '    wow_original_WOWYield16, wow_original_WOWDirectedYield16 };',
    'VOID WINAPI WowOriginalTimerFunc(UINT, HWND, UINT, DWORD);',
    'BOOL WINAPI wow_original_task_cleanup_uninitialized(DWORD);',
    'VOID WINAPI wow_original_task_retire_async(void);',
    'VOID WINAPI WowOriginalFreeTask(PVOID);',
    'static const wow_task_cleanup_provider cleanup_provider = {',
    '    wow_original_task_cleanup_uninitialized, wow_original_task_retire_async, WowOriginalFreeTask };',
    '#include "compat/include/wow_class_client_bindings.h"',
    'ATOM WINAPI wow_original_register_class(WNDCLASSA *, DWORD *);',
    'BOOL WINAPI wow_original_unregister_class(LPCSTR, HINSTANCE);',
    'BOOL WINAPI wow_original_get_class_info(HINSTANCE, LPCSTR, WNDCLASSA *);',
    'static const wow_class_client_provider class_client_provider = {',
    '    wow_original_register_class, wow_original_unregister_class, wow_original_get_class_info, _RegisterClassEx, GetClassPtr, _InnerGetClassPtr, _UnregisterClass };',
    '#include "compat/include/wow_button_lifetime.h"',
    'PVOID WINAPI wow_original_button_data(struct tagWND *);',
    'VOID WINAPI FreeLookasideEntry(PLOOKASIDE, PVOID);',
    '#include "compat/include/wow_task_profile_bindings.h"',
    'UINT WINAPI wow_original_task_profile(UINT, LPCWSTR, LPCWSTR, LPWSTR, DWORD);',
    '#include "compat/include/wow_native_text_descriptor.h"',
    '#include "compat/include/wow_bitmap_bindings.h"',
    '#include "compat/include/wow_cleanup_bindings.h"',
    '#include "compat/include/wow_default_window_provider.h"',
    'LONG WINAPI wow_original_default_window_proc(struct tagWND *, UINT, WPARAM, LPARAM, DWORD);',
    'extern const wow_cleanup_provider wow_original_cleanup_provider;',
    'HBITMAP WINAPI WOWLoadBitmapA(HINSTANCE, LPCSTR, LPBYTE, DWORD);',
    'void WINAPI wow_original_text_capture(wow_native_text_descriptor *, const struct _LARGE_STRING *);',
    'INT WINAPI GetStdClassNumber(LPSTR);',
    'LPCSTR WINAPI wow_original_class_name(UINT);',
    'ATOM WINAPI wow_original_class_atom(UINT);',
    'BOOL WINAPI wow_original_class_capture(LPCSTR, PUNICODE_STRING);',
    'static const wow_standard_class_provider class_provider = {',
    '    GetStdClassNumber, wow_original_class_name, wow_original_class_atom, wow_original_class_capture };',
    'BOOL WINAPI wow_original_idle_registered(DWORD);',
    'BOOL WINAPI wow_original_separate_launch(void);',
    'LRESULT CALLBACK WowOriginalForegroundIdleHook(int, WPARAM, LPARAM);',
    'static const wow_task_state_callbacks task_callbacks = {',
    '    wow_original_idle_registered, wow_original_separate_launch, WowOriginalForegroundIdleHook };',
    'void WINAPI wow_hook_capture_context(wow_hook_context *);',
    'void WINAPI wow_hook_restore_context(const wow_hook_context *);',
    'BOOL WINAPI wow_hook_query_type(INT, INT *);',
    'LONG WINAPI WowOriginalStdHookProc(INT, LONG, LONG, INT);',
    'static const wow_hook_callbacks hook_callbacks = {',
    '    wow_hook_capture_context, wow_hook_restore_context, wow_hook_query_type, WowOriginalStdHookProc };',
    'UINT WINAPI WowOriginalDialogProc(HWND, UINT, WPARAM, LPARAM);',
    'UINT WINAPI WowOriginalPrintSetupDialogProc(HWND, UINT, WPARAM, LPARAM);',
    'UINT WINAPI WowOriginalFindReplaceDialogProc(HWND, UINT, WPARAM, LPARAM);',
    'static const wow_common_dialog_callbacks dialog_callbacks = {',
    '    WowOriginalDialogProc, WowOriginalPrintSetupDialogProc, WowOriginalFindReplaceDialogProc };',
    'extern unsigned char __stdcall W32DllInitialize(void *, unsigned long, void *);',
    'extern void __stdcall wow_bitmap_bindings_shutdown(void);',
    'extern void __stdcall wow_registry_binding_shutdown(void);',
    '#include "compat/include/wow_control_brush_bindings.h"',
    'HBRUSH WINAPI xxxGetControlBrush(struct tagWND *, HDC, UINT);',
    'int __stdcall DllMain(void *instance, unsigned long reason, void *reserved)',
    '{',
    '    int result;',
    '    if (reason == 1 && !wow_control_brush_bind(xxxGetControlBrush)) return 0;',
    '    if (reason == 1 && !wow_common_dialog_bind(&dialog_callbacks)) return 0;',
    '    if (reason == 1 && !wow_hook_bind(&hook_callbacks)) return 0;',
    '    if (reason == 1 && !wow_task_state_bind(&task_callbacks)) return 0;',
    '    if (reason == 1 && !wow_standard_class_bind(&class_provider)) return 0;',
    '    if (reason == 1 && !wow_class_client_bind(&class_client_provider)) return 0;',
    '    if (reason == 1 && !wow_button_bind(wow_original_button_data, FreeLookasideEntry)) return 0;',
    '    if (reason == 1 && !wow_native_text_bind(wow_original_text_capture)) return 0;',
    '    if (reason == 1 && !wow_profile_bind(wow_original_task_profile, FastGetProfileIntW)) return 0;',
    '    if (reason == 1 && !wow_bitmap_bind_provider(WOWLoadBitmapA)) return 0;',
    '    if (reason == 1 && !wow_cleanup_bind(&wow_original_cleanup_provider)) return 0;',
    '    if (reason == 1 && !wow_default_window_bind(wow_original_default_window_proc)) return 0;',
    '    if (reason == 1 && !wow_task_cleanup_bind(&cleanup_provider)) return 0;',
    '    if (reason == 1 && !wow_timer_bind(WowOriginalTimerFunc)) return 0;',
    '    if (reason == 1 && !wow_native_extension_bind(&extension_provider)) return 0;',
    '    if (reason == 1 && !wow_thunk_bind(&thunk_provider)) return 0;',
    '    result = W32DllInitialize(instance, reason, reserved);',
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
    'src/compat/include',
    'src/mvdm-host/wow32',
    'src/mvdm-host/inc',
    'src/mvdm-host/softpc.new/base/inc',
    'src/mvdm-host/softpc.new/host/inc',
    'src/non-mvdm/public/sdk/inc',
    'src/non-mvdm/public/internal/base/inc',
    'src/non-mvdm/windows/core/ntuser/inc',
    'src/non-mvdm/public/internal/windows/inc',
    'src/non-mvdm/public/internal/shell/inc',
    'src/non-mvdm/private/windows/inc'
)
$includes = ($includeRoots | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }) -join ' '
$force = '/FI "' + (Join-Path $root 'src/non-mvdm/public/sdk/inc/nt.h') + '"'
# Original NT x86 i386mk.inc defaults to /Gz. Keep this DLL's
# internal ABI; worker-owned cdecl imports need explicit declarations, not a
# package-wide /Gd override or aliases that conceal different stack cleanup.
$cflags = '/nologo /TC /c /MT /W3 /Gz /showIncludes /D_NO_CRT_STDIO_INLINE /DWIN32 /DWINNT /DNTVDM /DCPU_40_STYLE /DNEW_CPU /DCCPU /DC_VID /DSPC386 /DSIM32 /DV7VGA /DANSI /DPROD ' + $force + ' ' + $includes
$nativeCflags = '/nologo /TC /c /MT /W3 /Gz /showIncludes /D_NO_CRT_STDIO_INLINE /DWINNT'
$adapterCflags = $nativeCflags + ' ' + $force + ' ' + $includes
$compatWowCflags = $nativeCflags + ' ' + $includes


$ninja = [System.Collections.Generic.List[string]]::new()
$ninja.Add('ninja_required_version = 1.10')
$ninja.Add('root = ' + (ConvertTo-NinjaPath $root))
$ninja.Add('environment = ' + (ConvertTo-NinjaPath $environment))
$ninja.Add('release_cflags = /O2')
$ninja.Add('cflags = ' + $cflags.Replace('\', '/'))
$ninja.Add('wow_cflags = $cflags /DWOW_ORIGINAL_THUNK_BUILD')
$ninja.Add('adapter_cflags = ' + $adapterCflags.Replace('\', '/'))
$ninja.Add('compat_wow_cflags = ' + $compatWowCflags.Replace('\', '/'))
# OpenNT ntuser/client/sources.inc and rtl/client/sources.inc select UNICODE.
# Keep the recovered USER island distinct from original WOW32's ANSI graph.
$ninja.Add('control_user_flags = /UANSI /DUNICODE')
$ninja.Add('rule cc')
$ninja.Add('  command = ' + $compilerCommand + ' $cflags $release_cflags /Fo$out $in')
$ninja.Add('  deps = msvc')
$ninja.Add('  description = CC $in')
$ninja.Add('rule generic_call_asm')
$ninja.Add('  command = ml /nologo /c /DDBG=0 /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/public/sdk/inc')) + '" /Fo$out $in')
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
        $ninja.Add('  cflags = $wow_cflags /DWOWSysErrorBox=wow_user_sys_error_box')
    } elseif ($source -eq 'wkman.c') {
        $ninja.Add('  cflags = $wow_cflags /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/wow_native_process.h')) + '"')
    } elseif ($source -eq 'wstruc.c') {
        # Structure conversion reads public GWL_STYLE, not WOW-private words.
        $ninja.Add('  cflags = $cflags')
    } elseif ($source -eq 'wudlg.c') {
        $ninja.Add('  cflags = $wow_cflags /DWOWSysErrorBox=wow_user_sys_error_box')
    } elseif ($source -eq 'wkfileio.c') {
        $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/wow-vdd')) + '" $wow_cflags')
    } elseif ($source -eq 'wcommdlg.c') {
        $ninja.Add('  cflags = $wow_cflags /DWOW_COMMON_DIALOG_BINDING /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/wow_common_dialog_calls.h')) + '"')
    } elseif ($source -in @('wuser.c','wuser31.c','wuwind.c')) {
        $hardErrorBinding = if ($source -eq 'wuser.c') {' /DWOWSysErrorBox=wow_user_sys_error_box'} else {''}
        $ninja.Add('  cflags = $wow_cflags' + $hardErrorBinding)
    } elseif ($source -eq 'wkgthunk.c') {
        $ninja.Add('  cflags = $wow_cflags /DWOW_X86_GENERIC_CALL /DWK32ICallProc32=wow_original_WK32ICallProc32 /DWOWYield16=wow_original_WOWYield16 /DWOWDirectedYield16=wow_original_WOWDirectedYield16 /DWOW_NATIVE_EXTENSION_BINDING /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/wow_user_native_extension.h')) + '"')
    } elseif ($source -eq 'wmmedia.c') {
        $ninja.Add('  cflags = $wow_cflags /DWMM32CallProc32=wow_original_WMM32CallProc32')
    } elseif ($source -eq 'wshell.c') {
        $ninja.Add('  cflags = $wow_cflags /DWOWShellExecute=wow_shell_execute /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/wow_registry_binding.h')) + '"')
    } elseif ($source -in @('wkernel.c','mapembed.c','wdde.c','wow32fax.c')) {
        $ninja.Add('  cflags = $wow_cflags /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/wow_registry_binding.h')) + '"')
    } else {
        $ninja.Add('  cflags = $wow_cflags')
    }
}
# OPENNT-HOST-297: selected original conversion is C, despite upstream suffix.
$objects.Add('obj/opennt-ole/thoputil.obj')
$oleIncludes = ($includeRoots | Where-Object { $_ -notmatch '^src/mvdm-host(/|$)' } | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }) -join ' '
$ninja.Add('build obj/opennt-ole/thoputil.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/com/ole32/olethunk/olethk32/thoputil.cxx')))
$ninja.Add('  cflags = ' + $cflags.Replace($includes, $oleIncludes))
$objects.Add('obj/callpr32.obj')
$ninja.Add('build obj/callpr32.obj: generic_call_asm ' + (ConvertTo-NinjaPath (Join-Path $wow 'i386/callpr32.asm')) + ' | ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/public/sdk/inc/callconv.inc')))
$supportObjects = [System.Collections.Generic.List[string]]::new()
$objects.Add('obj/compat/wow_window_capture.obj')
$ninja.Add('build obj/compat/wow_window_capture.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_window_capture.c')))
$ninja.Add('  cflags = ' + $nativeCflags + ' /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src')) + '"')
$objects.Add('obj/compat/wow_registry_binding.obj')
$ninja.Add('build obj/compat/wow_registry_binding.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_registry_binding.c')))
$ninja.Add('  cflags = ' + $nativeCflags + ' /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src')) + '"')
$boundaryTest = ConvertTo-NinjaPath (Join-Path $root 'test/build/wow_thunk_macro_boundary.c')
$ninja.Add('build obj/tests/wow-boundary-adapter.obj: cc ' + $boundaryTest)
$ninja.Add('  cflags = $adapter_cflags')
$ninja.Add('build obj/tests/wow-boundary-thunk.obj: cc ' + $boundaryTest)
$ninja.Add('  cflags = $wow_cflags')
$ninja.Add('build wow-macro-boundary-test: phony obj/tests/wow-boundary-adapter.obj obj/tests/wow-boundary-thunk.obj')
# T447 S3: compose the recovered collaboration cohort, not just its adapters.
# These bodies already share the audited control-client facade in the whole
# cohort verification graph. Distinct path-qualified object names prevent
# collisions with the original MVDM thunk objects and the existing subsets.
foreach ($controlBody in @('editec','edecrare','editsl','editml','edmlonce',
        'edmlrare','edslrare','combo','combodir','comboini','lb1','lboxctl1',
        'lboxctl2','lboxctl3','lboxmult','lboxrare','lboxvar','dlgbegin',
        'dlgend','dlgmgr','dlgmgrc','dlgmgr2','dmmnem','winmgrc')) {
    $controlBodyPath = Join-Path $root ('src/non-mvdm/windows/core/ntuser/client/' + $controlBody + '.c')
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
    $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-client')) + '" $cflags $control_user_flags' + $bodyFlags)
}
$objects.Add('obj/compat/wow-dialog-lookup.obj')
$ninja.Add('build obj/compat/wow-dialog-lookup.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_dialog_lookup.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_DIALOG_LOOKUP_ONLY')
foreach ($viewBody in @('winmgr','wow')) {
    $viewObject = 'obj/opennt-user/control-view-' + $viewBody + '.obj'
    $objects.Add($viewObject)
    $ninja.Add('build ' + $viewObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/non-mvdm/windows/core/ntuser/rtl/' + $viewBody + '.c'))))
    $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT')
}
# Original USER owner, not an adapter implementation (OPENNT-HOST-032).
$ddeDataObject = 'obj/opennt-user/hdata.obj'
$objects.Add($ddeDataObject)
$ninja.Add('build ' + $ddeDataObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/hdata.c')))
foreach ($userOwner in @('clres', 'cldib', 'rtlinit', 'ntstubs', 'clmenu', 'clglobal')) {
    $object = 'obj/opennt-user/' + $userOwner + '.obj'
    $objects.Add($object)
    $ninja.Add('build ' + $object + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/non-mvdm/windows/core/ntuser/client/' + $userOwner + '.c'))))
    if ($userOwner -eq 'clglobal') {
        # Original USER globals include WCHAR TEXT literals; retain that
        # unit's Unicode contract, distinct from the ANSI WOW thunk build.
        $ninja.Add('  cflags = $cflags $control_user_flags')
    }
}
$objects.Add('obj/wow32-dll/wow_client_dispatch.obj')
$ninja.Add('build obj/wow32-dll/wow_client_dispatch.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_client_dispatch.c')))
$classLookupObject = 'obj/opennt-user/class.obj'
# Original dialog initialization and shared EDIT DS callbacks use the same
# registered WOW table as LOCAL*. Do not replace these bodies in an adapter.
$dialogMemoryObject = 'obj/wow32-dll/wow_dialog_memory.obj'
# OPENNT-HOST-079: original modal template entry, separate from clres's
# bitmap slice. The existing modal entry owns context/task/borrow lifetimes.
$dialogTemplateObject = 'obj/opennt-user/dialog-template.obj'
$objects.Add($dialogTemplateObject)
$ninja.Add('build ' + $dialogTemplateObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/clres.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_DIALOG_CLIENT /DWOW_ORIGINAL_DIALOG_MANAGER /DWOW_ORIGINAL_DIALOG_TEMPLATE')
$objects.Add($dialogMemoryObject)
$ninja.Add('build ' + $dialogMemoryObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_dialog_memory.c')))
$ninja.Add('  cflags = $cflags $control_user_flags')
# Original state transitions, not replacements in the binding units. The
# kernel ntstubs subset has its registered receive facade in the source;
# miscutil keeps precomp.h and receives only its finite original layouts.
$controlFnidObject = 'obj/wow32-dll/wow_control_fnid.obj'
$controlTextObject = 'obj/opennt-user/control-text.obj'
$objects.Add($controlTextObject)
$ninja.Add('build ' + $controlTextObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/ntstubs.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_CONTROL_TEXT')
$controlColorObject = 'obj/opennt-user/control-color.obj'
$objects.Add($controlColorObject)
$ninja.Add('build ' + $controlColorObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/ntstubs.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_CONTROL_COLOR')
$controlBrushObject = 'obj/wow32-dll/wow_control_brush.obj'
$controlBrushClientObject = 'obj/opennt-user/control-brush-client.obj'
$objects.Add($controlBrushClientObject)
$ninja.Add('build ' + $controlBrushClientObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/ntstubs.c')))
$ninja.Add('  cflags = $cflags $control_user_flags /DWOW_ORIGINAL_CONTROL_BRUSH_CLIENT')
$objects.Add($controlBrushObject)
$ninja.Add('build ' + $controlBrushObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/source/wow_control_brush.c')))
$ninja.Add('  cflags = $cflags $control_user_flags')
$controlStateObject = 'obj/wow32-dll/wow_control_state.obj'
$objects.Add($controlStateObject)
$ninja.Add('build ' + $controlStateObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_control_state.c')))
$ninja.Add('  cflags = $cflags $control_user_flags')
$objects.Add($controlFnidObject)
$ninja.Add('build ' + $controlFnidObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_control_fnid.c')))
$ninja.Add('  cflags = $cflags $control_user_flags')
$dialogPointerObject = 'obj/wow32-dll/wow_dialog_pointer.obj'
$objects.Add($dialogPointerObject)
$ninja.Add('build ' + $dialogPointerObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_dialog_pointer.c')))
$ninja.Add('  cflags = $cflags $control_user_flags')
$stringServicesObject = 'obj/compat/ntuser_rtl_compat.obj'
$objects.Add($stringServicesObject)
$ninja.Add('build ' + $stringServicesObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/ntuser_rtl_compat.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$controlRectObject = 'obj/compat/wow-control-rect.obj'
$objects.Add($controlRectObject)
$ninja.Add('build ' + $controlRectObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_control_rect.c')))
$ninja.Add('  cflags = ' + $nativeCflags + ' /DUNICODE')
$controlDrawObject = 'obj/opennt-user/control-draw.obj'
$defaultMessagesObject = 'obj/opennt-user/default-messages.obj'
$defaultClientObject = 'obj/opennt-user/default-client.obj'
$objects.Add($defaultClientObject)
$ninja.Add('build ' + $defaultClientObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/clmsg.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_DEFAULT_WINDOW')
$defaultBitsObject = 'obj/wow32-dll/wow_default_window_bits.obj'
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
    $ninja.Add('build ' + $accessObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/non-mvdm/windows/core/ntuser/' + $accessOwner[1]))))
    $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT ' + $accessOwner[2])
}
$objects.Add($privateWordSetterObject)
$ninja.Add('build ' + $privateWordSetterObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/getset.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_PRIVATE_WORD_SETTER')
$objects.Add($defaultBitsObject)
$ninja.Add('build ' + $defaultBitsObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_default_window_bits.c')))
$ninja.Add('  cflags = $cflags $control_user_flags')
$defaultBordersObject = 'obj/opennt-user/default-borders.obj'
$objects.Add($defaultBordersObject)
$ninja.Add('build ' + $defaultBordersObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/rtl/winmgr.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-geometry')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_DEFAULT_GEOMETRY')
$objects.Add($defaultMessagesObject)
$ninja.Add('build ' + $defaultMessagesObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/server.c')))
$ninja.Add('  cflags = $cflags $control_user_flags')
$objects.Add($controlDrawObject)
$ninja.Add('build ' + $controlDrawObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/rtl/draw.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-drawing')) + '" $cflags $control_user_flags')
foreach ($textPart in @('client', 'rtl')) {
    $textObject = 'obj/opennt-user/control-text-' + $textPart + '.obj'
    $objects.Add($textObject)
    $ninja.Add('build ' + $textObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/non-mvdm/windows/core/ntuser/' + $textPart + '/drawtext.c'))))
    $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-drawing')) + '" $cflags $control_user_flags')
}
# T447 S3 original painting dependencies, not a substitute button WndProc.
$extentObject = 'obj/opennt-user/control-text-extent.obj'
$objects.Add($extentObject)
$ninja.Add('build ' + $extentObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/rtl/text.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/text-measure')) + '" $cflags $control_user_flags')
foreach ($drawingPart in @('draw', 'mngrayc')) {
    $drawingObject = 'obj/opennt-user/control-client-' + $drawingPart + '.obj'
    $objects.Add($drawingObject)
    $ninja.Add('build ' + $drawingObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root ('src/non-mvdm/windows/core/ntuser/client/' + $drawingPart + '.c'))))
    if ($drawingPart -eq 'mngrayc') {
        $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-draw-state')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_STATE_DRAWING')
    } else {
        $ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT')
    }
}
$buttonObject = 'obj/opennt-user/button-client.obj'
$objects.Add($buttonObject)
$ninja.Add('build ' + $buttonObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/btnctl.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/control-client')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_BUTTON_CLIENT')
$buttonBlitObject = 'obj/opennt-user/button-blit.obj'
$objects.Add($buttonBlitObject)
$ninja.Add('build ' + $buttonBlitObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/mngrayc.c')))
$ninja.Add('  cflags = /I "' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/include/button-drawing')) + '" $cflags $control_user_flags /DWOW_ORIGINAL_BUTTON_BLIT_ONLY')
  $probeObject = 'obj/opennt-user/probe.obj'
  $cleanupObject = 'obj/opennt-user/cleanup.obj'
  $handleFlagsObject = 'obj/compat/wow-object-ownership.obj'
  $objects.Add($handleFlagsObject)
  $ninja.Add('build ' + $handleFlagsObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_object_ownership.c')))
  $ninja.Add('  cflags = $cflags /FI "wow_user_client_view_layout.h"')
  $objects.Add($cleanupObject)
  $ninja.Add('build ' + $cleanupObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/cleanup.c')))
$objects.Add($probeObject)
$ninja.Add('build ' + $probeObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/base/ntos/ex/probe.c')))
$classRegisterObject = 'obj/opennt-user/class-register.obj'
$classRemoveObject = 'obj/opennt-user/class-remove.obj'
$objects.Add($classRemoveObject)
$ninja.Add('build ' + $classRemoveObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/class.c')))
$ninja.Add('  cflags = $cflags /DWOW_ORIGINAL_CLASS_REMOVAL')
$objects.Add($classRegisterObject)
$ninja.Add('build ' + $classRegisterObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/class.c')))
$ninja.Add('  cflags = $cflags /DWOW_ORIGINAL_CLASS_REGISTRATION')
$taskOrderObject = 'obj/opennt-user/taskman.obj'
$objects.Add($taskOrderObject)
$ninja.Add('build ' + $taskOrderObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/taskman.c')))
$classClientObject = 'obj/wow32-dll/wow_class_client.obj'
$inputGroupObject = 'obj/wow32-dll/wow_input_membership.obj'
$objects.Add($inputGroupObject)
$ninja.Add('build ' + $inputGroupObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_input_membership.c')))
$taskInitObject = 'obj/opennt-user/queue.obj'
$profileObject = 'obj/opennt-user/profile.obj'
$objects.Add($profileObject)
$ninja.Add('build ' + $profileObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/profile.c')))
$objects.Add($taskInitObject)
$ninja.Add('build ' + $taskInitObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/queue.c')))
$taskRegisterObject = 'obj/opennt-user/exitwin.obj'
$objects.Add($taskRegisterObject)
$ninja.Add('build ' + $taskRegisterObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/exitwin.c')))
$objects.Add($classClientObject)
$ninja.Add('build ' + $classClientObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/wow32-dll/source/wow_class_client.c')))
$ninja.Add('  cflags = $cflags')
$objects.Add($classLookupObject)
$ninja.Add('build ' + $classLookupObject + ': cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/kernel/class.c')))
foreach ($source in $providerSupportSources) {
    $owner = if ($source.StartsWith((Join-Path $root 'src/compat'), [StringComparison]::OrdinalIgnoreCase)) { 'compat' } elseif ($source.StartsWith($adapterWow, [StringComparison]::OrdinalIgnoreCase)) { 'wow32-dll' } else { 'adapter-win32' }
    $object = 'obj/' + $owner + '/' + [IO.Path]::GetFileNameWithoutExtension($source) + '.obj'
    $supportObjects.Add($object)
    $ninja.Add('build ' + $object + ': cc ' + (ConvertTo-NinjaPath $source))
    if ([IO.Path]::GetFileName($source) -eq 'wow_clipboard_epoch.c') {
        $ninja.Add('  cflags = ' + $nativeCflags)
    } elseif ([IO.Path]::GetFileName($source) -eq 'chartran.c') {
        $ninja.Add('  cflags = $cflags /DWOW_ORIGINAL_LARGE_STRING_LAYOUT /FI "' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/inc/user.h')) + '"')
    } elseif ($source.StartsWith((Join-Path $root 'src/lib-win32'), [StringComparison]::OrdinalIgnoreCase)) {
        $ninja.Add('  cflags = ' + $nativeCflags)
    } elseif ($source.StartsWith($adapterWow, [StringComparison]::OrdinalIgnoreCase) -or
        $source.StartsWith((Join-Path $root 'src/ntvdm-exe'), [StringComparison]::OrdinalIgnoreCase)) {
        $ninja.Add('  cflags = $adapter_cflags')
    }
}
$supportObjects.Add('obj/compat/wow_start_glass.obj')
$ninja.Add('build obj/compat/wow_start_glass.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_start_glass.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_hook_native_binding.obj')
$ninja.Add('build obj/compat/wow_hook_native_binding.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_hook_native_binding.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_fonts.obj')
$ninja.Add('build obj/compat/wow_fonts.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_fonts.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_control_brushes.obj')
$ninja.Add('build obj/compat/wow_control_brushes.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_control_brushes.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_icon_size.obj')
$ninja.Add('build obj/compat/wow_icon_size.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_icon_size.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_dialog_slots.obj')
$ninja.Add('build obj/compat/wow_dialog_slots.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_dialog_slots.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_user_failure.obj')
$ninja.Add('build obj/compat/wow_user_failure.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_user_failure.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_native_dialog.obj')
$ninja.Add('build obj/compat/wow_native_dialog.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_native_dialog.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_native_controls.obj')
$ninja.Add('build obj/compat/wow_native_controls.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_native_controls.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_profile_native.obj')
$ninja.Add('build obj/compat/wow_profile_native.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_profile_native.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_task_event_binding.obj')
$ninja.Add('build obj/compat/wow_task_event_binding.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_task_event_binding.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
$supportObjects.Add('obj/compat/wow_native_environment.obj')
$ninja.Add('build obj/compat/wow_native_environment.obj: cc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/compat/wow_native_environment.c')))
$ninja.Add('  cflags = ' + $nativeCflags)
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
$ninja.Add('build ' + $editMenuResource + ': rc ' + (ConvertTo-NinjaPath (Join-Path $root 'src/non-mvdm/windows/core/ntuser/client/res.rc')))
$ninja.Add('build legacy-wow-user32.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyUser32Definition))
$ninja.Add('build legacy-wow-kernel32.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyKernel32Definition))
$ninja.Add('build legacy-wow-ntdll.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyNtdllDefinition))
$ninja.Add('build wow32.dll | wow32.lib: wow_dll_link ' + (($objects + $supportObjects + @($systemImportAliasObject, $parentImportAliasObject, $dllEntryBridgeObject, 'obj/wow32.res', 'legacy-wow-user32.lib', 'legacy-wow-kernel32.lib', 'legacy-wow-ntdll.lib')) -join ' ') + ' | wow32-x86.def ' + (ConvertTo-NinjaPath $parentImportLibraryPath) + ' obj/tests/wow-boundary-adapter.obj obj/tests/wow-boundary-thunk.obj')
$ninja.Add('default wow32.dll')

# Named original-layout adapters live in B; keep original per-unit ABI flags.
$compatWowNames = @('wow_hook_callback.c', 'wow_user_thunk_scope.c', 'wow_timer_callback.c', 'wow_shell_callback.c', 'wow_ole_descriptor_binding.c', 'wow_user_private_access.c', 'wow_common_dialog_calls.c', 'wow_common_dialog_callback.c', 'wow_bitmap_bindings.c', 'wow_button_lifetime.c', 'wow_button_resources.c', 'wow_callback_frame_lease.c', 'wow_class_client_bindings.c', 'wow_class_words_binding.c', 'wow_cleanup_bindings.c', 'wow_client_dispatch.c', 'wow_control_brush.c', 'wow_control_brush_bindings.c', 'wow_control_draw_state.c', 'wow_control_extent_bindings.c', 'wow_control_fnid.c', 'wow_control_state.c', 'wow_default_message_bindings.c', 'wow_default_native_bindings.c', 'wow_default_state_bindings.c', 'wow_default_window_bindings.c', 'wow_default_window_bits.c', 'wow_dialog_call_bindings.c', 'wow_dialog_context_bindings.c', 'wow_dialog_creation_binding.c', 'wow_dialog_keyboard_binding.c', 'wow_dialog_finalization_binding.c', 'wow_dialog_memory.c', 'wow_dialog_native_bindings.c', 'wow_dialog_pointer.c', 'wow_dialog_procedure_bindings.c', 'wow_dialog_state_bindings.c', 'wow_edit_call_bindings.c', 'wow_edit_conversion_bindings.c', 'wow_edit_entry_bindings.c', 'wow_edit_lookaside_bindings.c', 'wow_edit_native_bindings.c', 'wow_edit_state_bindings.c', 'wow_edit_thread_bindings.c', 'wow_idle_hook_binding.c', 'wow_input_group_lifetime.c', 'wow_input_membership.c', 'wow_list_native_bindings.c', 'wow_menu_client_bindings.c', 'wow_menu_native_bindings.c', 'wow_private_user_compat.c', 'wow_standard_class_bindings.c', 'wow_task_cleanup_entry.c', 'wow_task_profile_binding.c', 'wow_user_borrow_scope.c', 'wow_user_discovery.c', 'wow_user_message_bridge.c', 'wow_user_native_extension.c', 'wow_user_object_bindings.c', 'wow_user_registration_state.c', 'wow_user_registration_bridge.c', 'wow_task_start_notification.c', 'wow_user_task_lifecycle.c', 'wow_window_client_access_bindings.c', 'wow_window_client_state_bindings.c', 'wow_window_creation_binding.c', 'wow_window_procedure_entry.c', 'wow_window_words_binding.c')
for ($index = 0; $index -lt $ninja.Count; ++$index) {
    if ($ninja[$index] -notmatch '^build .*: cc .*src/compat/([^/]+\.c)$' -or
        $Matches[1] -notin $compatWowNames) { continue }
    $flags = '  cflags = $compat_wow_cflags'
    if ($index + 1 -lt $ninja.Count -and $ninja[$index + 1].StartsWith('  cflags = ')) {
        $flags = $ninja[$index + 1].Replace('$adapter_cflags', '$compat_wow_cflags').Replace('$cflags', '$compat_wow_cflags')
        $flags = $flags -replace '/I "[^"]*/src/wow32-dll/[^"]*"\s*', ''
        $ninja[$index + 1] = $flags
    } else { $ninja.Insert($index + 1, $flags) }
}

[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $ninja, [Text.UTF8Encoding]::new($false))
Write-Host "Generated T404 S5 WOW32 $Architecture provider graph: $build (bodies=$($sources.Count); carriers=$($headers.Count + 1); jobs=$ParallelJobs)"
