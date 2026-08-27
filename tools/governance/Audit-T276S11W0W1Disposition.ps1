[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$fileLedgerPath = Join-Path $operations 'mvdm-file-recovery-ledger.tsv'
$buildLedgerPath = Join-Path $operations 'm0-t276-s2-translation-unit-build-ledger.tsv'
$callLedgerPath = Join-Path $operations 'm0-t276-s4-textual-call-candidate-ledger.tsv'
foreach ($path in @($fileLedgerPath, $buildLedgerPath, $callLedgerPath)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing T276 input: $path" }
}

$roots = @('dirs', 'makefil0', 'inc', 'oemuni', 'suballoc', 'vdmutils')
$files = @(Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" | Where-Object { $_.package_root -in $roots } | Sort-Object package_root, target_path)
$builds = @(Import-Csv -LiteralPath $buildLedgerPath -Delimiter "`t")
$calls = @(Import-Csv -LiteralPath $callLedgerPath -Delimiter "`t" | Where-Object { $_.caller_package -in $roots } | Sort-Object caller_package, caller_path, source_line, call_id)
if ($files.Count -ne 140) { throw "Expected 140 W0/W1 file rows, found $($files.Count)." }

function Get-FileDisposition {
    param([string]$Package, [string]$Path, [string]$Kind)
    if ($Package -in @('dirs', 'makefil0')) {
        return @('build-control-carrier', 'original build-control input; no runtime translation unit', 'original build description only')
    }
    if ($Package -eq 'inc') {
        return @('declaration-carrier', 'shared declaration/resource carrier; no standalone runtime object', 'S3 lexical declaration evidence')
    }
    if ($Path -in @('oemuni/file.c', 'oemuni/process.c')) {
        return @('runtime-support-candidate', 'original oemuni library source; public Win32 plus NT RTL binding review required', 'oemuni/sources: SOURCES=file.c process.c')
    }
    if ($Path -eq 'oemuni/toemuni.c') {
        return @('test-program-profile-excluded', 'original UMTEST only; not an oemuni library member', 'oemuni/sources: UMTEST=toemuni')
    }
    if ($Package -eq 'oemuni') {
        return @('runtime-support-input', 'supporting input for original oemuni library or test', 'oemuni/sources')
    }
    if ($Path -eq 'suballoc/suballoc.c') {
        return @('runtime-support-candidate', 'original suballoc library member; CRT and diagnostics binding review required', 'suballoc/sources: SOURCES=suballoc.c')
    }
    if ($Path -eq 'suballoc/tsa.c') {
        return @('test-program-profile-excluded', 'original UMTEST program; not a suballoc library member', 'suballoc/sources: UMTEST=tsa')
    }
    if ($Path -eq 'suballoc/debug.c') {
        return @('unreferenced-debug-source-profile-excluded', 'not listed in the original literal library or UMTEST source set', 'suballoc/sources')
    }
    if ($Package -eq 'suballoc') {
        return @('runtime-support-input', 'supporting input for original suballoc library or test', 'suballoc/sources')
    }
    if ($Package -eq 'vdmutils') {
        if ($Path -like 'vdmutils/pifedit/*') {
            return @('historical-gui-tool-profile-excluded', 'original PIFEDIT Windows product input, outside host-runtime profile', 'vdmutils/pifedit/sources: UMTYPE=windows')
        }
        if ($Kind -eq 'source') {
            return @('standalone-tool-profile-excluded', 'original utility program source, not a reusable VDM host runtime library', 'per-directory original sources description')
        }
        return @('tool-support-input-profile-excluded', 'original standalone tool supporting input, outside host-runtime profile', 'per-directory original sources description')
    }
    throw "No W0/W1 disposition rule for $Path"
}

$fileOut = foreach ($file in $files) {
    $membership = @($builds | Where-Object { $_.file_id -eq $file.file_id })
    $membershipText = if ($membership.Count -eq 0) { 'not-a-translation-unit' } else { ($membership.original_build_membership -join ';') }
    $rule = Get-FileDisposition -Package $file.package_root -Path $file.target_path -Kind $file.file_kind
    [pscustomobject][ordered]@{
        file_id = $file.file_id
        target_path = $file.target_path
        package_root = $file.package_root
        final_component_owner = if ($file.package_root -eq 'vdmutils') { 'mvdm-tools' } else { 'mvdm-support' }
        file_kind = $file.file_kind
        original_build_membership = $membershipText
        provisional_composition_disposition = $rule[0]
        source_role_and_reason = $rule[1]
        original_evidence = $rule[2]
        interface_state = if ($file.file_kind -eq 'source') { 'see source-unit family ledger and call-accounting ledger' } else { 'no executable source interface disposition in S11' }
        final_state = 'provisional-not-enabled'
    }
}

$sourceFamilies = @(
    @{ path='oemuni/file.c'; family='public Win32 file/path/drive APIs'; symbols='CreateDirectoryW;CreateFileW;DeleteFileW;FindFirstFileW;FindNextFileW;FindClose;GetCurrentDirectoryW;GetDiskFreeSpaceW;GetDriveTypeW;GetFileAttributesW;GetFullPathNameW;GetVolumeInformationW;MoveFileExW;MoveFileW;RemoveDirectoryW;RemoveFontResourceW;SetCurrentDirectoryW;SetFileAttributesW;SetLastError;OutputDebugStringA'; owner='adapter-win32'; state='adapter-binding-review'; reason='original OEM wrappers preserve algorithm; public calls are available but original OEM/ANSI/NT error ABI must be retained' },
    @{ path='oemuni/file.c'; family='NT RTL/PEB/TEB conversion and heap APIs'; symbols='NtCurrentPeb;NtCurrentTeb;RtlAcquirePebLock;RtlAllocateHeap;RtlEqualUnicodeString;RtlFreeAnsiString;RtlFreeHeap;RtlFreeOemString;RtlFreeUnicodeString;RtlGetCurrentDirectory_U;RtlGetFullPathName_U;RtlInitAnsiString;RtlInitString;RtlInitUnicodeString;RtlOemStringToUnicodeString;RtlProcessHeap;RtlReleasePebLock;RtlSetCurrentDirectory_U;RtlUnicodeStringToAnsiString;RtlUnicodeStringToOemString'; owner='adapter-win32'; state='adapter-binding-review'; reason='historical NTDLL-facing ABI requires same-shaped compatibility assessment before source enablement' },
    @{ path='oemuni/process.c'; family='public Win32 process/environment/path APIs'; symbols='CreateProcessW;ExpandEnvironmentStrings;GetEnvironmentVariableA;GetShortPathNameW;GetSystemDirectoryW;GetTempFileNameW;GetTempPathW;GetWindowsDirectoryW;SearchPathW;SetEnvironmentVariableA;SetLastError'; owner='adapter-win32'; state='adapter-binding-review'; reason='public APIs exist; original OEM conversion and process semantics must remain in mirror source' },
    @{ path='oemuni/process.c'; family='NT RTL/TEB conversion and heap APIs'; symbols='NtCurrentTeb;RtlAllocateHeap;RtlAnsiStringToUnicodeString;RtlFreeAnsiString;RtlFreeHeap;RtlFreeUnicodeString;RtlInitString;RtlInitUnicodeString;RtlOemStringToUnicodeSize;RtlOemStringToUnicodeString;RtlProcessHeap;RtlUnicodeStringToAnsiString;RtlUnicodeStringToOemString'; owner='adapter-win32'; state='adapter-binding-review'; reason='historical NTDLL-facing ABI requires same-shaped compatibility assessment before source enablement' },
    @{ path='oemuni/toemuni.c'; family='original console test program'; symbols='CreateDirectoryOem;CreateFileOem;DeleteFileOem;GetCurrentDirectory;GetEnvironmentVariableOem;GetSystemDirectory;GetTempFileNameOem;GetTempPathOem;GetWindowsDirectoryOem;RemoveDirectoryOem;SetCurrentDirectoryOem;SetEnvironmentVariableOem;CloseHandle;getch;printf'; owner='none'; state='profile-excluded'; reason='UMTEST only; test does not prove or enable the oemuni library' },
    @{ path='suballoc/suballoc.c'; family='CRT allocator and public diagnostic surface'; symbols='malloc;OutputDebugString;RtlZeroMemory;sprintf'; owner='adapter-win32-and-toolchain'; state='adapter-binding-review'; reason='original library member; CRT allocation ABI and optional diagnostic binding require explicit x86/x64 review' },
    @{ path='suballoc/tsa.c'; family='original suballoc stress-test native virtual-memory APIs'; symbols='NtAllocateVirtualMemory;NtCurrentProcess;NtFreeVirtualMemory;RtlMoveMemory;malloc;free;printf'; owner='none'; state='profile-excluded'; reason='UMTEST-only stress program; no runtime enablement follows from its source presence' },
    @{ path='suballoc/debug.c'; family='empty/debug-only source carrier'; symbols='none'; owner='none'; state='profile-excluded'; reason='not listed in original SOURCES or UMTEST membership' },
    @{ path='vdmutils/forcedos/forcedos.c'; family='standalone FORCEDOS console/process utility'; symbols='CreateProcess;WaitForSingleObject;GetExitCodeProcess;GetStdHandle;WriteFile;GetCurrentDirectory;GetEnvironmentVariable;GetFileAttributes;SearchPath;SetCurrentDirectory;ExitProcess;NT RTL string conversion'; owner='none'; state='profile-excluded'; reason='original UMAPPL console utility, not a reusable host provider' },
    @{ path='vdmutils/graftabl/graftabl.c'; family='standalone GRAFTABL console/code-page utility'; symbols='CharToOem;GetConsoleOutputCP;SetConsoleOutputCP;LoadString;console CRT'; owner='none'; state='profile-excluded'; reason='original UMAPPL console utility, not a reusable host provider' },
    @{ path='vdmutils/pifedit/mods.c'; family='PIFEDIT GUI support'; symbols='GetTextExtent;GetTextExtentPoint;memcpy;unlink'; owner='none'; state='profile-excluded'; reason='support object of historical PIFEDIT Windows product' },
    @{ path='vdmutils/pifedit/pifedit.c'; family='PIFEDIT GUI, shell and common-dialog product surface'; symbols='User32/GDI32;Shell32;Comdlg32;WinHelp;drag/drop;profile APIs;legacy window/data APIs'; owner='none'; state='profile-excluded'; reason='historical PIFEDIT Windows product, outside MVDM host runtime profile' },
    @{ path='vdmutils/pifedit/pifsubs.c'; family='PIFEDIT file/dialog support'; symbols='OpenFile;_lread;_lwrite;_llseek;_lclose;User32 dialog APIs;GetEnvironmentVariable'; owner='none'; state='profile-excluded'; reason='support object of historical PIFEDIT Windows product' },
    @{ path='vdmutils/win/win.c'; family='standalone WIN launcher utility'; symbols='CreateProcess;WaitForSingleObject;GetExitCodeProcess;CloseHandle;GetLastError;RtlZeroMemory'; owner='none'; state='profile-excluded'; reason='original UMAPPL Windows .COM launcher, not a reusable host provider' }
)
$familyOut = foreach ($family in $sourceFamilies) {
    [pscustomobject][ordered]@{
        source_path = $family.path
        original_interface_family = $family.family
        representative_symbols = $family.symbols
        proposed_owner_or_nonowner = $family.owner
        provisional_interface_state = $family.state
        source_derived_reason = $family.reason
        evidence = "$($family.path); original sources description where applicable"
    }
}

$runtimeSupportOut = @(
    [pscustomobject][ordered]@{
        binding_id = 'MVDM-W1-BIND-001'
        package_root = 'oemuni'
        original_translation_unit = 'oemuni/file.c'
        original_consumer_evidence = 'softpc.new/obj.vdm/sources:91; wow32/sources:20 link oemuni.lib'
        original_interface_shape = 'OEM public wrappers declared by inc/oemuni.h; internal source uses NTSTATUS, STRING/UNICODE_STRING, NtCurrentPeb/NtCurrentTeb and Rtl heap/conversion calls'
        required_owner = 'adapter-win32'
        binding_disposition = 'same-shaped historical Win32/NTDLL binding review required'
        retained_source_rule = 'retain file.c algorithm and public entrypoint names unchanged; no source-body fork or custom OEM algorithm'
        prerequisite = 'exact type/calling-convention/error-conversion and temporary-string-lifetime audit for each imported NT RTL helper'
        state = 'adapter-pending-not-enabled'
    }
    [pscustomobject][ordered]@{
        binding_id = 'MVDM-W1-BIND-002'
        package_root = 'oemuni'
        original_translation_unit = 'oemuni/process.c'
        original_consumer_evidence = 'softpc.new/obj.vdm/sources:91; wow32/sources:20 link oemuni.lib'
        original_interface_shape = 'OEM process/environment/path wrappers declared by inc/oemuni.h; source preserves OEM-to-Unicode conversion, STARTUPINFOA/PROCESS_INFORMATION and SetLastError mapping'
        required_owner = 'adapter-win32'
        binding_disposition = 'same-shaped historical Win32/NTDLL binding review required'
        retained_source_rule = 'retain process.c conversion/order/error logic and original entrypoint names; no session/COMMAND logic enters oemuni'
        prerequisite = 'exact CreateProcess/environment/current-directory and RTL string/heap ABI/error mapping audit for x86 and x64'
        state = 'adapter-pending-not-enabled'
    }
    [pscustomobject][ordered]@{
        binding_id = 'MVDM-W1-BIND-003'
        package_root = 'suballoc'
        original_translation_unit = 'suballoc/suballoc.c'
        original_consumer_evidence = 'softpc.new/obj.vdm/sources:90 links suballoc.lib'
        original_interface_shape = 'inc/suballoc.h exports SAInitialize/SAQueryFree/SAAllocate/SAFree/SAReallocate with ULONG address/size and caller-supplied NTSTATUS commit/decommit plus overlapping move callbacks'
        required_owner = 'toolchain binding; adapter-softpc only at original machine-facing callers'
        binding_disposition = 'original algorithm-library candidate; no direct Bochs or guest semantic binding in library'
        retained_source_rule = 'retain 32-bit ULONG callback contract and source algorithm; do not reinterpret BaseAddress as a host pointer or insert mapping-manager logic'
        prerequisite = 'compile-header/CRT allocation review and separate caller-side adapter-softpc audit of commit/decommit/move callbacks'
        state = 'binding-pending-not-enabled'
    }
)

$callOut = foreach ($call in $calls) {
    $sourceFile = @($fileOut | Where-Object { $_.target_path -eq $call.caller_path })
    if ($sourceFile.Count -ne 1) { throw "No W0/W1 file row for call $($call.call_id)." }
    [pscustomobject][ordered]@{
        call_id = $call.call_id
        caller_file_id = $call.caller_file_id
        caller_path = $call.caller_path
        caller_package = $call.caller_package
        source_line = $call.source_line
        lexical_callee = $call.lexical_callee
        original_correlation = $call.correlation
        declaration_candidate_ids = $call.declaration_candidate_ids
        file_provisional_disposition = $sourceFile[0].provisional_composition_disposition
        call_accounting_state = 'accounted-lexically; ABI/provider form remains governed by source-unit family disposition or later package review'
        evidence = $call.evidence
    }
}

function Export-Tsv {
    param([object[]]$Rows, [string]$Path)
    $Rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath $Path -Encoding utf8
}
Export-Tsv -Rows $fileOut -Path (Join-Path $operations 'm0-t276-s11-w0-w1-file-disposition-ledger.tsv')
Export-Tsv -Rows $familyOut -Path (Join-Path $operations 'm0-t276-s11-w0-w1-source-interface-family-ledger.tsv')
Export-Tsv -Rows $callOut -Path (Join-Path $operations 'm0-t276-s11-w0-w1-call-accounting-ledger.tsv')
Export-Tsv -Rows $runtimeSupportOut -Path (Join-Path $operations 'm0-t276-s11-w0-w1-runtime-support-binding-ledger.tsv')

Write-Output "T276 S11 W0/W1 audit generated: $($fileOut.Count) files, $($familyOut.Count) source-interface families, $($callOut.Count) lexical call candidates, $($runtimeSupportOut.Count) runtime-support bindings."
