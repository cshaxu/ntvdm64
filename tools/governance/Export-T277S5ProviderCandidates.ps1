[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$source = Join-Path $root 'src/mvdm-host'
$work = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s2-source-form-worklist.tsv') -Delimiter "`t" | Where-Object { $_.caller_package -eq 'softpc.new' })
if ($work.Count -ne 197) { throw "Expected 197 SoftPC work items; found $($work.Count)." }

function CandidateClass($relativePath) {
    if ($relativePath -match '^softpc\.new/base/(ccpu386|cvidc|system|video|comms|disks|keymouse)/') { return 'Bochs-replacement-executor-candidate' }
    if ($relativePath -match '^softpc\.new/host/src/') { return 'host-control-provider-candidate' }
    if ($relativePath -match '^dos/') { return 'MVDM-host-provider-candidate' }
    if ($relativePath -match '^dbg/') { return 'debugger-provider-candidate' }
    if ($relativePath -match '^v86/') { return 'monitor-or-product-shell-candidate' }
    if ($relativePath -match '^dpmi32/') { return 'DPMI-provider-candidate' }
    if ($relativePath -match '^inc/') { return 'declaration-carrier-candidate' }
    return 'other-source-candidate'
}

$files = @(Get-ChildItem -LiteralPath $source -Recurse -File | Where-Object { $_.Extension -in @('.c','.cc','.cpp','.h','.asm','.s') })
$rows = foreach ($symbol in ($work.symbol | Sort-Object -Unique)) {
    $pattern = "(?i)\b$([regex]::Escape($symbol))\s*\("
    $hits = @($files | Select-String -Pattern $pattern)
    if (!$hits) {
        [pscustomobject]@{ symbol=$symbol; candidate_path=''; candidate_line=''; candidate_class='no-lexical-provider-candidate'; source_text=''; evidence_state='manual-source-form-review-required' }
        continue
    }
    foreach ($hit in $hits) {
        $relative = $hit.Path.Substring($source.Length + 1).Replace('\','/')
        [pscustomobject]@{
            symbol=$symbol
            candidate_path=$relative
            candidate_line=$hit.LineNumber
            candidate_class=CandidateClass $relative
            source_text=$hit.Line.Trim()
            evidence_state='lexical-candidate-only; definition-versus-call-or-declaration-not-inferred'
        }
    }
}
$rows | Sort-Object symbol,candidate_path,candidate_line | Export-Csv -LiteralPath (Join-Path $ops 'm0-t277-s5-softpc-provider-candidate-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "Wrote $($rows.Count) T277 S5 lexical provider candidates for $(($work.symbol | Sort-Object -Unique).Count) symbols."
