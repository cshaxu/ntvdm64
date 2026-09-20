[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Probe,
    [Parameter(Mandatory)][string]$LogPrefix,
    [ValidatePattern('^[A-Za-z0-9]{1,8}\.COM$')][string]$ProbeName = 'D38I.COM',
    [string]$PackageRoot = 'O:\winnt',
    [string]$LogRoot = 'O:\winnt\logs',
    [ValidateSet(16,32)][int]$ClientBits = 16,
    [switch]$Reflection,
    [switch]$VcdService,
    [switch]$DebugGroups,
    [switch]$DebugRollback,
    [switch]$Int21State,
    [switch]$FileFamily,
    [switch]$FindFamily,
    [switch]$DirectoryNavigation,
    [switch]$PspFamily,
    [switch]$CreatePsp,
    [switch]$ExecFailure,
    [switch]$ExecInherit,
    [string]$ExecChildProbe,
    [switch]$ExecLoadOnly,
    [switch]$ExecLoadCall,
    [switch]$NetworkName,
    [switch]$NetworkAssign,
    [switch]$NetworkAssignCreate,
    [switch]$NetworkDpl,
    [switch]$NetworkSpool,
    [switch]$NetworkDplPointer,
    [switch]$NetworkPrint,
    [switch]$TsrExit,
    [switch]$BpbRefusal,
    [switch]$ParseFamily,
    [switch]$FcbFamily,
    [switch]$ExtendedFcb,
    [switch]$ExtendedOpen,
    [switch]$TempFile,
    [switch]$CountryQuery,
    [switch]$PointerQuery,
    [switch]$DosMemory,
    [switch]$DriveQuery,
    [switch]$KeyboardInput,
    [switch]$KeyboardFlush,
    [switch]$KeyboardDiscard,
    [switch]$KeyboardStatus,
    [switch]$IoctlFailure,
    [switch]$IoctlPrinter,
    [switch]$IoctlTrackMock,
    [switch]$IoctlFormatMock,
    [switch]$IoctlMediaQuery,
    [switch]$IoctlMetadataMock,
    [switch]$IoctlDeviceMock,
    [switch]$IoctlControlMock,
    [ValidateSet('normal','empty','overflow')][string]$KeyboardCase = 'normal',
    [switch]$MemoryInfo,
    [switch]$Code32,
    [switch]$LargeCount,
    [switch]$MemoryHighOffset,
    [switch]$DescriptorContract
)
$ErrorActionPreference = 'Stop'
if ($FileFamily -and !$Int21State) { throw 'FileFamily requires the INT21 probe' }
if ($FindFamily -and !$Int21State) { throw 'FindFamily requires the INT21 probe' }
if ($DirectoryNavigation -and !$FindFamily) { throw 'DirectoryNavigation requires FindFamily' }
if ($PspFamily -and !$Int21State) { throw 'PspFamily requires the INT21 probe' }
if ($CreatePsp -and !$Int21State) { throw 'CreatePsp requires the INT21 probe' }
if ($ExecFailure -and !$Int21State) { throw 'ExecFailure requires the INT21 probe' }
if ($ExecChildProbe -and !$ExecFailure) { throw 'EXEC child requires ExecFailure family' }
if ($ExecInherit -and (!$ExecChildProbe -or $ExecLoadOnly)) { throw 'ExecInherit requires an executing child, not load-only' }
if ($ExecLoadOnly -and !$ExecChildProbe) { throw 'Load-only requires the EXEC child' }
if ($ExecLoadCall -and !$ExecLoadOnly) { throw 'Loaded entry execution requires load-only' }
if ($NetworkName -and !$Int21State) { throw 'NetworkName requires the INT21 probe' }
if ($NetworkAssign -and !$Int21State) { throw 'NetworkAssign requires the INT21 probe' }
if ($NetworkAssignCreate -and !$NetworkAssign) { throw 'NetworkAssignCreate requires NetworkAssign' }
if (($NetworkDpl -or $NetworkPrint) -and !$Int21State) { throw 'Network DPL/print checks require the INT21 probe' }
if ($NetworkDplPointer -and !$NetworkDpl) { throw 'NetworkDplPointer requires NetworkDpl' }
if ($NetworkSpool -and !$NetworkDpl) { throw 'NetworkSpool requires NetworkDpl' }
if ($TsrExit -and (!$Int21State -or $KeyboardInput)) { throw 'TSR exit requires a non-keyboard INT21 probe' }
if ($BpbRefusal -and !$Int21State) { throw 'BpbRefusal requires the INT21 probe' }
if ($ParseFamily -and !$Int21State) { throw 'ParseFamily requires the INT21 probe' }
if ($FcbFamily -and !$Int21State) { throw 'FcbFamily requires the INT21 probe' }
if ($ExtendedFcb -and !$FcbFamily) { throw 'ExtendedFcb requires FcbFamily' }
if ($ExtendedOpen -and !$Int21State) { throw 'ExtendedOpen requires the INT21 probe' }
if ($TempFile -and !$Int21State) { throw 'TempFile requires the INT21 probe' }
if ($CountryQuery -and !$Int21State) { throw 'CountryQuery requires the INT21 probe' }
if ($PointerQuery -and !$Int21State) { throw 'PointerQuery requires the INT21 probe' }
if ($DosMemory -and !$Int21State) { throw 'DosMemory requires the INT21 probe' }
if ($DriveQuery -and !$Int21State) { throw 'DriveQuery requires the INT21 probe' }
if ($KeyboardInput -and !$Int21State) { throw 'KeyboardInput requires the INT21 probe' }
if ($KeyboardFlush -and !$KeyboardInput) { throw 'KeyboardFlush requires KeyboardInput' }
if ($KeyboardDiscard -and !$KeyboardInput) { throw 'KeyboardDiscard requires KeyboardInput' }
if ($KeyboardStatus -and !$KeyboardDiscard) { throw 'KeyboardStatus requires KeyboardDiscard' }
if ($IoctlFailure -and !$Int21State) { throw 'IoctlFailure requires the INT21 probe' }
if ($IoctlPrinter -and !$Int21State) { throw 'IoctlPrinter requires the INT21 probe' }
if ($IoctlTrackMock -and !$Int21State) { throw 'IoctlTrackMock requires the INT21 probe' }
if ($IoctlFormatMock -and !$Int21State) { throw 'IoctlFormatMock requires the INT21 probe' }
if ($IoctlMediaQuery -and !$Int21State) { throw 'IoctlMediaQuery requires the INT21 probe' }
if ($IoctlMetadataMock -and !$Int21State) { throw 'IoctlMetadataMock requires the INT21 probe' }
if ($IoctlDeviceMock -and !$IoctlMetadataMock) { throw 'IoctlDeviceMock requires the controlled metadata probe' }
if ($IoctlControlMock -and !$IoctlMetadataMock) { throw 'IoctlControlMock requires the controlled metadata probe' }
if ($KeyboardCase -ne 'normal' -and !$KeyboardInput) { throw 'KeyboardCase requires KeyboardInput' }
if ($DescriptorContract -and ($MemoryInfo -or $Int21State -or $Reflection -or $VcdService -or $DebugGroups -or $DebugRollback)) { throw 'Descriptor contract uses its separate probe' }
if ($Code32 -and (!$Int21State -or $ClientBits -ne 32)) { throw 'CODE32 requires the 32-bit INT21 probe' }
if ($LargeCount -and (!$Int21State -or $ClientBits -ne 32)) { throw 'LargeCount requires the 32-bit INT21 probe' }
if ($MemoryHighOffset -and (!$MemoryInfo -or $ClientBits -ne 32)) { throw 'MemoryHighOffset requires the 32-bit memory information probe' }
if ($MemoryInfo -and ($Reflection -or $VcdService -or $DebugGroups -or $DebugRollback -or $Int21State)) { throw 'Memory info uses its separate probe' }
if ($Int21State -and ($Reflection -or $VcdService -or $DebugGroups -or $DebugRollback)) { throw 'INT21 state uses its separate probe' }
$Observer = (Resolve-Path -LiteralPath $Observer).Path
$Probe = (Resolve-Path -LiteralPath $Probe).Path
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path
$LogRoot = (Resolve-Path -LiteralPath $LogRoot).Path
if ($ExecFailure -and (Test-Path -LiteralPath "$PackageRoot\tests\NOEXEC38.EXE")) { throw 'EXEC negative pathname must be absent' }
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid log prefix' }
if ($VcdService -and ($Reflection -or $ClientBits -ne 16)) { throw 'VCD uses its separate 16-bit service probe' }
if ($DebugGroups -and ($Reflection -or $VcdService -or $ClientBits -ne 16)) { throw 'Debug groups use their separate 16-bit probe' }
if ($DebugRollback -and ($Reflection -or $VcdService -or $DebugGroups -or $ClientBits -ne 16)) { throw 'Debug rollback uses its separate 16-bit probe' }
$paths = @('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object { Join-Path $PackageRoot $_ }
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object { $_.ExecutablePath -in $paths })
}
if ((PackageProcesses).Count) { throw 'Package already in use' }
if ($ExecChildProbe) { Copy-Item -LiteralPath (Resolve-Path -LiteralPath $ExecChildProbe).Path -Destination "$PackageRoot\tests\D38CH.COM" }
Copy-Item -LiteralPath $Probe -Destination "$PackageRoot\tests\$ProbeName"
$results = @()
foreach ($route in @('direct','nested')) {
    if ((PackageProcesses).Count) { throw 'Package process remains' }
    if ($NetworkAssignCreate -and ((Test-Path Z:\) -or
        @(Get-SmbMapping -ErrorAction Stop | Where-Object LocalPath -eq 'Z:').Count)) {
        throw 'Assignment test requires unused Z:'
    }
    $report = "$LogRoot\$LogPrefix-$route.txt"
    if (Test-Path -LiteralPath $report) { throw 'Use a fresh log prefix' }
    $arguments = @("tests\$ProbeName")
    if ($route -eq 'nested') { $arguments = @('COMMAND.COM','/c','COMMAND.COM','/c') + $arguments }
    if ($KeyboardInput) {
        $keyboardText = switch ($KeyboardCase) {
            'empty' { "`r" }
            'overflow' { "ABCDEFGHIJK`r" }
            default { "ABCD`r" }
        }
        $arguments += @('--observe-console-input-marker','S38_KEYBOARD_READY',
            '--observe-console-input-text',('"' + $keyboardText + '"'))
    }
    if ($TsrExit) {
        $arguments += @('--observe-console-input-marker','S38_INT21_IO_STATE_FAILURE_CLEANUP_OK',
            '--observe-console-input-text',('"' + "exit`r" + '"'))
    }
    $launcher = 0
    $observation = Start-Process -FilePath $Observer -ArgumentList (
        @($paths[0],$PackageRoot,$report) + $arguments + @('--observation-timeout-ms','15000')
    ) -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
    try {
        if (!$observation.WaitForExit(30000)) { throw 'Observer remains live; inspect before retry' }
        if (!(Test-Path -LiteralPath $report)) {
            throw "Observer exited without report: exit=$($observation.ExitCode), route=$route"
        }
        $record = Get-Content -LiteralPath $report -Raw
        if ($KeyboardInput -and $record -notmatch '(?m)^scripted-console-input=delivered\r?$') { throw 'Keyboard input was not delivered' }
        $launcher = [int]([regex]::Match($record,'(?m)^pid=(\d+)').Groups[1].Value)
        if ($record -notmatch '(?m)^result=exited\r?$' -or
            $record -notmatch '(?m)^exit=0x00000000\r?$') { throw "Guest did not pass: $route" }
        $screen = Get-Content -LiteralPath "$report.console.txt" -Raw
        $text = ([regex]::Matches($screen, '(?m)^\[\d+\] (.*)\r?$') |
            ForEach-Object { $_.Groups[1].Value.TrimEnd("`r") }) -join ''
        if ($text -match 'S38_FAIL|Bad command or filename') { throw 'Guest reported failure' }
        if ($DescriptorContract) {
            if ($text -notmatch 'S38_DESCRIPTOR_REJECTION_PRESERVATION_OK') { throw 'Missing descriptor rejection assertions' }
            if ($ClientBits -eq 32 -and $text -notmatch 'S38_DESCRIPTOR_CLIENT32_OK') { throw 'Missing 32-bit descriptor assertions' }
        } elseif ($MemoryInfo) {
            if ($MemoryHighOffset -and $text -notmatch 'S38_MEMORY_INFO_HIGH_EDI_OK') { throw 'Missing high EDI memory query assertions' }
            if ($ClientBits -eq 32 -and $text -notmatch 'S38_MEMORY_INFO_CLIENT32_OK') { throw 'Missing 32-bit memory query assertions' }
            if ($text -notmatch 'S38_MEMORY_INFO_ALLOC_FREE_FIELDS_OK') { throw 'Missing memory information assertions' }
        } elseif ($Int21State) {
            if ($ExecFailure -and $text -notmatch 'S38_INT21_EXEC_FAILURE_PARENT_ENV_OK') { throw 'Missing EXEC failure/state assertions' }
            if ($ExecChildProbe -and $text -notmatch 'S38_EXEC_CHILD_TAIL_OK') { throw 'Missing real EXEC child assertion' }
            if ($NetworkDplPointer -and $text -notmatch 'S38_NETWORK_ERROR_POINTER_LINEAR_OK') { throw 'Missing extended-error pointer assertion' }
            if ($ExecLoadCall -and $text -notmatch 'S38_EXEC_LOADED_ENTRY_STACK_TAIL_CALL_OK') { throw 'Missing loaded-entry execution assertion' }
            if ($ExecInherit -and ($text -notmatch 'S38_EXEC_INHERITED_HANDLE_PARENT_SURVIVES_OK' -or
                (Test-Path -LiteralPath "$PackageRoot\tests\D38INH.TMP"))) { throw 'Inherited handle assertions or cleanup failed' }
            if ($ExecLoadOnly -and $text -notmatch 'S38_EXEC_LOAD_ONLY_RETURN_FREE_OK') { throw 'Missing load-only return/free assertions' }
            if ($NetworkName -and $text -notmatch 'S38_NETWORK_NAME_REAL_PM_EQUAL_OK') { throw 'Missing network name comparison' }
            if ($NetworkAssign -and $text -notmatch 'S38_NETWORK_ASSIGN_END_PRESERVED_OK') { throw 'Missing assignment enumeration assertions' }
            if ($NetworkAssignCreate -and $text -notmatch 'S38_NETWORK_ASSIGN_CREATE_QUERY_BREAK_OK') { throw 'Missing assignment lifecycle assertions' }
            if ($NetworkDpl -and $text -notmatch 'S38_NETWORK_DPL_ERROR_ROUNDTRIP_OK') { throw 'Missing DPL error roundtrip' }
            if ($NetworkSpool -and $text -notmatch 'S38_NETWORK_SPOOL_FLAG_REJECT_RESTORE_OK') { throw 'Missing original spool flag lifecycle' }
            if ($NetworkPrint -and $text -notmatch 'S38_NETWORK_PRINT_REAL_PM_EQUAL_OK') { throw 'Missing print boundary comparison' }
            if ($CreatePsp -and $text -notmatch 'S38_INT21_CREATE_PSP_FIELDS_FREE_OK') { throw 'Missing CreatePSP assertion' }
            if ($CreatePsp -and $Code32 -and $text -notmatch 'S38_INT21_CREATE_PSP_HIGH_UNCHANGED_OK') { throw 'Missing high-memory CreatePSP refusal assertion' }
            if ($IoctlMetadataMock -and $text -notmatch 'S38_INT21_METADATA_MOCK_PAIRS_OK') { throw 'Missing controlled metadata-pair assertions' }
            if ($IoctlDeviceMock -and $text -notmatch 'S38_IOCTL_DEVICE_LENGTH_RETURN_GUARD_OK') { throw 'Missing controlled device-packet assertions' }
            if ($IoctlControlMock -and $text -notmatch 'S38_IOCTL_CONTROL_FULL_SHORT_FAILURE_OK') { throw 'Missing controlled data-transfer assertions' }
            if ($IoctlMediaQuery -and $text -notmatch 'S38_INT21_IOCTL_MEDIA_QUERY_OK') { throw 'Missing real media query assertion' }
            if ($IoctlFormatMock -and $text -notmatch 'S38_INT21_FORMAT_MOCK_PACKET_OK') { throw 'Missing controlled format packet assertion' }
            if ($IoctlTrackMock -and $text -notmatch 'S38_INT21_TRACK_MOCK_TRANSFER_OK') { throw 'Missing controlled track transfer assertion' }
            if ($IoctlPrinter -and $text -notmatch 'S38_INT21_PRINTER_IOCTL_READ_OK') { throw 'Missing original printer IOCTL assertion' }
            if ($IoctlFailure -and $text -notmatch 'S38_INT21_IOCTL_INVALID_HANDLE_BUFFER_OK') { throw 'Missing IOCTL failure assertion' }
            if ($KeyboardInput -and $text -notmatch 'S38_INT21_BUFFERED_KEYBOARD_CONTENT_GUARD_OK') { throw 'Missing buffered keyboard assertion' }
            if ($KeyboardFlush -and $text -notmatch 'S38_INT21_FLUSH_THEN_BUFFERED_INPUT_OK') { throw 'Missing flush/input assertion' }
            if ($KeyboardDiscard -and $text -notmatch 'S38_INT21_QUEUED_KEY_DISCARDED_OK') { throw 'Missing queued input discard assertion' }
            if ($KeyboardStatus -and $text -notmatch 'S38_INT21_NONBLOCKING_EMPTY_ZF_OK') { throw 'Missing nonblocking input flags assertion' }
            if ($KeyboardCase -eq 'empty' -and $text -notmatch 'S38_INT21_KEYBOARD_EMPTY_OK') { throw 'Missing empty input assertion' }
            if ($KeyboardCase -eq 'overflow' -and $text -notmatch 'S38_INT21_KEYBOARD_CAPACITY_OK') { throw 'Missing input capacity assertion' }
            if ($DriveQuery -and $text -notmatch 'S38_INT21_DRIVE_POINTERS_REAL_PM_EQUAL_OK') { throw 'Missing drive pointer translation assertion' }
            if ($DriveQuery -and $text -notmatch 'S38_INT21_INVALID_DRIVE_REJECT_OK') { throw 'Missing invalid drive assertion' }
            if ($DosMemory -and $text -notmatch 'S38_INT21_MEMORY_GROW_SHRINK_FREE_REJECT_OK') { throw 'Missing DOSX memory lifecycle assertion' }
            if ($PointerQuery -and $text -notmatch 'S38_INT21_INDOS_SELECTOR_LINEAR_OK') { throw 'Missing InDOS pointer translation assertion' }
            if ($PointerQuery -and $text -notmatch 'S38_INT21_LIST_SELECTOR_LINEAR_OK') { throw 'Missing DOS list pointer translation assertion' }
            if ($PointerQuery -and $text -notmatch 'S38_INT21_DBCS_SELECTOR_LINEAR_OK') { throw 'Missing DBCS pointer translation assertion' }
            if ($PointerQuery -and $text -notmatch 'S38_INT21_NON_DBCS_SUBFUNCTION_STATE_OK') { throw 'Missing non-DBCS subfunction preservation assertion' }
            if ($CountryQuery -and $text -notmatch 'S38_INT21_COUNTRY_REAL_PM_EQUAL_OK') { throw 'Missing real/protected country comparison' }
            if ($CountryQuery -and $text -notmatch 'S38_INT21_EXTENDED_COUNTRY_EQUAL_OK') { throw 'Missing extended-country comparison' }
            if ($TempFile -and $text -notmatch 'S38_INT21_TEMP_CREATE_REOPEN_DELETE_OK') { throw 'Missing temporary-file lifecycle assertion' }
            if ($ExtendedOpen -and $text -notmatch 'S38_INT21_EXTENDED_OPEN_OK') { throw 'Missing extended-open assertion' }
            if ($ExtendedFcb -and $text -notmatch 'S38_INT21_EXTENDED_FCB_OK') { throw 'Missing extended FCB assertion' }
            if ($FcbFamily -and $text -notmatch 'S38_INT21_FCB_FIND_RENAME_DELETE_OK') { throw 'Missing FCB operation assertions' }
            if ($FcbFamily -and $text -notmatch 'S38_INT21_FCB_DTA_DIRECTORY_RESTORED_OK') { throw 'Missing FCB state restoration assertions' }
            if ($FcbFamily -and (Test-Path -LiteralPath "$PackageRoot\tests\D38FCB.TMP")) { throw 'Guest retained FCB rename output' }
            if ($ParseFamily -and $text -notmatch 'S38_INT21_PARSE_FCB_OK') { throw 'Missing filename/FCB translation assertion' }
            if ($ParseFamily -and $text -notmatch 'S38_INT21_PARSE_WILDCARD_OK') { throw 'Missing wildcard parsing assertion' }
            if ($BpbRefusal -and $text -notmatch 'S38_INT21_BPB_REFUSAL_OK') { throw 'Missing BPB refusal assertion' }
            if ($PspFamily -and $text -notmatch 'S38_INT21_PSP_IDENTITY_OK') { throw 'Missing PSP identity assertions' }
            if ($PspFamily -and $Code32 -and $text -notmatch 'S38_INT21_PSP_HIGH_REJECT_PRESERVE_OK') { throw 'Missing high PSP refusal assertions' }
            if ($DirectoryNavigation -and $text -notmatch 'S38_INT21_DIRECTORY_RESTORE_OK') { throw 'Missing directory navigation/restoration assertions' }
            if ($FindFamily -and $text -notmatch 'S38_INT21_DTA_FIND_DIRECTORY_OK') { throw 'Missing DTA/find/directory assertions' }
            if ($FindFamily -and (Test-Path -LiteralPath "$PackageRoot\tests\D38DIR")) { throw 'Guest retained its test directory' }
            if ($FileFamily -and $text -notmatch 'S38_INT21_FILE_FAMILY_OK') { throw 'Missing file family assertions' }
            if ($FileFamily -and (Test-Path -LiteralPath "$PackageRoot\tests\D38RN.TMP")) { throw 'Guest retained its renamed file' }
            if ($Code32 -and $text -notmatch 'S38_INT21_CODE32_OK') { throw 'Missing true 32-bit code assertions' }
            if ($LargeCount -and $text -notmatch 'S38_INT21_COUNT_ABOVE_64K_OK') { throw 'Missing large transfer assertions' }
            if ($text -notmatch 'S38_INT21_IO_STATE_FAILURE_CLEANUP_OK') { throw 'Missing INT21 state assertions' }
            if ($ClientBits -eq 32 -and $text -notmatch 'S38_INT21_CLIENT32_STATE_OK') { throw 'Missing 32-bit INT21 assertions' }
            if (Test-Path -LiteralPath "$PackageRoot\tests\D38IO.TMP") { throw 'Guest did not delete its test file' }
        } elseif ($VcdService) {
            if ($text -notmatch 'S38_VCD_VERSION_PORTS_REFUSAL_OK') { throw 'Missing VCD assertions' }
        } elseif ($DebugRollback) {
            if ($text -notmatch 'S38_DEBUG_WRITE_HIT_OK' -or
                $text -notmatch 'S38_DEBUG_INVALID_SPAN_ROLLBACK_OK') { throw 'Missing debug rollback assertions' }
        } elseif ($DebugGroups) {
            if ($text -notmatch 'S38_DEBUG_WRITE_HIT_OK' -or
                $text -notmatch 'S38_DEBUG_SCALAR_RMW_STRING_STACK_CODE_OK') { throw 'Missing debug delivery assertions' }
        } elseif (
            $text -notmatch "S38_INT${ClientBits}_RETURN_OK" -or
            $text -notmatch "S38_FAULT${ClientBits}_RETURN_NEGATIVE_OK" -or
            (!$Reflection -and $text -notmatch 'S38_HARDWARE_IRQ_RETURN_OK')) { throw 'Missing guest assertions' }
        if ($Reflection -and $text -notmatch 'S38_UNHANDLED_REFLECTION_OK') {
            throw 'Missing unhandled exception reflection assertion'
        }
        $results += @{Route=$route; ClientBits=$ClientBits; Reflection=[bool]$Reflection; VcdService=[bool]$VcdService; DebugGroups=[bool]$DebugGroups; DebugRollback=[bool]$DebugRollback; Report=$report; ProbeSha256=(Get-FileHash $Probe).Hash; WorkerSha256=(Get-FileHash $paths[1]).Hash}
        $results[-1].Int21State = [bool]$Int21State
        $results[-1].FileFamily = [bool]$FileFamily
        $results[-1].FindFamily = [bool]$FindFamily
        $results[-1].DirectoryNavigation = [bool]$DirectoryNavigation
        $results[-1].PspFamily = [bool]$PspFamily
        $results[-1].CreatePsp = [bool]$CreatePsp
        $results[-1].ExecFailure = [bool]$ExecFailure
        $results[-1].ExecInherit = [bool]$ExecInherit
        $results[-1].ExecLoadCall = [bool]$ExecLoadCall
        $results[-1].NetworkDplPointer = [bool]$NetworkDplPointer
        $results[-1].ExecChild = [bool]$ExecChildProbe
        $results[-1].ExecLoadOnly = [bool]$ExecLoadOnly
        $results[-1].NetworkName = [bool]$NetworkName
        $results[-1].NetworkAssign = [bool]$NetworkAssign
        $results[-1].NetworkAssignCreate = [bool]$NetworkAssignCreate
        $results[-1].NetworkDpl = [bool]$NetworkDpl
        $results[-1].NetworkSpool = [bool]$NetworkSpool
        $results[-1].NetworkPrint = [bool]$NetworkPrint
        if ($ExecChildProbe) { $results[-1].ExecChildSha256 = (Get-FileHash -LiteralPath "$PackageRoot\tests\D38CH.COM").Hash }
        $results[-1].BpbRefusal = [bool]$BpbRefusal
        $results[-1].ParseFamily = [bool]$ParseFamily
        $results[-1].FcbFamily = [bool]$FcbFamily
        $results[-1].ExtendedFcb = [bool]$ExtendedFcb
        $results[-1].ExtendedOpen = [bool]$ExtendedOpen
        $results[-1].TempFile = [bool]$TempFile
        $results[-1].CountryQuery = [bool]$CountryQuery
        $results[-1].PointerQuery = [bool]$PointerQuery
        $results[-1].DosMemory = [bool]$DosMemory
        $results[-1].DriveQuery = [bool]$DriveQuery
        $results[-1].KeyboardInput = [bool]$KeyboardInput
        $results[-1].KeyboardFlush = [bool]$KeyboardFlush
        $results[-1].KeyboardDiscard = [bool]$KeyboardDiscard
        $results[-1].KeyboardStatus = [bool]$KeyboardStatus
        $results[-1].IoctlFailure = [bool]$IoctlFailure
        $results[-1].IoctlPrinter = [bool]$IoctlPrinter
        $results[-1].IoctlTrackMock = [bool]$IoctlTrackMock
        $results[-1].IoctlFormatMock = [bool]$IoctlFormatMock
        $results[-1].IoctlMediaQuery = [bool]$IoctlMediaQuery
        $results[-1].IoctlMetadataMock = [bool]$IoctlMetadataMock
        $results[-1].IoctlDeviceMock = [bool]$IoctlDeviceMock
        $results[-1].IoctlControlMock = [bool]$IoctlControlMock
        $results[-1].KeyboardCase = $KeyboardCase
        $results[-1].MemoryInfo = [bool]$MemoryInfo
        $results[-1].Code32 = [bool]$Code32
        $results[-1].LargeCount = [bool]$LargeCount
        $results[-1].MemoryHighOffset = [bool]$MemoryHighOffset
        $results[-1].DescriptorContract = [bool]$DescriptorContract
        if ($NetworkAssignCreate -and
            @(Get-SmbMapping -ErrorAction Stop | Where-Object LocalPath -eq 'Z:').Count) {
            throw 'Guest did not remove the test assignment'
        }
        Write-Host "PASS S38 DPMI capability $route"
    } finally {
        if ($NetworkAssignCreate) {
            $testMapping = @(Get-SmbMapping -ErrorAction Stop | Where-Object {
                $_.LocalPath -eq 'Z:' -and $_.RemotePath -eq '\\LOCALHOST\O$'
            })
            if ($testMapping.Count) {
                Remove-SmbMapping -LocalPath 'Z:' -Force -ErrorAction Stop
            }
        }
        if ($launcher) {
            foreach ($child in (PackageProcesses)) {
                if ($child.ParentProcessId -eq $launcher) {
                    $process = Get-Process -Id $child.ProcessId -ErrorAction SilentlyContinue
                    if ($process) {
                        # Broker-loss teardown may finish between lookup and kill.
                        Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue
                        if (!$process.WaitForExit(5000)) { throw 'Test-owned process survived cleanup' }
                    }
                }
            }
        }
    }
}
$results | ConvertTo-Json -Depth 4 | Set-Content "$LogRoot\$LogPrefix-summary.json" -Encoding utf8
