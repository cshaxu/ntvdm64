param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Nasm = 'nasm.exe'
)
# An owned, private-desktop witness for the retained original HDC document
# route.  It assembles only an independently authored NE probe; it never
# rebuilds or replaces guest inputs and never selects a default/physical printer.
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root = [IO.Path]::GetFullPath($BuildRoot)
if (!$root.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $root)) { throw 'Require a fresh build-owned evidence root' }
$null = New-Item -ItemType Directory -Path $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime = $null; $scope = $null; $process = $null
$saved = @{}
foreach ($name in @('TEMP', 'TMP', 'PATH', 'MVDM_OBSERVER_PRIVATE_DESKTOP',
        'MVDM_WOW_CALLBACK_FRAME_TRACE_PATH')) {
    $saved[$name] = [Environment]::GetEnvironmentVariable($name)
}
try {
    $assembler = (Get-Command $Nasm -ErrorAction Stop).Source
    $source = Join-Path $PSScriptRoot 'win16_print_probe.asm'
    & $assembler ('-DOUTPUT_DIR="' + $root + '"') -f bin -o "$root/PRNPROBE.EXE" -l "$root/probe.lst" $source *> "$root/assemble.txt"
    if ($LASTEXITCODE) { throw 'Independent print probe assembly failed' }

    $runtime = New-PhysicalRuntimeStage (Resolve-Path -LiteralPath $PackageRoot).Path
    $scope = New-IsolatedPackageScope $runtime
    $binary = Get-PackageBinaryRoot $runtime
    $spool = Join-Path $binary 'WINSPOOL.EXE'
    if (!(Test-Path -LiteralPath $spool) -or
            (Get-IsolatedFileSha256 $spool) -ne 'E4E787340F124476090ED0F22837D7D7ACAB37FD7BBE49DB46CAE9AC4944D535') {
        throw 'Require the approved immutable WINSPOOL companion in the isolated package'
    }
    $tests = Join-Path $runtime 'tests'
    $null = New-Item -ItemType Directory -Path $tests -Force
    Copy-Item -LiteralPath "$root/PRNPROBE.EXE" -Destination "$tests/PRNPROBE.EXE"
    @($source, "$root/PRNPROBE.EXE", $spool, (Join-Path $binary 'WOW32.DLL')) |
        Get-FileHash | Select-Object Path, Hash | ConvertTo-Json | Set-Content "$root/inputs.json"

    $env:TEMP = $runtime; $env:TMP = $runtime
    $env:PATH = "$binary;" + $env:SystemRoot + '\System32'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP = '1'
    $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH = "$root/callbacks.txt"
    $start = [Diagnostics.ProcessStartInfo]::new((Resolve-Path -LiteralPath $Observer).Path)
    $start.UseShellExecute = $false; $start.CreateNoWindow = $true
    foreach ($arg in @((Join-Path $binary 'run16.exe'), $tests, "$root/observer.txt", '--wait',
            "$tests/PRNPROBE.EXE", '--observation-timeout-ms', '40000')) { $start.ArgumentList.Add($arg) }
    $process = [Diagnostics.Process]::Start($start); $null = $process.Handle
    @{ pid = $process.Id; start = $process.StartTime.ToUniversalTime().ToString('o');
       arguments = @($start.ArgumentList); timeoutSeconds = 45 } |
        ConvertTo-Json | Set-Content "$root/process.json"
    if (!$process.WaitForExit(45000)) { throw 'Owned print observer exceeded deadline' }
    $process.Refresh()
    if (Test-Path "$tests/PRNPROBE.BIN") { Copy-Item "$tests/PRNPROBE.BIN" "$root/guest-report.bin" }
    if ($process.ExitCode -or (Get-Content "$root/observer.txt" -Raw) -notmatch 'result=exited') {
        throw 'Guest print probe did not complete normally'
    }
    $bytes = [IO.File]::ReadAllBytes("$root/guest-report.bin")
    if ($bytes.Length -ne 80) { throw 'Expected four 20-byte print records' }
    $names = @('phase','errors','normalCalls','cancelCalls','cancelled','deleted','freed','task')
    $rows = @()
    foreach ($record in 0..3) {
        $offset = 20 * $record
        if ([Text.Encoding]::ASCII.GetString($bytes, $offset, 4) -ne 'PRN1') { throw 'Bad print report signature' }
        $row = [ordered]@{}
        for ($i = 0; $i -lt $names.Count; $i++) { $row[$names[$i]] = [BitConverter]::ToUInt16($bytes, $offset + 4 + 2 * $i) }
        $rows += [pscustomobject]$row
    }
    $rows | ConvertTo-Json | Set-Content "$root/guest-report.json"
    $last = $rows[-1]
    if (($rows.phase -join ',') -ne '0,1,2,2' -or @($rows | Where-Object errors -ne 0).Count -or
            $last.normalCalls -lt 1 -or $last.cancelCalls -lt 1 -or $last.cancelled -ne 1 -or
            $last.deleted -ne 2 -or $last.freed -ne 1 -or !$last.task) { throw 'Print callback/return/cleanup contract failed' }
    $pdf = [IO.File]::ReadAllBytes("$root/normal.pdf")
    if ($pdf.Length -lt 5 -or [Text.Encoding]::ASCII.GetString($pdf, 0, 5) -ne '%PDF-') { throw 'Normal guest print produced no PDF' }
    if (Test-Path "$root/cancel.pdf") { throw 'Cancelled guest print unexpectedly produced output' }
    $trace = Get-Content "$root/callbacks.txt" -Raw
    $entries = [regex]::Matches($trace, '(?m)^(\d+) before ret=0022 .*task=([0-9A-Fa-f]+) ')
    if ($entries.Count -ne ($last.normalCalls + $last.cancelCalls)) { throw 'Original abort callback count mismatch' }
    foreach ($entry in $entries) {
        if ([Convert]::ToInt32($entry.Groups[2].Value, 16) -ne $last.task -or
                $trace -notmatch ('(?m)^' + $entry.Groups[1].Value + ' after ret=0022 .*task=' + $entry.Groups[2].Value + ' ')) {
            throw 'Original abort callback did not return on its owning task'
        }
    }
    'PASS real Win16 original HDC print/abort normal-cancel cleanup witness' | Set-Content "$root/result.txt"
    '0' | Set-Content "$root/result.exit"
} catch {
    $_ | Out-String | Set-Content "$root/failure.txt"; '1' | Set-Content "$root/result.exit"; throw
} finally {
    if ($process) { if (!$process.HasExited) { $process.Kill(); $process.WaitForExit(5000) | Out-Null }; $process.Dispose() }
    try { if ($scope) { Stop-IsolatedPackageScope $scope } }
    finally { try { if ($runtime) { Remove-PhysicalRuntimeStage $runtime } }
    finally { foreach ($name in $saved.Keys) { [Environment]::SetEnvironmentVariable($name, $saved[$name]) } } }
}
