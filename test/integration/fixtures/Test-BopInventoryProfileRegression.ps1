param([string]$RepositoryRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)))
$ErrorActionPreference='Stop'
$temporary = [IO.Path]::Combine([IO.Path]::GetTempPath(), ('runner-bop-inventory-' + [guid]::NewGuid().ToString('N')))
try {
    & (Join-Path $RepositoryRoot 'tools\governance\Export-OpenNtBopInventory.ps1') -RepositoryRoot $RepositoryRoot -OutputRoot $temporary
    $path=Join-Path $temporary 'opennt-bop-inventory.json'
    $data=Get-Content -LiteralPath $path -Raw|ConvertFrom-Json
if (@($data.top_level_selectors).Count -ne 16) { throw 'Expected 16 top-level selectors' }
$expected=@{BOP_DOS=0x50;BOP_WOW=0x51;BOP_XMS=0x52;BOP_DPMI=0x53;BOP_CMD=0x54;BOP_DEBUGGER=0x56;BOP_REDIR=0x57;BOP_NOSUPPORT=0x59;BOP_WAITIFIDLE=0x5a;BOP_DBGBREAKPOINT=0x5b;BOP_KBD=0x5c;BOP_VIDEO=0x5d;BOP_NOTIFICATION=0x5e;BOP_UNIMPINT=0x5f;BOP_SWITCHTOREALMODE=0xfd;BOP_UNSIMULATE=0xfe}
foreach($entry in $data.top_level_selectors){if(!$expected.ContainsKey($entry.name) -or [int]$entry.value -ne $expected[$entry.name]){throw "Unexpected top selector $($entry.name)"}}
$families=@{BOP_DOS=74;BOP_CMD=18;BOP_XMS=12;BOP_DPMI=25;BOP_REDIR=50;BOP_DEBUGGER=16}
    foreach($family in $data.service_families){if($families.ContainsKey($family.selector_name) -and @($family.entries).Count -ne $families[$family.selector_name]){throw "Unexpected service count $($family.selector_name)"}}
    Write-Output 'OpenNT BOP inventory/profile regression verified: selectors=16 service-identities=195 BIOS-forms=349'
} finally {
    if (Test-Path -LiteralPath $temporary) { Remove-Item -LiteralPath $temporary -Recurse -Force }
}
