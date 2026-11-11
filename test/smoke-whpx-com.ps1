param(
    [Parameter(Mandatory = $true)][string]$Dispatcher,
    [Parameter(Mandatory = $true)][string]$Runner
)

$program = Join-Path $env:TEMP ("whpx-com-" + [guid]::NewGuid() + ".com")
$stdout = $program + ".out"
$stderr = $program + ".err"
try {
    # AH=02 prints Hi; AH=09 reads DS:DX until '$'; then AH=4C exits with 7.
    [byte[]]$code = 0xB2,0x48,0xB4,0x02,0xCD,0x21,0xB2,0x69,0xB4,0x02,0xCD,0x21,0xBA,0x18,0x01,0xB4,0x09,0xCD,0x21,0xB8,0x07,0x4C,0xCD,0x21,0x20,0x74,0x68,0x65,0x72,0x65,0x24
    [System.IO.File]::WriteAllBytes($program, $code)
    $start = [System.Diagnostics.ProcessStartInfo]::new()
    $start.FileName = $Dispatcher
    $start.Arguments = '--engine "' + $Runner + '" "' + $program + '"'
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true
    $process = [System.Diagnostics.Process]::new()
    $process.StartInfo = $start
    if (-not $process.Start()) {
        throw "WHPX COM runner did not start"
    }
    if (-not $process.WaitForExit(5000)) {
        Stop-Process -Id $process.Id -Force
        throw "WHPX COM runner timed out: stdout=$(Get-Content $stdout -Raw -ErrorAction SilentlyContinue) stderr=$(Get-Content $stderr -Raw -ErrorAction SilentlyContinue)"
    }
    $standardOutput = $process.StandardOutput.ReadToEnd()
    $standardError = $process.StandardError.ReadToEnd()
    $exitCode = $process.ExitCode
    $output = $standardOutput + $standardError
    if ($exitCode -ne 7 -or $output -notmatch "Hi there") {
        throw "WHPX COM runner failed: exit=$exitCode output=$output"
    }
}
finally {
    Remove-Item -LiteralPath $program -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $stdout -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $stderr -Force -ErrorAction SilentlyContinue
}
