[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)][string]$EvidenceRoot,
    [string]$BuildRoot = '',
    [string]$BinaryName = 'ntdos64-s7-runtime-trace.exe',
    [ValidateRange(1,30)][int]$WatchdogSeconds = 15,
    [ValidateRange(1,10)][int]$BenchmarkMillions = 1,
    [switch]$DeferredStartupPlan
)
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent $PSScriptRoot }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$evidence = [IO.Path]::GetFullPath($EvidenceRoot)
if (Test-Path -LiteralPath $evidence) { throw "Refusing to overwrite evidence directory: $evidence" }
function Hash([string]$path) { if(-not (Test-Path -LiteralPath $path -PathType Leaf)){throw "Missing: $path"}; (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash }
$build = if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    Join-Path $repository 'artifacts\build\bochs-2.6-native-s7-runtime-trace-msvc-r5'
} else {
    [IO.Path]::GetFullPath($BuildRoot)
}
$binary = Join-Path $build $BinaryName
$inputs = @(
 @{role='ntio';name='NTIO.SYS';bytes=33792;sha='CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937';path=(Join-Path $repository 'artifacts\toolchain-runs\ntio-tools16-opennt-v1\base\mvdm\dos\v86\doskrnl\bios\NTIO.SYS')},
 @{role='ntdos';name='NTDOS.SYS';bytes=27858;sha='957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84';path=(Join-Path $repository 'artifacts\toolchain-runs\ntdos-tools16-historical-v1\base\mvdm\dos\v86\doskrnl\dos\NTDOS.SYS')},
 @{role='command';name='COMMAND.COM';bytes=50384;sha='908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43';path=(Join-Path $repository 'artifacts\toolchain-runs\command-tools16-opennt-v1\base\mvdm\dos\v86\cmd\command\COMMAND.COM')},
 @{role='target';name='TARGET.COM';bytes=3;sha='06A37DFF559DF7325DE8B003F4DF53C188F733E0CA312AAD961C34DAE48D7B83';path=(Join-Path $repository 'artifacts\toolchain-runs\quit-tools16-opennt-v1\base\mvdm\v86\util\QUIT.COM')})
foreach($item in $inputs){if((Get-Item -LiteralPath $item.path).Length -ne $item.bytes -or (Hash $item.path) -ne $item.sha){throw "Identity mismatch: $($item.name)"}}
New-Item -ItemType Directory -Path $evidence,(Join-Path $evidence 'byob'),(Join-Path $evidence 'ROM') -Force | Out-Null
foreach($item in $inputs){Copy-Item $item.path (Join-Path $evidence ('byob\'+$item.name)); if((Hash (Join-Path $evidence ('byob\'+$item.name))) -ne $item.sha){throw "Copy mismatch: $($item.name)"}}
foreach($rom in @('BIOS-bochs-latest','VGABIOS-lgpl-latest')){Copy-Item (Join-Path $build ('ROM\'+$rom)) (Join-Path $evidence ('ROM\'+$rom)); [void](Hash (Join-Path $evidence ('ROM\'+$rom)))}
$driveTypes = @(0..25 | ForEach-Object { [int]([IO.DriveInfo]::new(([Convert]::ToChar(65 + [int]$_).ToString() + ':\\')).DriveType) })
$profile=[ordered]@{schema='ntdos64-byob-profile-v4';profile='nt4-en-us-command-smoke-v4';architecture='x86';locale='en-US';compatibility_group='t97-pathname-runtime-trace';components=@($inputs|%{[ordered]@{role=$_.role;file_name=$_.name;required=$true;bytes=$_.bytes;sha256=$_.sha.ToLowerInvariant();version=$null}});features=@();owner_note=$null;guest_command_placement=@{path='\COMMAND.COM';drive_index=2};guest_target_placement=@{path='\TARGET.COM';drive_index=2};guest_boot_files=@{config=@{path='\CONFIG.SYS';materialization='minimal-comment-v1'};autoexec=@{path='\AUTOEXEC.BAT';materialization='empty-v1'}};guest_search_metadata=@{command=@{attributes=32;dos_time=1;dos_date=33};target=@{attributes=32;dos_time=2;dos_date=34};config=@{attributes=32;dos_time=3;dos_date=35};autoexec=@{attributes=32;dos_time=4;dos_date=36}};host_drive_inventory=@{types=$driveTypes};machine_startup_plan=$true;machine_startup_entry='ntio-v0'}
$optionRomLine = ''
if ($DeferredStartupPlan) {
    $optionRom = New-Object byte[] 512
    $optionRom[0]=0x55; $optionRom[1]=0xaa; $optionRom[2]=0x01
    $optionRom[3]=0x0f; $optionRom[4]=0x0b; $optionRom[5]=0xf4
    $optionRom[511]=0xf2
    [IO.File]::WriteAllBytes((Join-Path $evidence 'ROM\ntdos64-post-handoff.rom'), $optionRom)
    if ((Hash (Join-Path $evidence 'ROM\ntdos64-post-handoff.rom')) -ne 'E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB') { throw 'Option-ROM identity mismatch' }
    $profile.machine_startup_handoff=[ordered]@{exception_vector=6;instruction_bytes='0f0b';cs=51840;ip=3}
    $optionRomLine="optromimage1: file=ROM/ntdos64-post-handoff.rom, address=0xca800`r`n"
}
$profilePath=Join-Path $evidence 'byob\profile.json'; [IO.File]::WriteAllText($profilePath,($profile|ConvertTo-Json -Depth 8 -Compress),[Text.UTF8Encoding]::new($false))
$bochsrc="config_interface: textconfig`r`ndisplay_library: nogui`r`ncpu: count=1, ips=1000000, reset_on_triple_fault=0`r`nmegs: 4`r`nromimage: file=ROM/BIOS-bochs-latest`r`nvgaromimage: file=ROM/VGABIOS-lgpl-latest`r`n$optionRomLine`npci: enabled=0`r`nvga: extension=none, update_freq=5`r`nfloppy_bootsig_check: disabled=1`r`nboot: disk`r`nplugin_ctrl: unmapped=0, biosdev=0, speaker=0, extfpuirq=0, parallel=0, serial=0, gameport=0`r`n"
[IO.File]::WriteAllText((Join-Path $evidence 'trace.bochsrc'),$bochsrc,[Text.UTF8Encoding]::new($false))
$p=[Diagnostics.Process]::new();$p.StartInfo.FileName='cmd.exe';$childCommand='set NTDOS64_ADAPTER_PROFILE='+$profilePath+'&& set NTDOS64_ADAPTER_ROOT='+(Join-Path $evidence 'byob')+'&& set NTDOS64_ADAPTER_LAUNCH_KIND=com&& set NTDOS64_ADAPTER_LAUNCH_TAIL=/q&& set NTDOS64_ADAPTER_BRIDGE=1&& set NTDOS64_ADAPTER_TRACE=1&& '+$binary+' -q -f trace.bochsrc -benchmark '+$BenchmarkMillions+' -log bochs.log';$p.StartInfo.Arguments='/d /c "'+$childCommand+'"';$p.StartInfo.WorkingDirectory=$evidence;$p.StartInfo.UseShellExecute=$false;$p.StartInfo.CreateNoWindow=$true;if(-not $p.Start()){throw 'Process did not start'};$timed=-not $p.WaitForExit($WatchdogSeconds*1000);if($timed){& taskkill.exe /T /F /PID $p.Id | Out-Null;$p.WaitForExit()};$log=if(Test-Path (Join-Path $evidence 'bochs.log')){Get-Content (Join-Path $evidence 'bochs.log') -Raw}else{''};$record=[ordered]@{binarySha256=Hash $binary;profileSha256=Hash $profilePath;watchdogSeconds=$WatchdogSeconds;benchmarkMillions=$BenchmarkMillions;watchdogTerminated=$timed;exitCode=$p.ExitCode;bopObservations=[regex]::Matches($log,'ntdos64 adapter bop observed').Count;commits=[regex]::Matches($log,'ntdos64 adapter (?:bulk |multi-write )?transaction committed').Count};[IO.File]::WriteAllText((Join-Path $evidence 'observation.json'),($record|ConvertTo-Json),[Text.UTF8Encoding]::new($false));Write-Host "S7 runtime trace observation recorded: $evidence"
