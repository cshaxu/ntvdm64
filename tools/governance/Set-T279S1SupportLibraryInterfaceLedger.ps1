[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'

$groups = @(
    @{ Path='oemuni/file.c'; Owner='direct public Win32'; Shape='original public Win32W call and original OEM conversion ordering'; X64='direct public Win32; HANDLE remains host-local'; Failure='original GetLastError/INVALID_HANDLE_VALUE or BOOL/DWORD return retained'; Symbols='CreateDirectoryW,CreateFileW,DeleteFileW,FindClose,FindFirstFileW,FindNextFileW,GetComputerNameW,GetDiskFreeSpaceW,GetDriveTypeW,GetFileAttributesW,GetVolumeInformationW,MoveFileExW,OutputDebugStringA,RemoveDirectoryW,RemoveFontResourceW,SetFileAttributesW,SetLastError' },
    @{ Path='oemuni/file.c'; Owner='adapter-mvdm-host-out/win32'; Shape='NT4 TEB/PEB and RTL string/heap/PEB-lock facade retains original declaration forms'; X64='adapter-mvdm-host-out/win32 keeps native pointers local; no pointer enters an OEM/guest 32-bit field'; Failure='map NTSTATUS through original BaseSetLastNTError macro and preserve caller result'; Symbols='ARGUMENT_PRESENT,NtCurrentPeb,NtCurrentTeb,RtlAcquirePebLock,RtlAllocateHeap,RtlEqualUnicodeString,RtlFreeAnsiString,RtlFreeHeap,RtlFreeOemString,RtlFreeUnicodeString,RtlGetCurrentDirectory_U,RtlGetFullPathName_U,RtlInitAnsiString,RtlInitString,RtlInitUnicodeString,RtlMoveMemory,RtlOemStringToUnicodeString,RtlProcessHeap,RtlReleasePebLock,RtlSetCurrentDirectory_U,RtlUnicodeStringToAnsiString,RtlUnicodeStringToOemString' },
    @{ Path='oemuni/process.c'; Owner='direct public Win32'; Shape='original public Win32W/A process/path/environment call and OEM conversion ordering'; X64='direct public Win32; PROCESS_INFORMATION/HANDLE values remain host-local'; Failure='original zero/BOOL/DWORD and GetLastError behavior retained'; Symbols='CreateProcessW,ExpandEnvironmentStrings,GetEnvironmentVariableA,GetShortPathNameW,GetSystemDirectoryW,GetTempFileNameW,GetTempPathW,GetWindowsDirectoryW,SearchPathW,SetEnvironmentVariableA,SetLastError' },
    @{ Path='oemuni/process.c'; Owner='adapter-mvdm-host-out/win32'; Shape='NT4 TEB and RTL string/heap facade retains original declaration forms'; X64='adapter-mvdm-host-out/win32 owns native temporary pointers and PEB/TEB access'; Failure='map NTSTATUS through original BaseSetLastNTError macro and preserve caller result'; Symbols='ARGUMENT_PRESENT,ASSERT,NtCurrentTeb,RtlAllocateHeap,RtlAnsiStringToUnicodeString,RtlFreeAnsiString,RtlFreeHeap,RtlFreeUnicodeString,RtlInitString,RtlInitUnicodeString,RtlMoveMemory,RtlOemStringToUnicodeSize,RtlOemStringToUnicodeString,RtlProcessHeap,RtlUnicodeStringToAnsiString,RtlUnicodeStringToOemString' },
    @{ Path='suballoc/suballoc.c'; Owner='native CRT plus original caller callback contract -> adapter-mvdm-host-out/softpc'; Shape='original malloc/RtlZeroMemory plus CommitRoutine, DecommitRoutine and MoveMemRoutine function-pointer contract'; X64='allocator metadata is host-local; BaseAddress/Size remain numeric ULONG values; caller binds callbacks through adapter-mvdm-host-out/softpc'; Failure='original FALSE/NULL callback and allocation failure result retained'; Symbols='malloc,RtlZeroMemory,CommitRoutine,DecommitRoutine,MoveMemRoutine' }
)

$rows = @(); $sequence = 0
foreach ($group in $groups) {
    $source = Join-Path $root ('src/mvdm-support/' + $group.Path)
    $text = Get-Content -LiteralPath $source
    foreach ($symbol in $group.Symbols.Split(',')) {
        $hits = @($text | Select-String -Pattern ('\b' + [regex]::Escape($symbol) + '\b'))
        if (!$hits.Count) { throw "Source-form symbol not found: $($group.Path):$symbol" }
        $sequence++
        $rows += [pscustomobject]@{
            interface_id = ('MVDM-T279-S1-IF-{0:d3}' -f $sequence)
            caller_path = $group.Path
            symbol = $symbol
            source_lines = ($hits | ForEach-Object LineNumber) -join ','
            original_shape = $group.Shape
            unique_owner = $group.Owner
            x86_x64_disposition = $group.X64
            failure_disposition = $group.Failure
            state = 'source-form-audited; build-not-enabled'
            evidence = ('src/mvdm-support/' + $group.Path + ':' + (($hits | Select-Object -First 1).LineNumber))
        }
    }
}

$out = Join-Path $ops 'm0-t279-s1-support-library-interface-ledger.tsv'
$rows | Export-Csv -LiteralPath $out -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "Wrote $($rows.Count) original support-library interface rows."
