$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$tool = Get-Content (Join-Path $repository 'tools\probe\Invoke-T95S6R3RealNtioStartupTransactionObservation.ps1') -Raw
$required = 'guest transaction committed cs=([0-9a-f]+) eip=([0-9a-f]+) address=([0-9a-f]+) bytes=([0-9a-f]+) resume=([0-9a-f]+)'
if (-not $tool.Contains($required)) { throw 'Observer does not require generic fault CS:EIP and commit fields.' }
if ($tool.Contains('guest transaction committed address=([0-9a-f]+) bytes=([0-9a-f]+) resume=([0-9a-f]+)')) { throw 'Observer still accepts the old three-field commit record.' }
if (-not $tool.Contains('bopObservationCount')) { throw 'Observer does not record passive BOP catalogue observations separately.' }
if ($tool.Contains("broadInterceptorObserved = `$log -match 'ntdos64 adapter ")) { throw 'Observer still conflates passive adapter BOP observations with a broad interceptor.' }
Write-Output 'Real-NTIO startup-transaction observer parser verification passed.'
