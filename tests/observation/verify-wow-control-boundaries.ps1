[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot '../..'),
    [string]$BuildRoot = 'build/.ninja/msvc-x86-release/call-boundaries',
    [Parameter(Mandatory=$true)][string]$EvidenceRoot,
    [switch]$CpdWindowResearch,
    [switch]$StyleNotificationResearch,
    [switch]$DialogNativeOwnerResearch,
    [switch]$ClassAddressResearch,
    [string]$VsDevCmd = 'C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat'
)
# Supplemental whole-cohort validation, not product/guest acceptance.
# Invoke this script as an owned hidden background process. All descendants
# are joined; a failed build never executes stale fixture binaries.
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if (([int]$CpdWindowResearch.IsPresent + [int]$StyleNotificationResearch.IsPresent + [int]$DialogNativeOwnerResearch.IsPresent + [int]$ClassAddressResearch.IsPresent) -gt 1) { throw 'Select one research cohort.' }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$allowed = [IO.Path]::GetFullPath((Join-Path $root 'build')) + [IO.Path]::DirectorySeparatorChar
function BuildPath([string]$path) {
    if (![IO.Path]::IsPathRooted($path)) { $path = Join-Path $root $path }
    $path = [IO.Path]::GetFullPath($path)
    if (!$path.StartsWith($allowed, [StringComparison]::OrdinalIgnoreCase)) {
        throw 'All outputs must remain below repository build/.'
    }
    return $path
}
$build = BuildPath $BuildRoot
$logs = BuildPath $EvidenceRoot
if (Test-Path -LiteralPath $logs) { throw 'Evidence directory must be new; do not overwrite a prior run.' }
New-Item -ItemType Directory -Force -Path $logs | Out-Null
$result = 1
function RunOwned([string]$name, [string]$exe, [string[]]$arguments, [int]$seconds) {
    $start = [DateTime]::UtcNow
    $params = @{FilePath=$exe; WorkingDirectory=$build; WindowStyle='Hidden'; PassThru=$true;
        RedirectStandardOutput="$logs/$name.stdout.log"; RedirectStandardError="$logs/$name.stderr.log"}
    if ($arguments.Count) { $params.ArgumentList = $arguments }
    $p = Start-Process @params
    $null = $p.Handle
    Write-Output "$name PID=$($p.Id) start=$($start.ToString('o')) timeout_seconds=$seconds"
    if (!$p.WaitForExit($seconds * 1000)) {
        & taskkill.exe /PID $p.Id /T /F | Out-Host
        throw "Owned $name exceeded its actual deadline"
    }
    Write-Output "$name exit=$($p.ExitCode) elapsed_seconds=$(([DateTime]::UtcNow-$start).TotalSeconds)"
    return $p.ExitCode
}
try {
    & "$root/tools/build/New-T447S3CallBoundaryNinja.ps1" -RepositoryRoot $root -BuildRoot $build -VsDevCmd $VsDevCmd
    # Include untracked staged sources: git diff alone omits new original slices.
    $sourceRoots = @('src/wow32-dll','src/opennt-host/windows/core/ntuser','src/opennt-abi/host-compat/include','tests/adapter-mvdm-host-out/wow','tools/build/New-T447S3CallBoundaryNinja.ps1')
    $deletedPaths = @(& git -C $root ls-files -d -- @sourceRoots)
    if ($LASTEXITCODE) { throw 'Cannot enumerate deleted source inputs.' }
    $paths = @(& git -C $root ls-files -co --exclude-standard -- @sourceRoots) | Sort-Object -Unique
    if ($LASTEXITCODE) { throw 'Cannot enumerate current source inputs.' }
    # A tracked removal is part of the input state, not a file to hash. Record
    # it explicitly; an unexpected missing current input still fails hashing.
    $deletedPaths | Set-Content -LiteralPath "$logs/source-deleted-paths.txt"
    $paths = @($paths | Where-Object { $_ -notin $deletedPaths })
    $paths | ForEach-Object { Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $root $_) } |
        Export-Csv -NoTypeInformation -LiteralPath "$logs/source-sha256.csv"
    Get-FileHash -Algorithm SHA256 -LiteralPath "$build/build.ninja",$VsDevCmd |
        Export-Csv -NoTypeInformation -LiteralPath "$logs/build-sha256.csv"
    $targets = @('original-control-bodies','original-getset-boundary','window-access-boundary','original-dialog-creator-lifetime','original-dialog-modal-lifetime','original-dialog-end-boundary','original-default-boundary','call-boundaries','dialog-context-boundaries','dialog-keyboard-boundary','original-mnemonic-lifetime-boundary','original-keyboard-lifetime-boundary',
        'original-string-boundaries','original-mnemonic-drawing-boundaries','original-button-drawing-boundaries','original-button-resource-boundaries','original-state-drawing-boundaries',
        'dialog-navigation-boundaries','original-window-view-boundaries','original-button-lifetime-boundaries')
    $targets += 'dialog-native-owner-research'
    $targets += 'dialog-initialization-boundary'
    if ($CpdWindowResearch) { $targets = @('cpd-window-research') }
    if ($StyleNotificationResearch) { $targets = @('style-notification-research') }
    if ($DialogNativeOwnerResearch) { $targets = @('dialog-native-owner-research') }
    if ($ClassAddressResearch) { $targets = @('class-address-research') }
    $buildResult = @(RunOwned 'build' (Get-Command ninja.exe).Source (@('-C',$build,'-j','4','-k','0') + $targets) 300)
    $buildResult | Write-Output
    $result = [int]$buildResult[-1]
    if ($result -eq 0) {
        $cases = @('wow_dialog_getset_fixture','wow_window_client_message_fixture','wow_dialog_creator_lifetime_fixture','wow_dialog_lifetime_fixture','wow_dialog_call_fixture','wow_edit_call_fixture','wow_edit_memory_callbacks_fixture',
                'wow_control_brush_fixture','wow_default_window_binding_fixture','wow_dialog_context_fixture',
                'wow_dialog_creation_entry_fixture','wow_dialog_finalization_fixture','wow_dialog_end_original_fixture','wow_dialog_keyboard_entry_fixture','wow_dialog_mnemonic_lifetime_fixture','wow_dialog_keyboard_lifetime_fixture',
                'wow_control_string_fixture','wow_text_measure_failure_fixture','wow_mnemonic_drawing_fixture',
                'wow_default_original_fixture','wow_button_drawing_fixture','wow_button_resources_fixture','wow_control_draw_state_fixture',
                'wow_dialog_navigation_fixture','wow_edit_view_fixture','wow_button_lifetime_fixture','wow_button_dc_fixture')
        $cases += 'wow_dialog_native_owner_probe'
        $cases += 'wow_dialog_initialization_fixture'
        if ($CpdWindowResearch) { $cases = @('wow_cpd_window_normalization_probe') }
        if ($StyleNotificationResearch) { $cases = @('wow_style_notification_probe') }
        if ($DialogNativeOwnerResearch) { $cases = @('wow_dialog_native_owner_probe') }
        if ($ClassAddressResearch) { $cases = @('wow_class_address_owner_probe') }
        foreach ($name in $cases) {
            $testResult = @(RunOwned $name "$build/bin/$name.exe" @() 30)
            $testResult | Write-Output
            if ([int]$testResult[-1] -ne 0) { $result = 1 }
        }
    }
} catch { Write-Output $_; $result = 1 }
[IO.File]::WriteAllText("$logs/exit.txt", [string]$result)
exit $result
