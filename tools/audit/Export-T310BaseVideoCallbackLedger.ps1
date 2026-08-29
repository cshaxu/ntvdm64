[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$CompilerLog,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

$ErrorActionPreference = 'Stop'
$rows = [System.Collections.Generic.List[object]]::new()
$number = 0
foreach ($line in Get-Content -LiteralPath $CompilerLog) {
    if ($line -notmatch '(?:src/mvdm-host/softpc\.new/base/video|generated/video)/([^\(]+)\((\d+)\): warning (C4113|C4133|C4047): (.+)$') {
        continue
    }
    $sourceName = $matches[1]
    $sourceLine = [int]$matches[2]
    $diagnostic = $matches[3]
    $contract = $matches[4]
    $number += 1
    $recovery = switch ($sourceName) {
        'egawrtm0.c' { 'same-file delayed prototype; generated source carrier may restore exact definition signature' }
        'egwrtm12.c' { 'same-file delayed prototype; generated source carrier may restore exact definition signature' }
        'gfx_updt.c' { 'polymorphic marking callback; generated typed wrapper must preserve ignored/consumed arguments' }
        'ega_writ.c' {
            if ($contract -match 'IU32,IHPE,IU32,IBOOL') { 'CVIDC EVID-to-MEM handler bridge; requires checked guest-address contract before enablement' }
            else { 'BIOS chain-write callback; recover exact prototype from original callee before enablement' }
        }
        'vga_mode.c' { 'display update callback; recover source-defined T_calc_update signature before enablement' }
        default { 'source-derived typed carrier required before runtime enablement' }
    }
    $rows.Add([pscustomobject]@{
        id = ('T310-S8-P4-VIDEO-{0:D3}' -f $number)
        source_path = ('src/mvdm-host/softpc.new/base/video/' + $sourceName)
        source_line = $sourceLine
        diagnostic = $diagnostic
        compiler_contract = $contract
        disposition = $recovery
    })
}
if ($rows.Count -eq 0) { throw "No base/video function-pointer ABI diagnostics found: $CompilerLog" }
$directory = Split-Path -Parent $OutputPath
if ($directory) { [IO.Directory]::CreateDirectory($directory) | Out-Null }
$header = "id`tsource_path`tsource_line`tdiagnostic`tcompiler_contract`tdisposition"
$body = foreach ($row in $rows) {
    @($row.id, $row.source_path, $row.source_line, $row.diagnostic, $row.compiler_contract, $row.disposition) -join "`t"
}
[IO.File]::WriteAllLines($OutputPath, @($header) + @($body), [Text.UTF8Encoding]::new($false))
Write-Output ("rows={0}" -f $rows.Count)
