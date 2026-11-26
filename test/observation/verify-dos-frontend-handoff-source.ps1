[CmdletBinding()]
param([string]$RepositoryRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)))

$ErrorActionPreference = 'Stop'

# This is a source-contract guard, not runtime or transport acceptance.
function Read-Source([string]$Path) {
    Get-Content -LiteralPath (Join-Path $RepositoryRoot $Path) -Raw
}
function Region([string]$Text, [string]$Start, [string]$End) {
    $a = $Text.IndexOf($Start, [StringComparison]::Ordinal)
    if ($a -lt 0) { throw "Missing region: $Start" }
    $b = $Text.IndexOf($End, $a + $Start.Length, [StringComparison]::Ordinal)
    if ($b -lt 0) { throw "Missing region end: $End" }
    $Text.Substring($a, $b - $a)
}
function Assert-Order([string]$Label, [string]$Text, [string[]]$Statements) {
    $cursor = 0
    foreach ($statement in $Statements) {
        $match = [regex]::Match($Text.Substring($cursor), $statement)
        if (-not $match.Success) { throw "$Label missing/out of order: $statement" }
        $cursor += $match.Index + $match.Length
    }
    "PASS source order: $Label"
}

$events = Read-Source 'src/mvdm-host/softpc.new/host/src/nt_event.c'
$exec = Read-Source 'src/mvdm-host/dos/command/cmdexec.c'
$misc = Read-Source 'src/mvdm-host/dos/command/cmdmisc.c'
$block = Region $events 'void nt_block_event_thread(ULONG BlockFlags)' 'void nt_resume_event_thread(void)'
$resume = Region $events 'void nt_resume_event_thread(void)' 'SyncBiosKbdLedToKbdDevice('
$execute = Region $exec 'VOID cmdExec32 (PCHAR pCmd32, PCHAR pEnv)' '/* cmdExecComspec32'
$create = Region $exec 'VOID cmdCreateProcess ( PSTD_HANDLES pStdHandles )' 'VOID cmdExec32 (PCHAR pCmd32, PCHAR pEnv)'

Assert-Order 'suspend before output drain and native mode restoration' $block @(
    'SetEvent\(hConsoleSuspend\)',
    'WaitForSingleObject\(hConsoleWaitStall',
    'stream_io_update\(\)',
    'ReturnUnusedKeyEvents\(UnusedKeyEvents\)',
    'ReturnBiosBufferKeys\(\)',
    'FlushMouseEvents\(\)',
    'SetConsoleMode\(sc.InputHandle,sc.OrgInConsoleMode\)',
    'SetConsoleMode\(sc.OutputHandle,sc.OrgOutConsoleMode\)',
    'SuspendTimerThread\(\)',
    'fEventThreadBlock\s*=\s*TRUE'
)
Assert-Order 'EXEC yields before native thread and original command wait' $execute @(
    'nt_block_event_thread\(0\)', 'fBlock\s*=\s*TRUE',
    'CreateThread\s*\(', 'cmdCreateProcess',
    'NO_PARENT_TO_WAKE\s*\|\s*RETURN_ON_NO_COMMAND',
    'GetNextVDMCommand\s*\(&VDMInfo\)',
    'if\s*\(VDMInfo.CmdSize\s*>\s*0\)',
    'IsRepeatCall\s*=\s*TRUE',
    'setAL\(\(UCHAR\)dwExitCode32\)', 'nt_resume_event_thread\(\)'
)
Assert-Order 'native execution remains original re-entry owner' $create @(
    'INCREMENT_REENTER_COUNT', 'GetNextVDMCommand', 'CreateProcess\s*\(',
    'mvdm_command_native_child_finish\(\)', 'ResumeThread\s*\(',
    'WaitForSingleObject\(ProcessInformation.hProcess',
    'GetExitCodeProcess', 'DECREMENT_REENTER_COUNT', 'GetNextVDMCommand'
)
# The first resume region ends at its LED-sync call; cover the final unblock
# separately so that both mode setup and timer precede the input wakeup.
Assert-Order 'DOS mode setup precedes timer restart' $resume @(
    'SetupConsoleMode\(\)', 'VDDResumeUserHook\(\)', 'ResumeTimerThread\(\)'
)
$resumeStart = $events.IndexOf('void nt_resume_event_thread(void)', [StringComparison]::Ordinal)
Assert-Order 'DOS keyboard recovery precedes input wakeup' $events.Substring($resumeStart) @(
    'SyncBiosKbdLedToKbdDevice\(\)', 'KbdResume\(\)', 'SetEvent\(hConsoleWait\)'
)
# XP retains an NEC_98 codepage block before resume. The selected CCPU
# product does not define NEC_98; do not require removal of that original code.
$queued = Region $misc 'if(!GetNextVDMCommand(&VDMInfo))' '// Sync VDMs enviornment variables'
$queued = [regex]::Replace($queued, '(?ms)^[ \t]*#if defined\(NEC_98\)\r?\n.*?^[ \t]*#endif // NEC_98[^\r\n]*\r?\n', '')
if ($queued -notmatch '(?s)if\(!GetNextVDMCommand\(&VDMInfo\)\).*?if\(fBlock\)\s*\{\s*nt_resume_event_thread\(\);\s*fBlock\s*=\s*FALSE;') {
    throw 'Queued DOS command no longer resumes the original blocked worker.'
}
'PASS source order: queued DOS resumes even while native parent remains alive'
'SOURCE ONLY: no production frontend channel or runtime behavior certified.'
