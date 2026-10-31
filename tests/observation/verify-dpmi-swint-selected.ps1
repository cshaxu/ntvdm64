param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$CompilerDriver
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'A fresh repository build output directory is required'
}
$driver = (Resolve-Path -LiteralPath $CompilerDriver).Path
$worker = (Resolve-Path "$repo/build/.ninja/msvc-x86-release/worker").Path
$source = "$repo/src/mvdm/dpmi32/dpmiint.c"
$fixture = "$repo/tests/mvdm-host/dpmi_swint_selected_fixture.c"
New-Item -ItemType Directory $out | Out-Null
$command = & ninja.exe -C $worker -t commands obj/dpmi/dpmiint.obj | Select-Object -Last 1
if ($LASTEXITCODE -or !$command.Contains('/Foobj/dpmi/dpmiint.obj')) { throw 'Selected compile command missing' }
# Function-section selection is test-only. The production object remains intact.
$command = $command.Replace('/Foobj/dpmi/dpmiint.obj', ('/Gy /Fo"' + $out + '\dpmiint.obj"'))
[IO.File]::WriteAllText("$out/compile.cmd", $command, [Text.Encoding]::ASCII)
Push-Location $worker
try { & $driver cmd.exe /d /c "$out/compile.cmd" *> "$out/compile.log"; if ($LASTEXITCODE) { throw 'Selected DPMI source compile failed' } }
finally { Pop-Location }
& $driver cl.exe /nologo /W3 /MT /Gy "/Fo$out/fixture.obj" $fixture /link /nologo /opt:ref "/out:$out/fixture.exe" "/map:$out/fixture.map" "$out/dpmiint.obj" kernel32.lib user32.lib *> "$out/link.log"
if ($LASTEXITCODE) { throw 'Selected DPMI fixture link failed' }
& "$out/fixture.exe" | Tee-Object -FilePath "$out/result.txt"
if ($LASTEXITCODE -or !(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_SELECTED_OBJECT_OK absent=1 v86=1 stack_failure=1')) {
    throw 'Selected DPMI fixture failed'
}
& "$out/fixture.exe" --wow-signature-mode | Tee-Object -FilePath "$out/wow-signature-mode.txt"
if ($LASTEXITCODE -or !(Select-String -LiteralPath "$out/wow-signature-mode.txt" -SimpleMatch 'DPMI_INT2A_MODE_OWNER_OK v86=declined real=declined protected=handled')) {
    throw 'Selected DPMI INT 2A mode ownership fixture failed'
}
& "$out/fixture.exe" --pm-fault-failure | Tee-Object -FilePath "$out/pm-fault-failure.txt"
if ($LASTEXITCODE -or !(Select-String -LiteralPath "$out/pm-fault-failure.txt" -SimpleMatch 'DPMI_PM_FAULT_FAILURE_ROLLBACK_OK absent=1 first=1 nested=1')) {
    throw 'Selected DPMI fault failure rollback fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_HANDLER_WIDTH_OK app=2 handler=2 stack=2')) {
    throw 'Selected DPMI handler-width fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_STACK_LIMIT_OK normal=2 expand=2 granular=2')) {
    throw 'Selected DPMI stack-limit fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_NULL_MAPPING_OK fallback=1 state=unchanged')) {
    throw 'Selected DPMI null-mapping fixture failed'
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
& "$out/fixture.exe" --pm-fault-vector14 | Tee-Object -FilePath "$out/pm-fault-result.txt"
if ($LASTEXITCODE -or !(Select-String -LiteralPath "$out/pm-fault-result.txt" -SimpleMatch 'DPMI_PM_VECTOR14_FRAME_OK')) {
    throw 'Selected DPMI protected fault-vector fixture failed'
}
$nonCcpuCommand = $command.Replace('/DCCPU', '/UCCPU').Replace('dpmiint.obj"', 'dpmiint-nonccpu.obj"')
if ($nonCcpuCommand -eq $command -or !$nonCcpuCommand.Contains('/UCCPU') -or !$nonCcpuCommand.Contains('dpmiint-nonccpu.obj')) {
    throw 'Could not construct non-CCPU negative compile'
}
[IO.File]::WriteAllText("$out/compile-nonccpu.cmd", $nonCcpuCommand, [Text.Encoding]::ASCII)
Push-Location $worker
try { & $driver cmd.exe /d /c "$out/compile-nonccpu.cmd" *> "$out/compile-nonccpu.log"; if ($LASTEXITCODE) { throw 'Non-CCPU DPMI source compile failed' } }
finally { Pop-Location }
& $driver dumpbin.exe /symbols "$out/dpmiint-nonccpu.obj" *> "$out/nonccpu-symbols.log"
if ($LASTEXITCODE -or (Select-String -LiteralPath "$out/nonccpu-symbols.log" -Pattern 'DpmiV86SoftwareInt')) {
    throw 'CCPU-only V86 symbol leaked into non-CCPU object'
}
Write-Output 'DPMI_NON_CCPU_V86_ISOLATION_OK'
@($source, $fixture, "$out/dpmiint.obj", "$out/fixture.exe", "$out/fixture.map") |
    Get-FileHash -Algorithm SHA256 | Export-Csv -NoTypeInformation "$out/inputs.csv"
