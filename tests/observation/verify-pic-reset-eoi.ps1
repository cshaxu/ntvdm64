[CmdletBinding()]
param(
    [string]$BuildCache = 'build/.ninja/msvc-x86-release/worker',
    [Parameter(Mandatory = $true)][string]$OutputRoot,
    [string]$PackageRoot,
    [string]$Observer,
    [string]$Nasm='nasm.exe'
)
$ErrorActionPreference = 'Stop'
if([bool]$PackageRoot -ne [bool]$Observer){throw 'Real guest check requires PackageRoot and Observer together'}
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$cache = (Resolve-Path (Join-Path $root $BuildCache)).Path
$out = [IO.Path]::GetFullPath((Join-Path $root $OutputRoot))
$allowed = [IO.Path]::GetFullPath((Join-Path $root 'build')) + '\'
if (!$out.StartsWith($allowed, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Evidence must be below repository build/.'
}
if (Test-Path -LiteralPath $out) { throw 'Select a fresh, unsealed evidence root.' }
$exe = Join-Path $cache 'pic-reset-eoi-test.exe'
$map = Get-Content -LiteralPath ($exe + '.map') -Raw
foreach ($entry in @(@('ica_outb','original-softpc-system:ica.obj'),
    @('ica_intack','original-softpc-system:ica.obj'),
    @('RtcIntEoiHook','original-softpc-system:cmosnt.obj'),
    @('MouseEoiHook','original-softpc-host-fixture-roots:nt_mouse.obj'),
    @('FlushMouseEvents','original-softpc-host-fixture-roots:nt_event.obj'))) {
    if ($map -notmatch ('(?m)^.*_' + $entry[0] + '\s+.*' + [regex]::Escape($entry[1]) + '\s*$')) {
        throw "Actual production provider missing: $($entry[0])"
    }
}
$graph = Get-Content -LiteralPath (Join-Path $cache 'build.ninja') -Raw
if ($graph -notmatch '/DNTVDM\b' -or $graph -notmatch '/DCPU_40_STYLE\b' -or
    $graph -match '/DCPU_30_STYLE\b') { throw 'Wrong selected PIC/CCPU profile.' }
New-Item -ItemType Directory -Path $out | Out-Null
$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = $exe; $start.UseShellExecute = $false; $start.CreateNoWindow = $true
$start.RedirectStandardError = $true; $start.RedirectStandardOutput = $true
$process = [Diagnostics.Process]::new(); $process.StartInfo = $start
$clock = [Diagnostics.Stopwatch]::StartNew()
try {
    if (!$process.Start()) { throw 'Fixture did not start.' }
    $stdout = $process.StandardOutput.ReadToEndAsync(); $stderr = $process.StandardError.ReadToEndAsync()
    if (!$process.WaitForExit(10000)) {
        $process.Kill(); $process.WaitForExit(); throw 'PIC-reset fixture timeout; not a pass.'
    }
    $text = $stdout.Result + $stderr.Result
    $text | Set-Content -LiteralPath (Join-Path $out 'pic-reset-eoi.txt') -Encoding UTF8
    if ($process.ExitCode -ne 0 -or $text -notmatch 'PIC_RESET_EOI assertions=24 failures=0' -or
        $text -notmatch 'PASS PIC reset retires pending and in-service master ownership' -or
        $text -notmatch 'PASS PIC reset retains controller-local cascade ownership' -or
        $text -notmatch 'PASS PIC reset distinguishes callback-requeued ownership' -or
        $text -notmatch 'PASS actual RTC reset cancellation and periodic requeue' -or
        $text -notmatch 'PASS actual mouse reset cancellation flushes without requeue') {
        throw 'PIC-reset EOI assertions failed.'
    }
    [pscustomobject]@{
        profile = 'x86 /MT NTVDM CCPU40'; elapsed_ms = $clock.ElapsedMilliseconds
        executable_sha256 = (Get-FileHash -LiteralPath $exe).Hash
        ica_sha256 = (Get-FileHash (Join-Path $root 'src/mvdm/softpc.new/base/system/ica.c')).Hash
        scope = 'actual production PIC reset; controlled EOI callback boundary; no guest mutation'
    } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $out 'result.json') -Encoding UTF8
    Write-Output $text
} finally { $process.Dispose() }

if($PackageRoot) {
    . "$PSScriptRoot/isolated_package_cleanup.ps1"
    $observerPath=(Resolve-Path -LiteralPath $Observer).Path
    $guestSource=Join-Path $PSScriptRoot 'pic_reset_eoi_guest.asm'
    & $Nasm -f bin $guestSource -o "$out/PICIRQ.COM"
    if($LASTEXITCODE){throw 'Independent PIC guest probe assembly failed'}
    Get-FileHash $guestSource,"$out/PICIRQ.COM",$observerPath |
        Select-Object Path,Hash | Export-Csv -NoTypeInformation "$out/guest-inputs.csv"
    $stage=New-PhysicalRuntimeStage (Resolve-Path -LiteralPath $PackageRoot).Path -TopLevelNames @('system32','system.ini')
    $scope=$null;$private=$env:MVDM_OBSERVER_PRIVATE_DESKTOP;$path=$env:PATH
    try {
        $scope=New-IsolatedPackageScope $stage
        New-Item -ItemType Directory -Path "$stage/tests" | Out-Null
        Copy-Item "$out/PICIRQ.COM" "$stage/tests/PICIRQ.COM"
        $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
        $env:PATH="$stage/system32;"+(Join-Path $env:SystemRoot 'System32')
        Get-ChildItem "$stage/system32" -File | Where-Object Extension -in @('.exe','.dll','.sys') |
            Get-FileHash | Select-Object Path,Hash | Export-Csv -NoTypeInformation "$out/runtime-inputs.csv"
        foreach($mode in @('control','reinit')) {
            $report=Join-Path $out "$mode.txt"
            $arguments=@("$stage/system32/run16.exe","$stage/tests",$report,'--wait',"$stage/tests/PICIRQ.COM")
            if($mode -eq 'reinit'){$arguments+='R'}
            $arguments+=@('--observation-timeout-ms','10000')
            & $observerPath @arguments
            if($LASTEXITCODE){throw "PIC guest observer failed: $mode"}
            $record=Get-Content $report -Raw
            $screen=Get-Content "$report.console.txt" -Raw
            if($record -notmatch '(?m)^result=exited' -or $record -notmatch '(?m)^exit=0x00000000' -or
                $screen -notmatch 'PIC-REAL-IRQ0-PASS'){throw "Actual DOS IRQ0 stopped: $mode"}
            "PASS actual DOS IRQ0 $mode"
        }
        'PASS real DOS IRQ0 control/reinitialization and original task exit' | Set-Content "$out/guest-result.txt"
    } finally {
        $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$private;$env:PATH=$path
        if($scope){Stop-IsolatedPackageScope $scope}
        Remove-PhysicalRuntimeStage $stage
    }
}
