[CmdletBinding()]
param(
    [ValidateSet('x86')]
    [string]$Architecture = 'x86',
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$ParentImportLibrary = '',
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
$wow = Join-Path $root 'src/mvdm-host/wow32'
$adapterWow = Join-Path $root 'src/adapter-mvdm-host-out/wow'
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
    (Join-Path $adapterWow 'wow_user_callback_callconv.c'),
    (Join-Path $adapterWow 'wow_private_user_compat.c'),
    (Join-Path $root 'src/adapter-mvdm-host-out/win32/source/wow_public_user_facade.c'),
    # This is the already-admitted narrow historical CRT spelling bridge.  It
    # has no parent-machine state, so the late-loaded DLL may own its copy.
    (Join-Path $root 'src/adapter-mvdm-host-out/win32/source/crt_compat.c')
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
    $build = Join-Path $root 'build/M0-T404/S5/formal-x86'
} else {
    $build = [IO.Path]::GetFullPath($BuildRoot)
}
New-Item -ItemType Directory -Force -Path $build, (Join-Path $build 'obj'), (Join-Path $build 'obj\adapter-wow'), (Join-Path $build 'obj\adapter-win32') | Out-Null

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
    [pscustomobject]@{ Decorated = 'RegisterWowExec@4'; Raw = 'RegisterWowExec' },
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
    [pscustomobject]@{ Decorated = 'RtlFreeAnsiString@4'; Raw = 'RtlFreeAnsiString' },
    [pscustomobject]@{ Decorated = 'RtlImageNtHeader@4'; Raw = 'RtlImageNtHeader' },
    [pscustomobject]@{ Decorated = 'RtlInitUnicodeString@8'; Raw = 'RtlInitUnicodeString' },
    [pscustomobject]@{ Decorated = 'RtlInitializeCriticalSection@4'; Raw = 'RtlInitializeCriticalSection' },
    [pscustomobject]@{ Decorated = 'RtlLeaveCriticalSection@4'; Raw = 'RtlLeaveCriticalSection' },
    [pscustomobject]@{ Decorated = 'RtlMultiByteToUnicodeN@20'; Raw = 'RtlMultiByteToUnicodeN' },
    [pscustomobject]@{ Decorated = 'RtlUnicodeStringToAnsiString@12'; Raw = 'RtlUnicodeStringToAnsiString' },
    [pscustomobject]@{ Decorated = 'RtlUnicodeToMultiByteN@20'; Raw = 'RtlUnicodeToMultiByteN' },
    [pscustomobject]@{ Decorated = 'DbgBreakPoint@0'; Raw = 'DbgBreakPoint' }
)
@('LIBRARY USER32.DLL', 'EXPORTS', '    UserRegisterWowHandlers',
  '    SetCursorContents') |
    Set-Content -LiteralPath $legacyUser32Definition -Encoding ascii
@('LIBRARY GDI32.DLL', 'EXPORTS', '    GdiQueryTable') |
    Set-Content -LiteralPath $legacyGdi32Definition -Encoding ascii
@('LIBRARY KERNEL32.DLL', 'EXPORTS',
  '    RegisterWowBaseHandlers',
  '    RegisterWowExec') |
    Set-Content -LiteralPath $legacyKernel32Definition -Encoding ascii
@('LIBRARY NTDLL.DLL', 'EXPORTS',
  '    CsrIdentifyAlertableThread',
  '    NtClose', '    NtOpenThread',
  '    NtQueryInformationProcess', '    NtQueryInformationThread',
  '    NtQueryPerformanceCounter', '    NtQuerySystemInformation',
  '    NtWaitForSingleObject', '    RtlAdjustPrivilege', '    RtlCharToInteger',
  '    RtlDeleteCriticalSection', '    RtlEnterCriticalSection',
  '    RtlFreeAnsiString', '    RtlImageNtHeader', '    RtlInitUnicodeString',
  '    RtlInitializeCriticalSection', '    RtlLeaveCriticalSection',
  '    RtlMultiByteToUnicodeN', '    RtlUnicodeStringToAnsiString',
  '    RtlUnicodeToMultiByteN', '    DbgBreakPoint') |
    Set-Content -LiteralPath $legacyNtdllDefinition -Encoding ascii
$systemImportAliasSource = Join-Path $build 'wow32-system-import-abi-aliases.c'
$systemImportAliasLines = [Collections.Generic.List[string]]::new()
$systemImportAliasLines.Add('/* Generated build-only COFF aliases; no provider implementation. */')
foreach ($alias in $systemImportAliases) {
    $systemImportAliasLines.Add('#pragma comment(linker, "/alternatename:_' + $alias.Decorated + '=_' + $alias.Raw + '")')
    $systemImportAliasLines.Add('#pragma comment(linker, "/alternatename:__imp__' + $alias.Decorated + '=__imp__' + $alias.Raw + '")')
}
[IO.File]::WriteAllLines($systemImportAliasSource, $systemImportAliasLines,
    [Text.UTF8Encoding]::new($false))

# Original WOW32 calls its NTVDM/OEMUNI import surface with NT4 COFF names
# (`_Function@N`).  The standalone parent exports normal loader names; these
# aliases bind the former to the parent's import thunks without adding bodies.
$parentImportAliasSource = Join-Path $build 'wow32-parent-import-abi-aliases.c'
$parentImportAliasLines = [Collections.Generic.List[string]]::new()
$parentImportAliasLines.Add('/* Generated build-only aliases to the parent NTVDM import surface. */')
foreach ($alias in @(
    [pscustomobject]@{ Decorated = 'opennt_support_current_teb@0'; Raw = 'opennt_support_current_teb' },
    [pscustomobject]@{ Decorated = 'NtCurrentPeb@0'; Raw = 'NtCurrentPeb' },
    [pscustomobject]@{ Decorated = 'ExitVDM@8'; Raw = 'ExitVDM' },
    [pscustomobject]@{ Decorated = 'GetNextVDMCommand@4'; Raw = 'GetNextVDMCommand' },
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
    'extern int __cdecl W32DllInitialize(void *, unsigned long, void *);',
    'int __stdcall DllMain(void *instance, unsigned long reason, void *reserved)',
    '{',
    '    return W32DllInitialize(instance, reason, reserved);',
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
    'src/adapter-mvdm-host-out/win32/include',
    'src/adapter-mvdm-host-out/wow/include',
    'src/mvdm-host/wow32',
    'src/mvdm-host/inc',
    'src/mvdm-host/softpc.new/base/inc',
    'src/mvdm-host/softpc.new/host/inc',
    'src/opennt-host/public/sdk/inc',
    'src/opennt-abi/source/public/sdk/inc',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/internal/windows/inc',
    'src/opennt-abi/source/public/internal/shell/inc',
    'src/opennt-abi/source/private/windows/inc',
    'src/adapter-mvdm-host-out/softpc/include',
    'src/adapter-mvdm-host-out/basesrv/include',
    'src/adapter-mvdm-host-out/monitor/include',
    'src/adapter-mvdm-host-out/vdd/include',
    'src/session'
)
$includes = ($includeRoots | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }) -join ' '
$force = '/FI "' + (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h') + '"'
$cflags = '/nologo /TC /c /MT /W3 /Gd /showIncludes /DMVDM_WOW32_PROVIDER /D_NO_CRT_STDIO_INLINE /DWIN32 /DWINNT /DNTVDM /DCPU_40_STYLE /DNEW_CPU /DCCPU /DC_VID /DSPC386 /DSIM32 /DV7VGA /DANSI /DPROD ' + $force + ' ' + $includes

$ninja = [System.Collections.Generic.List[string]]::new()
$ninja.Add('ninja_required_version = 1.10')
$ninja.Add('root = ' + (ConvertTo-NinjaPath $root))
$ninja.Add('environment = ' + (ConvertTo-NinjaPath $environment))
$ninja.Add('cflags = ' + $cflags.Replace('\', '/'))
$ninja.Add('rule cc')
$ninja.Add('  command = cl $cflags /Fo$out $in')
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

$objects = [System.Collections.Generic.List[string]]::new()
foreach ($source in $sources) {
    $object = 'obj/' + [IO.Path]::GetFileNameWithoutExtension($source) + '.obj'
    $objects.Add($object)
    $ninja.Add('build ' + $object + ': cc ' + (ConvertTo-NinjaPath (Join-Path $wow $source)))
}
$supportObjects = [System.Collections.Generic.List[string]]::new()
foreach ($source in $providerSupportSources) {
    $owner = if ($source.StartsWith($adapterWow, [StringComparison]::OrdinalIgnoreCase)) { 'adapter-wow' } else { 'adapter-win32' }
    $object = 'obj/' + $owner + '/' + [IO.Path]::GetFileNameWithoutExtension($source) + '.obj'
    $supportObjects.Add($object)
    $ninja.Add('build ' + $object + ': cc ' + (ConvertTo-NinjaPath $source))
}
$systemImportAliasObject = 'obj/wow32-system-import-abi-aliases.obj'
$ninja.Add('build ' + $systemImportAliasObject + ': cc ' + (ConvertTo-NinjaPath $systemImportAliasSource))
$parentImportAliasObject = 'obj/wow32-parent-import-abi-aliases.obj'
$ninja.Add('build ' + $parentImportAliasObject + ': cc ' + (ConvertTo-NinjaPath $parentImportAliasSource))
$dllEntryBridgeObject = 'obj/wow32-dll-entry-bridge.obj'
$ninja.Add('build ' + $dllEntryBridgeObject + ': cc ' + (ConvertTo-NinjaPath $dllEntryBridgeSource))
$ninja.Add('build obj/wow32.res: rc ' + (ConvertTo-NinjaPath $resource))
$ninja.Add('build legacy-wow-user32.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyUser32Definition))
$ninja.Add('build legacy-wow-gdi32.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyGdi32Definition))
$ninja.Add('build legacy-wow-kernel32.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyKernel32Definition))
$ninja.Add('build legacy-wow-ntdll.lib: legacy_import_lib ' + (ConvertTo-NinjaPath $legacyNtdllDefinition))
$ninja.Add('build wow32.dll | wow32.dll.lib: wow_dll_link ' + (($objects + $supportObjects + @($systemImportAliasObject, $parentImportAliasObject, $dllEntryBridgeObject, 'obj/wow32.res', 'legacy-wow-user32.lib', 'legacy-wow-gdi32.lib', 'legacy-wow-kernel32.lib', 'legacy-wow-ntdll.lib')) -join ' '))
$ninja.Add('default wow32.dll')

[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $ninja, [Text.UTF8Encoding]::new($false))
Write-Host "Generated T404 S5 WOW32 $Architecture provider graph: $build (bodies=$($sources.Count); carriers=$($headers.Count + 1); jobs=$ParallelJobs)"
