[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Join-Path $PSScriptRoot '../..'),
    [string]$BuildRoot = 'build/.ninja/msvc-x86-release/call-boundaries-native',
    [Parameter(Mandatory=$true)][string]$EvidenceRoot,
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
if (([int]$StyleNotificationResearch.IsPresent + [int]$DialogNativeOwnerResearch.IsPresent + [int]$ClassAddressResearch.IsPresent) -gt 1) { throw 'Select one research cohort.' }
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
if ($build -ne (BuildPath 'build/.ninja/msvc-x86-release/call-boundaries-native')) {
    throw 'Use the canonical native control fixture cache.'
}
$logs = BuildPath $EvidenceRoot
if (Test-Path -LiteralPath $logs) { throw 'Evidence directory must be new; do not overwrite a prior run.' }
New-Item -ItemType Directory -Force -Path $logs | Out-Null
$result = 1
$caseResults = [Collections.Generic.List[object]]::new()
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
    $caseResults.Add([pscustomobject]@{name=$name;pid=$p.Id;exit=$p.ExitCode;
        started=$start.ToString('o');sha256=(Get-FileHash -LiteralPath $exe).Hash})
    return $p.ExitCode
}
try {
    $previousToolchain = $env:NTVDM_VSDEVCMD
    try {
        $env:NTVDM_VSDEVCMD = $VsDevCmd
        & "$root/tools/build-fixtures.cmd" x86 wow-controls *> "$logs/build.log"
        if ($LASTEXITCODE) { throw 'Native WOW control build failed; no fixtures executed.' }
    } finally { $env:NTVDM_VSDEVCMD = $previousToolchain }
    # Include untracked staged sources: git diff alone omits new original slices.
    $sourceRoots = @('src/wow32-dll','src/non-mvdm/windows/core/ntuser','src/compat','test/compat','test/wow32-dll/source','test/adapter-mvdm-host-out/wow','tools/build/wow-controls','tools/build-fixtures.cmd')
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
    $result = 0
    if ($result -eq 0) {
        $cases = @('wow_dialog_getset_fixture','wow_window_client_message_fixture','wow_dialog_creator_lifetime_fixture','wow_dialog_lifetime_fixture','wow_default_messages_fixture','wow_default_state_fixture','wow_dialog_call_fixture','wow_edit_call_fixture','wow_edit_memory_callbacks_fixture',
                'wow_control_brush_fixture','wow_default_window_binding_fixture','wow_dialog_context_fixture',
                'wow_dialog_creation_entry_fixture','wow_dialog_finalization_fixture','wow_dialog_end_original_fixture','wow_dialog_keyboard_entry_fixture','wow_dialog_mnemonic_lifetime_fixture','wow_dialog_keyboard_lifetime_fixture',
                'wow_control_string_fixture','wow_text_measure_failure_fixture','wow_mnemonic_drawing_fixture',
                'wow_default_original_fixture','wow_button_drawing_fixture','wow_button_resources_fixture','wow_control_draw_state_fixture',
                'wow_dialog_navigation_fixture','wow_edit_view_fixture','wow_button_lifetime_fixture','wow_button_dc_fixture')
        $cases += 'wow_dialog_native_owner_probe', 'wow_control_fnid_fixture', 'wow_native_controls_fixture'
        $cases += 'wow_dialog_initialization_fixture', 'wow_task_profile_fixture', 'wow_task_profile_existing_fixture'
        $cases += 'wow_edit_lookaside_fixture', 'wow_edit_conversion_fixture'
        $cases += 'wow_def_window_bits_fixture', 'wow_original_dde_free_fixture', 'wow_user_capture_fixture'
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
[pscustomobject]@{exit=$result;cases=@($caseResults.ToArray())} |
    ConvertTo-Json -Depth 4 | Set-Content "$logs/result.json"
exit $result
