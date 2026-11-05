$ErrorActionPreference = 'Stop'

$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$tool = Get-Content (Join-Path $repository 'tools\probe\Invoke-T95S7RuntimeTraceObservation.ps1') -Raw
foreach ($required in @(
        '[switch]$DeferredStartupPlan',
        '[ValidateRange(1,10)][int]$BenchmarkMillions = 1',
        '-benchmark ''+$BenchmarkMillions+'' -log bochs.log',
        'benchmarkMillions=$BenchmarkMillions',
        'New-Object byte[] 512',
        '$optionRom[0]=0x55; $optionRom[1]=0xaa; $optionRom[2]=0x01',
        '$optionRom[3]=0x0f; $optionRom[4]=0x0b; $optionRom[5]=0xf4',
        '$optionRom[511]=0xf2',
        'E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB',
        'machine_startup_handoff',
        'optromimage1: file=ROM/runner-post-handoff.rom, address=0xca800')) {
    if (-not $tool.Contains($required)) {
        throw "Missing native POST composition input term: $required"
    }
}
foreach ($forbidden in @('floppy: 0_1_44=', 'ata0-master:', 'ata0-slave:',
        'diskc:', 'host_path', 'CreateFile')) {
    if ($tool -match [regex]::Escape($forbidden)) {
        throw "Composition runner contains forbidden expansion term: $forbidden"
    }
}
Write-Output 'Native POST handoff composition input verification passed.'
