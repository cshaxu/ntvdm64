param([string]$OriginalRoot = 'O:/repos.external/OpenNT',
    [string]$RepositoryRoot = (Split-Path (Split-Path $PSScriptRoot)))
$ErrorActionPreference = 'Stop'
# Source audit only: never execute the out-of-bounds original loop.
function Read-ObjectSource([string]$relative, [string]$hash) {
    $p = Join-Path $OriginalRoot $relative
    if ((Get-FileHash $p).Hash -ne $hash) { throw "Original identity changed: $relative" }
    [IO.File]::ReadAllText($p).Replace("`r`n","`n")
}
function Read-ObjectMirror([string]$relative) {
    $p=Join-Path $RepositoryRoot ('src/opennt-host/'+$relative)
    $t=[IO.File]::ReadAllText($p)
    if ([regex]::IsMatch($t,'(?<!\r)\n')) { throw "Mirror is not CRLF: $relative" }
    $t.Replace("`r`n","`n")
}
$handlePath='windows/core/ntuser/kernel/handtabl.c'
$handleSource=Read-ObjectSource $handlePath 'F0B56314A93423599F06645D17104391F19A0CA07C3944853B065D8866493F7C'
$tablePattern='(?s)CONST BYTE gabObjectCreateFlags\[TYPE_CTYPES\] = \{.*?\n\};'
$originalTable=[regex]::Match($handleSource,$tablePattern).Value
$mirrorTable=[regex]::Match((Read-ObjectMirror $handlePath),$tablePattern).Value
if (!$originalTable -or $originalTable -cne $mirrorTable) { throw 'Object ownership table differs from OpenNT' }
$typesPath='windows/core/ntuser/inc/user.h'
$typesSource=Read-ObjectSource $typesPath 'ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1'
$typesPattern='(?ms)^#define TYPE_FREE.*?^#define TYPE_GENERIC[^\n]*'
if ([regex]::Match($typesSource,$typesPattern).Value -cne
    [regex]::Match((Read-ObjectMirror $typesPath),$typesPattern).Value) {
    throw 'Object type declarations differ from OpenNT'
}
$flagsPath='windows/core/ntuser/kernel/userk.h'
$flagsSource=Read-ObjectSource $flagsPath '9260E0BA3CC89974E10B3804ED66D18B9864F4AE7D1175CCE6DDE8EE79D66E8C'
$flagsPattern='(?m)^#define OCF_[^\n]+'
if (([regex]::Matches($flagsSource,$flagsPattern).Value -join "`n") -cne
    ([regex]::Matches((Read-ObjectMirror $flagsPath),$flagsPattern).Value -join "`n")) {
    throw 'Object ownership flag declarations differ from OpenNT'
}
$path = Join-Path $OriginalRoot 'windows/core/ntuser/kernel/cleanup.c'
$hash = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
if ($hash -ne 'DFB0FB8D81AFEB555B3631ADC5456F4CEB13142F2E954A6F2B22631C49142E5C') {
    throw 'Original cleanup.c identity changed; re-audit required'
}
$source = [IO.File]::ReadAllText($path)
$classPath = Join-Path $OriginalRoot 'windows/core/ntuser/kernel/class.c'
if ((Get-FileHash -LiteralPath $classPath -Algorithm SHA256).Hash -ne
    '82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A') {
    throw 'Original class.c identity changed; re-audit required'
}
$classSource = [IO.File]::ReadAllText($classPath)
$destroy = [regex]::Match($classSource, '(?ms)^void DestroyClass\(\s*PPCLS ppcls\)\s*\{.*?^\}')
if (!$destroy.Success -or !$destroy.Value.Contains('*ppcls = pcls->pclsNext;')) {
    throw 'Original class destruction no longer proves unconditional unlinking'
}
# Both cleanup class loops rely on DestroyClass itself advancing the link.
# A BOOL native adapter that leaves a failed node linked is not interchangeable
# with that void original contract. Do not execute an unbounded native retry.
$destroySites = [regex]::Matches($source,
    'if \(\(\*ppcls\)->cWndReferenceCount == 0\)\s*\{\s*DestroyClass\(ppcls\);')
if ($destroySites.Count -ne 2) { throw 'Cleanup destruction sites changed' }
$array = [regex]::Match($source, '(?s)static PROC spfnwp\[\]\s*=\s*\{(.*?)\};')
if (!$array.Success) { throw 'Missing original procedure table' }
$count = [regex]::Matches($array.Groups[1].Value, '(?m)^\s*0\s*,?\s*//').Count
if ($count -ne 13 -or
    !$source.Contains('iSel < sizeof(spfnwp)') -or
    !$source.Contains('iSel == sizeof(spfnwp)')) {
    throw 'Original loop assumptions changed'
}
# The formal provider is x86: PROC occupies four bytes. Check index domains
# symbolically, without reading any adjacent native memory.
$byteBound = $count * 4
$firstUnsafeProcedureIndex = $count
if ($byteBound -ne 52 -or $firstUnsafeProcedureIndex -ge $byteBound) {
    throw 'Failed to demonstrate byte-count/index-count mismatch'
}
# Check the full recovered owner, reversing only registered textual edits.
$marker = "/*`n * Determines if a Wow wndproc"
$original = $source.Replace("`r`n","`n")
$mirror = [IO.File]::ReadAllText((Join-Path $RepositoryRoot 'src/opennt-host/windows/core/ntuser/kernel/cleanup.c'))
if ([regex]::IsMatch($mirror,'(?<!\r)\n')) { throw 'Cleanup mirror must retain CRLF' }
$body = $mirror.Replace("`r`n","`n")
$body = $body.Substring($body.IndexOf($marker)).TrimEnd()
$cleanupBody = $body.Substring($body.IndexOf('BOOL _WOWCleanup('))
if ([regex]::Matches($cleanupBody,'return FALSE;').Count -ne 4 -or
    [regex]::Matches($cleanupBody,'return TRUE;').Count -ne 3 -or
    !$cleanupBody.Contains("if (SelListTemp == NULL)`n            return FALSE;") -or
    !$cleanupBody.Contains("UserFreePool(SelListTemp);`n            return FALSE;")) {
    throw 'Cleanup failure/result contract changed'
}
$cleanupBody = $body.Substring($body.IndexOf('BOOL _WOWCleanup('))
if ([regex]::Matches($cleanupBody,'return FALSE;').Count -ne 4 -or
    [regex]::Matches($cleanupBody,'return TRUE;').Count -ne 3 -or
    !$cleanupBody.Contains("if (SelListTemp == NULL)`n            return FALSE;") -or
    !$cleanupBody.Contains("UserFreePool(SelListTemp);`n            return FALSE;")) {
    throw 'Cleanup failure/result contract changed'
}
$body = $body.Replace(', wow_cleanup_context *context)', ')')
$body = $body.Replace('PseudoDestroyClassWindows(pwnd, pcls, context);', 'PseudoDestroyClassWindows(pwnd, pcls);')
$body = $body.Replace('PseudoDestroyClassWindows(PtiCurrent()->rpdesk->pDeskInfo->spwnd, *ppcls, context);',
    'PseudoDestroyClassWindows(PtiCurrent()->rpdesk->pDeskInfo->spwnd, *ppcls);')
$body = $body.Replace('BOOL _WOWCleanup(', 'VOID _WOWCleanup(')
$body = $body.Replace("    DWORD nSel,`n    wow_cleanup_context *context)", '    DWORD nSel)')
$body = $body.Replace('sizeof(spfnwp) / sizeof(spfnwp[0])', 'sizeof(spfnwp)')
$body = $body.Replace("if (!DestroyClass(ppcls))`n                            return FALSE;", 'DestroyClass(ppcls);')
if (!$body.EndsWith("    return TRUE;`n}")) { throw 'Missing successful cleanup return' }
$body = $body.Substring(0,$body.Length-"    return TRUE;`n}".Length) + '}'
$cleanupStart = $body.IndexOf('VOID _WOWCleanup(')
$body = $body.Substring(0,$cleanupStart) + $body.Substring($cleanupStart).Replace('return FALSE;', 'return;').Replace('return TRUE;', 'return;')
if ($body -cne $original.Substring($original.IndexOf($marker)).TrimEnd()) {
    throw 'Cleanup owner differs outside registered OPENNT-HOST-062 changes'
}
[pscustomobject]@{
    SourceHash = $hash
    PointerBytes = 4
    Entries = $count
    OriginalIterationBound = $byteBound
    FirstUnsafeProcedureIndex = $firstUnsafeProcedureIndex
    CorrectedIterationBound = $count
    Classification = 'confirmed-original-host-source-defect'
    RuntimeReachability = 'not-yet-proved'
    MemoryAccess = 'symbolic-only'
    DestructionSitesAssumingUnlink = $destroySites.Count
    NativeReleaseFailure = 'requires-explicit-integration-handling-not-infinite-retry'
    RecoveredWholeBody = 'matches-pinned-source-with-registered-adaptations'
    ObjectClassification = 'original-handtabl-and-original-type-and-flag-declarations'
} | Format-List
