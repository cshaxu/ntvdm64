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
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_HANDLER_WIDTH_OK app=2 handler=2 stack=2')) {
    throw 'Selected DPMI handler-width fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_SWINT_STACK_LIMIT_OK normal=2 expand=2 granular=2')) {
    throw 'Selected DPMI stack-limit fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_FAULT_IRET32_SELECTED_OBJECT_OK stacks=2 nesting=2 registers=full')) {
    throw 'Selected DPMI fault-return fixture failed'
}
if (!(Select-String -LiteralPath "$out/result.txt" -SimpleMatch 'DPMI_INT_IRET32_SELECTED_OBJECT_OK stacks=2 nesting=2 frame=full')) {
    throw 'Selected DPMI interrupt-return fixture failed'
}
@($source, $fixture, "$out/dpmiint.obj", "$out/fixture.exe", "$out/fixture.map") |
    Get-FileHash -Algorithm SHA256 | Export-Csv -NoTypeInformation "$out/inputs.csv"
