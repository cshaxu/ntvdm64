[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$CompilerCache = '',
    [Parameter(Mandatory = $true)][string]$VsDevCmd
)

# Supplemental x86 call-boundary fixtures, not the S3 acceptance suite.
# Inputs: tracked adapters, retained original slices and explicit test doubles.
# Outputs: build.ninja and an x86 environment launcher beneath repository build/.
# Configure/build/run only within the owned background whole-group job.
# This generator does not compile, execute tests or publish product files.
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if (!$RepositoryRoot) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$allowed = [IO.Path]::GetFullPath((Join-Path $root 'build')) + [IO.Path]::DirectorySeparatorChar
if (!$BuildRoot) { $BuildRoot = Join-Path $allowed '.ninja/msvc-x86-release/call-boundaries' }
$build = [IO.Path]::GetFullPath($BuildRoot)
$compilerCommand = & "$PSScriptRoot/Initialize-NinjaTree.ps1" -BuildRoot $build -CompilerCache $CompilerCache
$vs = (Resolve-Path -LiteralPath $VsDevCmd).Path
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw 'VsDevCmd must name the toolchain batch file.' }

function NinjaPath([string]$value) {
    return $value.Replace('\', '/').Replace('$', '$$').Replace(':', '$:').Replace(' ', '$ ')
}
function NinjaValue([string]$value) { return $value.Replace('\', '/').Replace('$', '$$') }

# Original state guards and color client must be separate selections: their
# preprocessor subsets are mutually exclusive, as in the product generator.
$units = @(
    @('dialog_native_owner_probe', 'tests/adapter-mvdm-host-out/wow/wow_dialog_native_owner_probe.c', ''),
    @('dialog_procedure_adapter', 'src/wow32-dll/source/wow_dialog_procedure_bindings.c', ''),
    @('dialog_initialization_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_initialization_fixture.c', ''),
    @('dialog_initialization_adapter', 'src/wow32-dll/source/wow_dialog_procedure_bindings.c', ('/FI"'+$root+'/tests/adapter-mvdm-host-out/wow/wow_dialog_initialization_faults.h"')),
    @('style_notification_probe', 'tests/adapter-mvdm-host-out/wow/wow_style_notification_probe.c', ''),
    @('button_focus_probe', 'tests/adapter-mvdm-host-out/wow/wow_button_focus_service_probe.c', ''),
    @('string_fixture', 'tests/adapter-mvdm-host-out/wow/wow_control_string_fixture.c', ''),
    @('string_owner', 'src/opennt-host/windows/core/ntuser/rtl/random.c', ''),
    @('string_conversion', 'src/opennt-host/windows/core/ntuser/rtl/chartran.c', ''),
    @('string_services', 'src/wow32-dll/source/ntuser_rtl_compat.c', ''),
    @('dialog_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_call_fixture.c', ''),
    @('window_access_fixture', 'tests/adapter-mvdm-host-out/wow/wow_window_client_message_fixture.c', ''),
    @('window_access_adapter', 'src/wow32-dll/source/wow_window_client_access_bindings.c', ''),
    @('dialog_adapter', 'src/wow32-dll/source/wow_dialog_call_bindings.c', ''),
    @('edit_fixture', 'tests/adapter-mvdm-host-out/wow/wow_edit_call_fixture.c', ''),
    @('edit_adapter', 'src/wow32-dll/source/wow_edit_call_bindings.c', ''),
    @('control_extent', 'src/wow32-dll/source/wow_control_extent_bindings.c', ''),
    @('state_guards', 'src/opennt-host/windows/core/ntuser/client/client.c', '/DWOW_ORIGINAL_CONTROL_STATE'),
    @('color_client', 'src/opennt-host/windows/core/ntuser/client/ntstubs.c', '/DWOW_ORIGINAL_CONTROL_COLOR'),
    @('memory_fixture', 'tests/adapter-mvdm-host-out/wow/wow_edit_memory_callbacks_fixture.c', ''),
    @('dialog_memory', 'src/opennt-host/windows/core/ntuser/client/client.c', '/DWOW_ORIGINAL_DIALOG_EDIT'),
    @('brush_fixture', 'tests/adapter-mvdm-host-out/wow/wow_control_brush_fixture.c', ''),
    @('brush_adapter', 'src/wow32-dll/source/wow_control_brush_bindings.c', ''),
    @('brush_owner', 'src/opennt-host/windows/core/ntuser/kernel/random.c', ''),
    @('brush_client', 'src/opennt-host/windows/core/ntuser/client/ntstubs.c', '/DWOW_ORIGINAL_CONTROL_BRUSH_CLIENT'),
    @('default_fixture', 'tests/adapter-mvdm-host-out/wow/wow_default_window_binding_fixture.c', ''),
    @('default_adapter', 'src/wow32-dll/source/wow_default_window_bindings.c', ''),
    @('default_original_fixture', 'tests/adapter-mvdm-host-out/wow/wow_default_original_fixture.c', ''),
    @('dialog_context_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_context_fixture.c', ''),
    @('dialog_context_adapter', 'src/wow32-dll/source/wow_dialog_context_bindings.c', ''),
    @('dialog_creation_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_creation_entry_fixture.c', ''),
    @('creator_lifetime_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_creator_lifetime_fixture.c', ''),
    @('dialog_creation_adapter', 'src/wow32-dll/source/wow_dialog_creation_binding.c', ''),
    @('keyboard_entry_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_keyboard_entry_fixture.c', ''),
    @('keyboard_entry_adapter', 'src/wow32-dll/source/wow_dialog_keyboard_binding.c', ''),
    @('mnemonic_lifetime_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_mnemonic_lifetime_fixture.c', ''),
    @('keyboard_lifetime_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_keyboard_lifetime_fixture.c', ''),
    @('keyboard_unreached', 'tests/adapter-mvdm-host-out/wow/wow_dialog_keyboard_unreached.c', ''),
    @('dialog_finalization_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_finalization_fixture.c', ''),
    @('dialog_finalization_adapter', 'src/wow32-dll/source/wow_dialog_finalization_binding.c', ''),
    @('dialog_end_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_end_original_fixture.c', ''),
    @('dialog_modal_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_modal_entry_fixture.c', ''),
    @('dialog_modal_adapter', 'src/wow32-dll/source/wow_title_policy.c', ''),
    # The modal/context unit has no native execution transport. Accidental
    # selection of the unused native-dialog paths must fail, never succeed.
    @('dialog_modal_native_unreached', 'tests/adapter-mvdm-host-out/wow/wow_modal_native_unreached.c', ''),
    @('cpd_class_probe', 'tests/adapter-mvdm-host-out/wow/wow_cpd_class_normalization_probe.c', ''),
    @('class_address_probe', 'tests/adapter-mvdm-host-out/wow/wow_class_address_owner_probe.c', ''),
    @('destroy_event_probe', 'tests/adapter-mvdm-host-out/wow/wow_window_destroy_event_probe.c', ''),
    @('destroy_event_dll', 'tests/adapter-mvdm-host-out/wow/wow_window_destroy_event_probe.c', '/DWOW_DESTROY_EVENT_DLL'),
    @('cpd_window_probe', 'tests/adapter-mvdm-host-out/wow/wow_cpd_window_normalization_probe.c', '')
)
$controlObjects = @()
$getsetFlags = '/I "' + (NinjaValue (Join-Path $root 'tests/adapter-mvdm-host-out/wow/dialog-getset')) + '"'
$units += ,@('getset_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_getset_fixture.c', '')
$units += ,@('getset_reader', 'src/opennt-host/windows/core/ntuser/rtl/getset.c', $getsetFlags)
$units += ,@('getset_client', 'src/opennt-host/windows/core/ntuser/client/ntstubs.c', ($getsetFlags + ' /DWOW_ORIGINAL_DIALOG_GETSET'))
$units += ,@('getset_server', 'src/opennt-host/windows/core/ntuser/kernel/getset.c', $getsetFlags)
$units += ,@('getset_callproc', 'src/opennt-host/windows/core/ntuser/client/clmsg.c', ($getsetFlags + ' /DWOW_ORIGINAL_CLIENT_CPD'))
$nativeGetsetFlags = $getsetFlags + ' /DWOW_NATIVE_DIALOG_PROCEDURE_OWNER /FI "' + (NinjaValue (Join-Path $root 'src/wow32-dll/include/wow_dialog_procedure_bindings.h')) + '"'
$units += ,@('native_getset_reader', 'src/opennt-host/windows/core/ntuser/rtl/getset.c', ($nativeGetsetFlags + ' /DWOW_ORIGINAL_PRIVATE_WORD_GETTER'))
$units += ,@('native_getset_client', 'src/opennt-host/windows/core/ntuser/client/ntstubs.c', ($nativeGetsetFlags + ' /DWOW_ORIGINAL_DIALOG_GETSET'))
$units += ,@('native_getset_server', 'src/opennt-host/windows/core/ntuser/kernel/getset.c', $nativeGetsetFlags)
$modalLifetimeFlags = '/I "' + (NinjaValue (Join-Path $root 'tests/adapter-mvdm-host-out/wow/dialog-lifetime')) + '"'
$units += ,@('modal_lifetime_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_lifetime_fixture.c', '')
$units += ,@('modal_lifetime_body', 'src/opennt-host/windows/core/ntuser/client/dlgmgr.c', $modalLifetimeFlags)
$units += ,@('modal_lifetime_end', 'src/opennt-host/windows/core/ntuser/client/dlgend.c', $modalLifetimeFlags)
$units += ,@('modal_lifetime_pointer', 'src/opennt-host/windows/core/ntuser/kernel/miscutil.c', $modalLifetimeFlags)
$navigationFlags = '/DWOW_ORIGINAL_EDIT_CLIENT /I "' + (NinjaValue (Join-Path $root 'src/wow32-dll/include/control-client')) + '"'
$units += ,@('navigation_fixture', 'tests/adapter-mvdm-host-out/wow/wow_dialog_navigation_fixture.c', '')
$units += ,@('navigation_graph', 'src/opennt-host/windows/core/ntuser/rtl/winmgr.c', $navigationFlags)
$units += ,@('navigation_help', 'src/opennt-host/windows/core/ntuser/rtl/help.c', '')
$textFlags = '/I "' + (NinjaValue (Join-Path $root 'src/wow32-dll/include/control-drawing')) + '"'
$units += ,@('string_prefix', 'src/opennt-host/windows/core/ntuser/rtl/drawtext.c', $textFlags)
$units += ,@('string_extent', 'src/opennt-host/windows/core/ntuser/rtl/text.c', $textFlags)
$mnemonicFlags = '/DWOW_ORIGINAL_MNEMONIC_DRAWING /I "' + (NinjaValue (Join-Path $root 'tests/adapter-mvdm-host-out/wow/mnemonic-drawing')) + '"'
$units += ,@('mnemonic_body', 'src/opennt-host/windows/core/ntuser/rtl/drawtext.c', $mnemonicFlags)
$units += ,@('mnemonic_fixture', 'tests/adapter-mvdm-host-out/wow/wow_mnemonic_drawing_fixture.c', '')
$measureFlags = '/I "' + (NinjaValue (Join-Path $root 'tests/adapter-mvdm-host-out/wow/text-measure')) + '"'
$units += ,@('measure_failure_body', 'src/opennt-host/windows/core/ntuser/rtl/text.c', $measureFlags)
$units += ,@('measure_failure_fixture', 'tests/adapter-mvdm-host-out/wow/wow_text_measure_failure_fixture.c', '')
$pushFlags = '/DWOW_ORIGINAL_BUTTON_DRAWING /I "' + (NinjaValue (Join-Path $root 'tests/adapter-mvdm-host-out/wow/edit-drawing')) + '"'
$units += ,@('push_fixture', 'tests/adapter-mvdm-host-out/wow/wow_edit_drawing_fixture.c', $pushFlags)
$units += ,@('push_owner', 'src/opennt-host/windows/core/ntuser/rtl/draw.c', $pushFlags)
$units += ,@('push_rect', 'src/opennt-host/windows/core/ntuser/rtl/rect.c', $pushFlags)
$resourceFlags = '/I "' + (NinjaValue (Join-Path $root 'src/wow32-dll/include/button-resources')) + '"'
$resourceTestFlags = '/DWOW_BUTTON_RESOURCE_TEST_LEAVES /FI "' + (NinjaValue (Join-Path $root 'tests/adapter-mvdm-host-out/wow/button-resources/leaves.h')) + '"'
$units += ,@('button_resources_table', 'src/opennt-host/windows/core/ntuser/kernel/inctlpan.c', $resourceFlags)
$units += ,@('button_resources_body', 'src/wow32-dll/source/wow_button_resources.c', $resourceTestFlags)
$units += ,@('button_resources_fixture', 'tests/adapter-mvdm-host-out/wow/wow_button_resources_fixture.c', '')
$stateDrawingFlags = '/DWOW_ORIGINAL_STATE_DRAWING /DWOW_ORIGINAL_BUTTON_ATLAS /I "' + (NinjaValue (Join-Path $root 'src/wow32-dll/include/control-draw-state')) + '"'
$units += ,@('state_drawing_body', 'src/opennt-host/windows/core/ntuser/client/mngrayc.c', $stateDrawingFlags)
$units += ,@('state_drawing_adapter', 'src/wow32-dll/source/wow_control_draw_state.c', '')
$units += ,@('state_drawing_brushes', 'src/wow32-dll/source/wow_edit_drawing_bindings.c', '')
$units += ,@('state_drawing_atlas', 'src/wow32-dll/source/wow_button_resources.c', '')
$units += ,@('state_drawing_fixture', 'tests/adapter-mvdm-host-out/wow/wow_control_draw_state_fixture.c', '')
$units += ,@('state_drawing_text', 'src/opennt-host/windows/core/ntuser/rtl/drawtext.c', ($textFlags + ' /DWOW_ORIGINAL_MNEMONIC_DRAWING'))
$units += ,@('button_lifetime_adapter', 'src/wow32-dll/source/wow_button_lifetime.c', '')
$units += ,@('button_lifetime_fixture', 'tests/adapter-mvdm-host-out/wow/wow_button_lifetime_fixture.c', '')
$buttonLifetimeResourceFlags = '/FI "' + (NinjaValue (Join-Path $root 'tests/adapter-mvdm-host-out/wow/button-lifetime/resource-leaves.h')) + '"'
$units += ,@('button_lifetime_resources', 'src/wow32-dll/source/wow_button_resources.c', $buttonLifetimeResourceFlags)
$buttonBlitFlags = '/DWOW_ORIGINAL_BUTTON_ATLAS /DWOW_ORIGINAL_BUTTON_BLIT_ONLY /I "' + (NinjaValue (Join-Path $root 'src/wow32-dll/include/button-drawing')) + '"'
$units += ,@('button_blit_owner', 'src/opennt-host/windows/core/ntuser/client/mngrayc.c', $buttonBlitFlags)
$buttonDcFlags = '/DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_BUTTON_CLIENT /I "' + (NinjaValue (Join-Path $root 'tests/adapter-mvdm-host-out/wow/button-lifetime')) + '"'
$units += ,@('button_dc_owner', 'src/opennt-host/windows/core/ntuser/client/btnctl.c', $buttonDcFlags)
$units += ,@('button_dc_fixture', 'tests/adapter-mvdm-host-out/wow/wow_button_dc_fixture.c', '')
$units += ,@('button_dc_unreached', 'tests/adapter-mvdm-host-out/wow/wow_button_dc_unreached.c', '')
$geometryFlags = '/DWOW_ORIGINAL_DEFAULT_GEOMETRY /I "' + (NinjaValue (Join-Path $root 'src/wow32-dll/include/control-geometry')) + '"'
$units += ,@('navigation_borders', 'src/opennt-host/windows/core/ntuser/rtl/winmgr.c', $geometryFlags)
$viewFlags = '/I "' + (NinjaValue (Join-Path $root 'tests/adapter-mvdm-host-out/wow/edit-view')) + '"'
$units += ,@('view_fixture', 'tests/adapter-mvdm-host-out/wow/wow_edit_view_fixture.c', $viewFlags)
$units += ,@('view_wow', 'src/opennt-host/windows/core/ntuser/rtl/wow.c', $viewFlags)
$units += ,@('view_winmgr', 'src/opennt-host/windows/core/ntuser/rtl/winmgr.c', $viewFlags)
$units += ,@('view_client', 'src/opennt-host/windows/core/ntuser/client/winmgrc.c', $viewFlags)
# Compile the complete admitted body group together. This target neither
# links a substitute fixture nor enables the unfinished product path.
foreach ($name in @('editec','edecrare','editsl','editml','edmlonce','edmlrare','edslrare',
        'combo','combodir','comboini','lb1','lboxctl1','lboxctl2','lboxctl3','lboxmult','lboxrare','lboxvar',
        'dlgbegin','dlgend','dlgmgr','dlgmgrc','dlgmgr2','dmmnem','wow','winmgrc','draw','mngrayc','btnctl')) {
    $flags = '/DWOW_ORIGINAL_EDIT_CLIENT /I "' + (NinjaValue (Join-Path $root 'src/wow32-dll/include/control-client')) + '"'
    if ($name.StartsWith('dlg') -or $name -eq 'dmmnem') { $flags += ' /DWOW_ORIGINAL_DIALOG_CLIENT /DWOW_ORIGINAL_DIALOG_MANAGER' }
    if ($name -eq 'dlgend') { $flags += ' /DWOW_ORIGINAL_DIALOG_END' }
    if ($name -eq 'dlgmgr') { $flags += ' /DWOW_ORIGINAL_DIALOG_DEFAULT' }
    if ($name -eq 'btnctl') { $flags += ' /DWOW_ORIGINAL_BUTTON_CLIENT' }
    $units += ,@(('original_' + $name), ('src/opennt-host/windows/core/ntuser/client/' + $name + '.c'), $flags)
    $controlObjects += 'obj/original_' + $name + '.obj'
}
$units += ,@('original_keyboard', 'src/opennt-host/windows/core/ntuser/client/dlgmgr2.c',
    ('/DWOW_ORIGINAL_EDIT_CLIENT /DWOW_ORIGINAL_DIALOG_CLIENT /DWOW_ORIGINAL_DIALOG_MANAGER /DWOW_ORIGINAL_DIALOG_KEYBOARD /I "' +
    (NinjaValue (Join-Path $root 'src/wow32-dll/include/control-client')) + '"'))
$controlObjects += 'obj/original_keyboard.obj'
$units += ,@('original_default', 'src/opennt-host/windows/core/ntuser/client/clmsg.c',
    ('/DWOW_ORIGINAL_DEFAULT_WINDOW /I "' +
    (NinjaValue (Join-Path $root 'src/wow32-dll/include/control-client')) + '"'))
$controlObjects += 'obj/original_default.obj'
$units += ,@('default_view', 'src/opennt-host/windows/core/ntuser/rtl/wow.c', $navigationFlags)
foreach ($unit in $units) {
    if (!(Test-Path -LiteralPath (Join-Path $root $unit[1]) -PathType Leaf)) {
        throw "Missing fixture input: $($unit[1])"
    }
}
$includes = @('src', 'src/wow32-dll/include', 'src/ntvdm-exe/wow/include',
    'src/ntvdm-exe/softpc/include', 'src/opennt-abi/host-compat/include',
    'src/opennt-abi/source/public/internal/windows/inc',
    'src/opennt-host/public/sdk/inc') | ForEach-Object {
        '/I "' + (NinjaValue (Join-Path $root $_)) + '"'
    }
New-Item -ItemType Directory -Force -Path $build, (Join-Path $build 'obj'), (Join-Path $build 'bin') | Out-Null
$environment = Join-Path $build 'msvc-x86.cmd'
$launcher = @('@echo off', 'set "T447_S3_FIXTURE_CWD=%CD%"',
    ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%', 'cd /d "%T447_S3_FIXTURE_CWD%"',
    '%*', 'exit /b %errorlevel%')
[IO.File]::WriteAllLines($environment, $launcher, [Text.Encoding]::ASCII)
$lines = [Collections.Generic.List[string]]::new()
$lines.Add('ninja_required_version = 1.10')
$lines.Add('environment = ' + (NinjaValue $environment))
$lines.Add('cflags = /nologo /TC /MT /O2 /W4 /Gz /DSTD_CALL /DUNICODE /D_UNICODE /showIncludes ' + ($includes -join ' '))
$lines.Add('rule cc')
$lines.Add('  command = cmd /d /c ""$environment" '+$compilerCommand+' $cflags $extra /c /Fo$out $in"')
$lines.Add('  deps = msvc')
$lines.Add('rule link')
$lines.Add('  command = cmd /d /c ""$environment" link /nologo /machine:x86 /out:"$out" $in user32.lib gdi32.lib $libs"')
foreach ($unit in $units) {
    $lines.Add('build obj/' + $unit[0] + '.obj: cc ' + (NinjaPath (Join-Path $root $unit[1])))
    $lines.Add('  extra = ' + $unit[2])
}
$lines.Add('build bin/wow_dialog_call_fixture.exe: link obj/dialog_fixture.obj obj/dialog_adapter.obj')
$lines.Add('build bin/wow_style_notification_probe.exe: link obj/style_notification_probe.obj')
$lines.Add('build style-notification-research: phony bin/wow_style_notification_probe.exe')
$lines.Add('build bin/wow_window_client_message_fixture.exe: link obj/window_access_fixture.obj obj/window_access_adapter.obj')
$lines.Add('build window-access-boundary: phony bin/wow_window_client_message_fixture.exe')
$lines.Add('build bin/wow_dialog_getset_fixture.exe: link obj/getset_fixture.obj obj/getset_reader.obj obj/getset_client.obj obj/getset_server.obj obj/getset_callproc.obj obj/window_access_adapter.obj obj/control_extent.obj')
$lines.Add('build original-getset-boundary: phony bin/wow_dialog_getset_fixture.exe')
$lines.Add('build bin/wow_edit_call_fixture.exe: link obj/edit_fixture.obj obj/edit_adapter.obj obj/control_extent.obj obj/state_guards.obj obj/color_client.obj')
$lines.Add('build bin/wow_edit_memory_callbacks_fixture.exe: link obj/memory_fixture.obj obj/dialog_memory.obj')
$lines.Add('build bin/wow_control_brush_fixture.exe: link obj/brush_fixture.obj obj/brush_adapter.obj obj/brush_owner.obj obj/brush_client.obj')
$lines.Add('build bin/wow_default_window_binding_fixture.exe: link obj/default_fixture.obj obj/default_adapter.obj')
$lines.Add('build bin/wow_default_original_fixture.exe: link obj/default_original_fixture.obj obj/original_default.obj obj/default_view.obj obj/string_conversion.obj obj/string_services.obj')
$lines.Add('  libs = ntdll.lib')
$lines.Add('build original-default-boundary: phony bin/wow_default_original_fixture.exe')
$lines.Add('build bin/wow_dialog_context_fixture.exe: link obj/dialog_context_fixture.obj obj/dialog_context_adapter.obj')
$lines.Add('build bin/wow_dialog_creation_entry_fixture.exe: link obj/dialog_creation_fixture.obj obj/dialog_creation_adapter.obj')
$lines.Add('build bin/wow_dialog_keyboard_entry_fixture.exe: link obj/keyboard_entry_fixture.obj obj/keyboard_entry_adapter.obj')
$lines.Add('build dialog-keyboard-boundary: phony bin/wow_dialog_keyboard_entry_fixture.exe')
$lines.Add('build bin/wow_dialog_mnemonic_lifetime_fixture.exe: link obj/mnemonic_lifetime_fixture.obj obj/original_dmmnem.obj obj/button_lifetime_adapter.obj obj/state_drawing_atlas.obj obj/button_resources_table.obj')
$lines.Add('build original-mnemonic-lifetime-boundary: phony bin/wow_dialog_mnemonic_lifetime_fixture.exe')
$lines.Add('build bin/wow_dialog_keyboard_lifetime_fixture.exe: link obj/keyboard_lifetime_fixture.obj obj/keyboard_unreached.obj obj/original_keyboard.obj obj/original_dlgmgr.obj obj/button_lifetime_adapter.obj obj/state_drawing_atlas.obj obj/button_resources_table.obj obj/string_conversion.obj obj/string_services.obj')
$lines.Add('  libs = ntdll.lib')
$lines.Add('build original-keyboard-lifetime-boundary: phony bin/wow_dialog_keyboard_lifetime_fixture.exe')
$lines.Add('build bin/wow_dialog_finalization_fixture.exe: link obj/dialog_finalization_fixture.obj obj/dialog_finalization_adapter.obj')
$lines.Add('build bin/wow_dialog_end_original_fixture.exe: link obj/dialog_end_fixture.obj obj/original_dlgend.obj')
$lines.Add('build original-dialog-end-boundary: phony bin/wow_dialog_end_original_fixture.exe')
$lines.Add('build bin/wow_dialog_lifetime_fixture.exe: link obj/modal_lifetime_fixture.obj obj/modal_lifetime_body.obj obj/modal_lifetime_end.obj obj/modal_lifetime_pointer.obj')
$lines.Add('build original-dialog-modal-lifetime: phony bin/wow_dialog_lifetime_fixture.exe')
$lines.Add('build bin/wow_dialog_creator_lifetime_fixture.exe: link obj/creator_lifetime_fixture.obj obj/original_dlgbegin.obj obj/string_owner.obj obj/string_conversion.obj obj/string_services.obj')
$lines.Add('  libs = ntdll.lib')
$lines.Add('build original-dialog-creator-lifetime: phony bin/wow_dialog_creator_lifetime_fixture.exe')
$lines.Add('build bin/wow_dialog_modal_entry_fixture.exe: link obj/dialog_modal_fixture.obj obj/dialog_modal_adapter.obj obj/dialog_modal_native_unreached.obj')
$lines.Add('  libs = shell32.lib')
$lines.Add('build dialog-modal-boundary: phony bin/wow_dialog_modal_entry_fixture.exe')
$lines.Add('build bin/wow_control_string_fixture.exe: link obj/string_fixture.obj obj/string_owner.obj obj/string_conversion.obj obj/string_services.obj obj/string_prefix.obj obj/string_extent.obj')
$lines.Add('  libs = ntdll.lib')
$lines.Add('build bin/wow_text_measure_failure_fixture.exe: link obj/measure_failure_fixture.obj obj/measure_failure_body.obj obj/string_prefix.obj')
$lines.Add('build original-string-boundaries: phony bin/wow_control_string_fixture.exe bin/wow_text_measure_failure_fixture.exe')
$lines.Add('build bin/wow_mnemonic_drawing_fixture.exe: link obj/mnemonic_fixture.obj obj/mnemonic_body.obj')
$lines.Add('build original-mnemonic-drawing-boundaries: phony bin/wow_mnemonic_drawing_fixture.exe')
$lines.Add('build bin/wow_button_drawing_fixture.exe: link obj/push_fixture.obj obj/push_owner.obj obj/push_rect.obj')
$lines.Add('build original-button-drawing-boundaries: phony bin/wow_button_drawing_fixture.exe')
$lines.Add('build bin/wow_button_resources_fixture.exe: link obj/button_resources_fixture.obj obj/button_resources_body.obj obj/button_resources_table.obj')
$lines.Add('build original-button-resource-boundaries: phony bin/wow_button_resources_fixture.exe')
$lines.Add('build bin/wow_control_draw_state_fixture.exe: link obj/state_drawing_fixture.obj obj/state_drawing_adapter.obj obj/state_drawing_body.obj obj/state_drawing_brushes.obj obj/state_drawing_atlas.obj obj/button_resources_table.obj obj/state_drawing_text.obj obj/string_extent.obj obj/string_services.obj')
$lines.Add('build original-state-drawing-boundaries: phony bin/wow_control_draw_state_fixture.exe')
$lines.Add('build bin/wow_button_lifetime_fixture.exe: link obj/button_lifetime_fixture.obj obj/button_lifetime_adapter.obj obj/button_blit_owner.obj obj/button_lifetime_resources.obj obj/button_resources_table.obj')
$lines.Add('build bin/wow_button_dc_fixture.exe: link obj/button_dc_fixture.obj obj/button_dc_owner.obj obj/button_dc_unreached.obj')
$lines.Add('build original-button-lifetime-boundaries: phony bin/wow_button_lifetime_fixture.exe bin/wow_button_dc_fixture.exe')
$lines.Add('build bin/wow_dialog_navigation_fixture.exe: link obj/navigation_fixture.obj obj/navigation_graph.obj obj/navigation_help.obj obj/navigation_borders.obj obj/original_dlgmgrc.obj obj/original_dlgmgr2.obj obj/original_wow.obj obj/original_winmgrc.obj')
$lines.Add('build dialog-navigation-boundaries: phony bin/wow_dialog_navigation_fixture.exe')
$lines.Add('build bin/wow_edit_view_fixture.exe: link obj/view_fixture.obj obj/view_wow.obj obj/view_winmgr.obj obj/view_client.obj')
$lines.Add('build original-window-view-boundaries: phony bin/wow_edit_view_fixture.exe')
$lines.Add('build bin/wow_cpd_class_normalization_probe.exe: link obj/cpd_class_probe.obj')
$lines.Add('build cpd-normalization-research: phony bin/wow_cpd_class_normalization_probe.exe')
$lines.Add('build bin/wow_class_address_owner_probe.exe: link obj/class_address_probe.obj')
$lines.Add('build class-address-research: phony bin/wow_class_address_owner_probe.exe')
$lines.Add('build bin/wow_window_destroy_event_probe.dll: link obj/destroy_event_dll.obj')
$lines.Add('  libs = /DLL /IMPLIB:obj/destroy_event_probe.lib')
$lines.Add('build bin/wow_window_destroy_event_probe.exe: link obj/destroy_event_probe.obj')
$lines.Add('build destroy-event-research: phony bin/wow_window_destroy_event_probe.exe bin/wow_window_destroy_event_probe.dll')
$lines.Add('build bin/wow_cpd_window_normalization_probe.exe: link obj/cpd_window_probe.obj')
$lines.Add('build cpd-window-research: phony bin/wow_cpd_window_normalization_probe.exe')
$lines.Add('build bin/wow_button_focus_service_probe.exe: link obj/button_focus_probe.obj')
$lines.Add('build button-focus-research: phony bin/wow_button_focus_service_probe.exe')
$lines.Add('build dialog-context-boundaries: phony bin/wow_dialog_context_fixture.exe bin/wow_dialog_creation_entry_fixture.exe bin/wow_dialog_finalization_fixture.exe')
$lines.Add('build call-boundaries: phony bin/wow_dialog_call_fixture.exe bin/wow_edit_call_fixture.exe bin/wow_edit_memory_callbacks_fixture.exe bin/wow_control_brush_fixture.exe bin/wow_default_window_binding_fixture.exe')
$lines.Add('default call-boundaries')
$lines.Add('build bin/wow_dialog_native_owner_probe.exe: link obj/dialog_native_owner_probe.obj obj/dialog_procedure_adapter.obj obj/native_getset_reader.obj obj/native_getset_client.obj obj/native_getset_server.obj')
$lines.Add('build dialog-native-owner-research: phony bin/wow_dialog_native_owner_probe.exe')
$lines.Add('build bin/wow_dialog_initialization_fixture.exe: link obj/dialog_initialization_fixture.obj obj/dialog_initialization_adapter.obj')
$lines.Add('build dialog-initialization-boundary: phony bin/wow_dialog_initialization_fixture.exe')
$lines.Add('build original-control-bodies: phony ' + ($controlObjects -join ' '))
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $lines, [Text.UTF8Encoding]::new($false))
Write-Output "Configured supplemental call-boundary graph: $build (not built or run)"
