param(
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'A fresh repository build output directory is required'
}
$worker = (Resolve-Path "$repo/build/.ninja/msvc-x86-release/worker").Path
$source = "$repo/src/mvdm-host/dpmi32/dpmiint.c"
$fixture = "$repo/test/mvdm-host/dpmi_swint_selected_fixture.c"
New-Item -ItemType Directory $out | Out-Null
& "$repo/tools/build-fixtures.cmd" x86 dpmi-swint-selected *> "$out/build.log"
if ($LASTEXITCODE) { throw 'Native DPMI fixture build failed' }
$exe = "$worker/dpmi-swint-selected.exe"
& $exe | Tee-Object -FilePath "$out/result.txt"
if ($LASTEXITCODE -or !(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_SELECTED_OBJECT_OK absent=1 protected=1 stack_failure=1')) {
    throw 'Selected DPMI fixture failed'
}
& $exe --wow-signature-mode | Tee-Object -FilePath "$out/wow-signature-mode.txt"
if ($LASTEXITCODE -or !(Select-String -LiteralPath "$out/wow-signature-mode.txt" -SimpleMatch 'DPMI_INT2A_MODE_OWNER_OK v86=declined real=ivt protected=declined')) {
    throw 'Selected DPMI INT 2A mode ownership fixture failed'
}
& $exe --pm-fault-failure | Tee-Object -FilePath "$out/pm-fault-failure.txt"
if ($LASTEXITCODE -or !(Select-String -LiteralPath "$out/pm-fault-failure.txt" -SimpleMatch 'DPMI_PM_FAULT_FAILURE_ROLLBACK_OK absent=1 first=1 nested=1')) {
    throw 'Selected DPMI fault failure rollback fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_HANDLER_WIDTH_OK app=2 handler=2 stack=2')) {
    throw 'Selected DPMI handler-width fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_STACK_LIMIT_OK normal=2 expand=2 granular=2')) {
    throw 'Selected DPMI stack-limit fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_INVALID_ALIAS_OK host_failure=1 no_dispatch=1')) {
    throw 'Selected DPMI invalid-provider alias fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_NONSELECTOR_NULL_OK prior_failure=preserved')) {
    throw 'Selected DPMI non-selector null-mapping fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_FAULT_IRET32_SELECTED_OBJECT_OK stacks=2 nesting=2 registers=full')) {
    throw 'Selected DPMI fault-return fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_INT_IRET32_SELECTED_OBJECT_OK stacks=2 nesting=2 frame=full')) {
    throw 'Selected DPMI interrupt-return fixture failed'
}
& $exe --pm-fault-vector14 | Tee-Object -FilePath "$out/pm-fault-result.txt"
if ($LASTEXITCODE -or !(Select-String -LiteralPath "$out/pm-fault-result.txt" -SimpleMatch 'DPMI_PM_VECTOR14_FRAME_OK')) {
    throw 'Selected DPMI protected fault-vector fixture failed'
}
@($source, $fixture, "$worker/obj/tests/dpmi-swint-owner.obj", $exe, "$exe.map") |
    Get-FileHash -Algorithm SHA256 | Export-Csv -NoTypeInformation "$out/inputs.csv"

exit 0
