param(
    [string]$Executable = 'O:/winnt/tests/S40USER/ccpu-user.exe',
    [string]$LogRoot = 'O:/winnt/logs'
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$media = Join-Path $root 'src/mvdm/wow16/user/k/daytona/USER.EXE'
$hash = '6746088F2A87CD366E4FF5DA424B39B5BD91586D5F7570AFBF3C05BCA94E935C'
if ((Get-FileHash $media).Hash -ne $hash) { throw 'Unrecognized original USER.EXE' }
$saved = $env:NTVDM_TEST_USER_IMAGE
$process = $null
$stdout = Join-Path $LogRoot 's40-user-handle-validator.stdout.log'
$stderr = Join-Path $LogRoot 's40-user-handle-validator.stderr.log'
try {
    $env:NTVDM_TEST_USER_IMAGE = $media
    $process = Start-Process -FilePath $Executable -WindowStyle Hidden -PassThru `
        -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    if (!$process.WaitForExit(15000)) { throw 'Original USER validator timed out' }
    $process.Refresh()
    $text = Get-Content -Raw $stdout
    if ($process.ExitCode -ne 0 -or
        $text -notmatch 'CCPU_ORIGINAL_USER_HANDLE_VALIDATOR_OK cases=10' -or
        $text -notmatch 'CCPU_HIGH_LINEAR_PAGE_BOUNDARY_OK') {
        throw "Original USER validation failed; inspect $stdout and $stderr"
    }
    if ((Get-FileHash $media).Hash -ne $hash) { throw 'Guest media changed' }
    $text
    Get-FileHash $Executable
} finally {
    if ($process -and !$process.HasExited) { Stop-Process -Id $process.Id }
    $env:NTVDM_TEST_USER_IMAGE = $saved
}
