[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Probe,
    [Parameter(Mandatory)][string]$LogPrefix,
    [ValidatePattern('^[0-9A-F]{8}$')][string]$FaultIp,
    [string]$PmAllocationLog,
    [switch]$AllocationIgnore,
    [string]$PackageRoot = 'V:\D38TRACE',
    [string]$LogRoot = 'O:\winnt\logs',
    [switch]$Nested,
    [switch]$IgnoreFirst
)
$ErrorActionPreference = 'Stop'
if ($AllocationIgnore -and !$PmAllocationLog) { throw 'AllocationIgnore requires allocation injection' }
if ($PmAllocationLog) {
    if ($FaultIp -or $IgnoreFirst) { throw 'Allocation failure is separate from GP/Ignore verification' }
    $expectedDialog = [regex]::Escape('The memory resources needed by NTVDM could not be allocated.')
    if (Test-Path -LiteralPath $PmAllocationLog) { throw 'Use a fresh allocation injection log' }
} else {
    if (!$FaultIp) { throw 'FaultIp is required for GP verification' }
    $expectedDialog = "X#=0D, CS=[0-9A-F]{4} IP=$FaultIp"
}
$Observer = (Resolve-Path -LiteralPath $Observer).Path
$Probe = (Resolve-Path -LiteralPath $Probe).Path
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid log prefix' }
$report = Join-Path $LogRoot "$LogPrefix.txt"
if (Test-Path -LiteralPath $report) { throw 'Use a fresh log prefix' }
$packagePaths = @('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object { Join-Path $PackageRoot $_ }
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object { $_.ExecutablePath -in $packagePaths })
}
if ((PackageProcesses).Count) { throw 'Isolated package already in use' }
Add-Type -TypeDefinition @'
using System;
using System.Text;
using System.Runtime.InteropServices;
public static class S38FatalDialog {
    public delegate bool Callback(IntPtr w, IntPtr p);
    [DllImport("user32.dll")] static extern bool EnumWindows(Callback cb, IntPtr p);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr w, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern int GetClassName(IntPtr w, StringBuilder b, int n);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern uint GetDlgItemText(IntPtr w, int id, StringBuilder b, int n);
    [DllImport("user32.dll")] static extern IntPtr GetDlgItem(IntPtr w, int id);
    [DllImport("user32.dll")] static extern IntPtr SendMessageTimeout(IntPtr w, uint m, IntPtr a, IntPtr b, uint flags, uint ms, out IntPtr result);
    [DllImport("kernel32.dll")] static extern IntPtr OpenThread(uint access, bool inherit, uint tid);
    [DllImport("kernel32.dll")] static extern uint WaitForSingleObject(IntPtr h, uint ms);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);
    public static uint ThreadId(IntPtr w) { uint pid; return GetWindowThreadProcessId(w,out pid); }
    public static void IgnoreAndWait(IntPtr w) {
        IntPtr h=OpenThread(0x100000,false,ThreadId(w));
        if(h==IntPtr.Zero) throw new Exception("Cannot observe original dialog thread");
        try {
            if(!Click(w,102)) throw new Exception("Ignore action failed");
            if(WaitForSingleObject(h,5000)!=0) throw new Exception("Original dialog thread did not exit");
        } finally { CloseHandle(h); }
    }
    public static IntPtr Find(uint pid) {
        IntPtr found=IntPtr.Zero;
        EnumWindows(delegate(IntPtr w, IntPtr p) {
            uint owner; GetWindowThreadProcessId(w,out owner);
            if(owner==pid) { var b=new StringBuilder(128); GetClassName(w,b,128);
                if(b.ToString()=="#32770") { found=w; return false; } }
            return true;
        },IntPtr.Zero);
        return found;
    }
    public static string Text(IntPtr w) { var b=new StringBuilder(2048); GetDlgItemText(w,104,b,2048); return b.ToString(); }
    public static bool Click(IntPtr w, int id) {
        IntPtr button=GetDlgItem(w,id), result;
        return button!=IntPtr.Zero && SendMessageTimeout(w,0x111,new IntPtr(id),button,2,2000,out result)!=IntPtr.Zero;
    }
}
'@
Copy-Item -LiteralPath $Probe -Destination "$PackageRoot\tests\D38F.COM"
$arguments = @('tests\D38F.COM')
if ($Nested) { $arguments = @('COMMAND.COM','/c','COMMAND.COM','/c') + $arguments }
$observation = $null
$workerId = 0
$dialogText = ''
try {
    $observation = Start-Process -FilePath $Observer -ArgumentList (
        @($packagePaths[0],$PackageRoot,$report) + $arguments + @('--observation-timeout-ms','20000')
    ) -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
    $deadline = [DateTime]::UtcNow.AddSeconds(15)
    $dialog = [IntPtr]::Zero
    while ([DateTime]::UtcNow -lt $deadline) {
        $workers = @(PackageProcesses | Where-Object { $_.Name -eq 'ntvdm.exe' })
        if ($workers.Count -gt 1) { throw 'Ambiguous test worker' }
        if ($workers.Count -eq 1) {
            $workerId = [int]$workers[0].ProcessId
            $dialog = [S38FatalDialog]::Find($workerId)
            if ($dialog -ne [IntPtr]::Zero) {
                $dialogText = [S38FatalDialog]::Text($dialog)
                if ($dialogText -match $expectedDialog) { break }
            }
        }
        Start-Sleep -Milliseconds 150
    }
    if ($dialogText -notmatch $expectedDialog) { throw "Expected exact original dialog absent: $dialogText" }
    $worker = Get-Process -Id $workerId
    $firstThread = [S38FatalDialog]::ThreadId($dialog)
    $secondThread = 0
    if ($IgnoreFirst) {
        [S38FatalDialog]::IgnoreAndWait($dialog)
        $deadline = [DateTime]::UtcNow.AddSeconds(5)
        do {
            $dialog = [S38FatalDialog]::Find($workerId)
            $secondThread = [S38FatalDialog]::ThreadId($dialog)
            $dialogText = [S38FatalDialog]::Text($dialog)
            if ($secondThread -ne 0 -and $secondThread -ne $firstThread -and
                $dialogText -match "X#=0D, CS=[0-9A-F]{4} IP=$FaultIp") { break }
            Start-Sleep -Milliseconds 50
        } while ([DateTime]::UtcNow -lt $deadline)
        if ($secondThread -eq 0 -or $secondThread -eq $firstThread -or
            $dialogText -notmatch "X#=0D, CS=[0-9A-F]{4} IP=$FaultIp") { throw 'Ignore did not produce a new exact-fault dialog' }
    }
    $button = if ($AllocationIgnore) { 102 } else { 100 }
    if (![S38FatalDialog]::Click($dialog,$button)) { throw 'Original dialog action failed' }
    if (!$worker.WaitForExit(10000)) { throw 'Worker survived Terminate' }
    if (!$observation.WaitForExit(10000)) { throw 'Observer/launcher remains live' }
    $record = Get-Content -LiteralPath $report -Raw
    if ($record -notmatch '(?m)^result=exited\r?$') { throw 'Launcher did not exit' }
    if ($PmAllocationLog) {
        $injection = Get-Content -LiteralPath $PmAllocationLog -Raw
        if ($injection -notmatch 'S38_PM_ALLOCATION_REFUSED size=36 address=0' -or
            $injection -match 'S38_FAIL_PM_PUBLICATION_RETURNED') { throw 'Allocation failure did not stop publication' }
    }
    @{ Dialog=$dialogText; WorkerId=$workerId; WorkerExited=$true; Nested=[bool]$Nested;
       IgnoreFirst=[bool]$IgnoreFirst; AllocationIgnore=[bool]$AllocationIgnore; PmAllocationLog=$PmAllocationLog; FirstDialogThread=$firstThread; SecondDialogThread=$secondThread;
       ProbeSha256=(Get-FileHash $Probe).Hash; WorkerSha256=(Get-FileHash $packagePaths[1]).Hash;
       Report=$report } | ConvertTo-Json | Set-Content -LiteralPath "$LogRoot\$LogPrefix.result.json"
    Write-Host "PASS S38 original failure dialog and Terminate: $LogPrefix"
} finally {
    # Only processes from the isolated, initially unused test package.
    foreach ($process in (PackageProcesses)) { Stop-Process -Id $process.ProcessId -Force -ErrorAction SilentlyContinue }
    if ($observation -and !$observation.HasExited) { $observation.Kill(); $observation.WaitForExit() }
    @{ Dialog=$dialogText; WorkerId=$workerId } | ConvertTo-Json | Set-Content -LiteralPath "$LogRoot\$LogPrefix.dialog.json"
}
