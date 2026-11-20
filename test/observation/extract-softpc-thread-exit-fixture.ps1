param(
    [Parameter(Mandatory)][string]$RepositoryRoot,
    [Parameter(Mandatory)][string]$OutputRoot
)

$source = Join-Path $RepositoryRoot 'src/mvdm-host/softpc.new/host/src/nt_thred.c'
$text = Get-Content -LiteralPath $source -Raw
$exitBody = [regex]::Matches($text, '(?ms)^VOID\r?\nWINAPI\r?\nhost_ExitThread\(.*?^\}')
if ($exitBody.Count -ne 1) { throw 'Expected one original host_ExitThread definition' }
[IO.File]::WriteAllText((Join-Path $OutputRoot 'softpc_thread_exit.inc'), $exitBody[0].Value)
