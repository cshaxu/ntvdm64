[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function Get-Sha256([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$output = [IO.Path]::GetFullPath($OutputDirectory)
if (Test-Path -LiteralPath $output) { throw "Refusing to overwrite S48 guest lifecycle-ledger fixture root: $output" }
$items = @(
    [ordered]@{ role='ntio'; stage='build\M0-T225-S8\ntio-primary-source-build-r8'; manifest='ntio-build-manifest.json'; relative='base\mvdm\dos\v86\doskrnl\bios\NTIO.SYS'; file='NTIO.SYS'; bytes=33792; sha256='cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937' },
    [ordered]@{ role='ntdos'; stage='build\M0-T225-S8\ntdos-primary-source-build-r1'; manifest='ntdos-build-manifest.json'; relative='base\mvdm\dos\v86\doskrnl\dos\NTDOS.SYS'; file='NTDOS.SYS'; bytes=27858; sha256='957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84' },
    [ordered]@{ role='command'; stage='build\M0-T225-S8\command-primary-source-build-r6'; manifest='command-build-manifest.json'; relative='base\mvdm\dos\v86\cmd\command\COMMAND.COM'; file='COMMAND.COM'; bytes=50384; sha256='908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43' },
    [ordered]@{ role='target'; source_role='share'; stage='build\M0-T225-S8\share-primary-source-build-r1'; manifest='share-build-manifest.json'; relative='base\mvdm\dos\v86\cmd\share\SHARE.EXE'; file='TARGET.EXE'; bytes=882; sha256='69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc' }
)
foreach ($item in $items) {
    $stageRoot = Join-Path $root $item.stage; $manifest = Join-Path $stageRoot $item.manifest; $artifact = Join-Path $stageRoot $item.relative
    if (!(Test-Path -LiteralPath $manifest -PathType Leaf) -or !(Test-Path -LiteralPath $artifact -PathType Leaf)) { throw "Primary stage input missing for $($item.role)" }
    $manifestObject = Get-Content -Raw -LiteralPath $manifest | ConvertFrom-Json
    if ($manifestObject.classification -ne 'primary-original-toolchain-source-built' -or (Get-Item -LiteralPath $artifact).Length -ne $item.bytes -or (Get-Sha256 $artifact) -ne $item.sha256) { throw "Primary source-built identity mismatch for $($item.role)" }
    $item.stage_root = $item.stage.Replace('\','/'); $item.manifest_sha256 = Get-Sha256 $manifest
}
New-Item -ItemType Directory -Path $output | Out-Null
foreach ($item in $items) {
    $source = Join-Path $root ($item.stage_root.Replace('/','\') + '\' + $item.relative); $destination = Join-Path $output $item.file
    Copy-Item -LiteralPath $source -Destination $destination
    if ((Get-Item -LiteralPath $destination).Length -ne $item.bytes -or (Get-Sha256 $destination) -ne $item.sha256) { throw "Copied S48 identity mismatch for $($item.role)" }
}
$profile = [ordered]@{
    schema='ntdos64-byob-profile-v8'; profile='nt4-en-us-cli-stream-v8'; architecture='x86'; locale='en-US'; compatibility_group='t225-s48-guest-exec-lifecycle-ledger'
    components=@($items | ForEach-Object { [ordered]@{role=$_.role;file_name=$_.file;required=$true;bytes=$_.bytes;sha256=$_.sha256;version=$null} })
    features=@(); owner_note='T225 S48 source-built whole-lifecycle ledger fixture; TARGET.EXE is a byte-identical profile alias for source-built SHARE.EXE.'
    guest_command_placement=[ordered]@{path='\COMMAND.COM';drive_index=2}
    guest_boot_files=[ordered]@{config=[ordered]@{path='\CONFIG.SYS';materialization='minimal-comment-v1'};autoexec=[ordered]@{path='\AUTOEXEC.BAT';materialization='empty-v1'}}
    guest_declared_targets=@([ordered]@{role='target';placement=[ordered]@{path='\TARGET.EXE';drive_index=2}})
    guest_search_metadata=[ordered]@{command=[ordered]@{attributes=32;dos_time=1;dos_date=33};target=[ordered]@{attributes=32;dos_time=2;dos_date=34};config=[ordered]@{attributes=32;dos_time=3;dos_date=35};autoexec=[ordered]@{attributes=32;dos_time=4;dos_date=36}}
    guest_display_state='stream-io-v1'; machine_external_initial_state_evidence_sha256='168219f4c7d73abee06b62b1423298a2f111685e1bf36da23b2c16a642310195'; machine_startup_plan=$true; machine_startup_entry='ntio-v0'
}
[IO.File]::WriteAllText((Join-Path $output 'profile.json'), (($profile | ConvertTo-Json -Depth 8) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText((Join-Path $output 'fixture-config.nt'), "files=20`r`n", [Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText((Join-Path $output 'fixture-autoexec.nt'), '', [Text.UTF8Encoding]::new($false))
[ordered]@{schema='ntdos64.t225.s48.guest-exec-lifecycle-ledger-inputs.v1';classification='primary-original-toolchain-source-built';sourceRoot=$root;inputs=$items;targetAlias=[ordered]@{source_role='share';source_name='SHARE.EXE';profile_name='TARGET.EXE';content_changed=$false};allowedUse=@('T225 S48 bounded Direct/Readonly whole-lifecycle ledger observation');forbiddenUse=@('reference fallback','default runtime discovery','release packaging','BOP semantic expansion','Overlay selection','Virtual selection')} | ConvertTo-Json -Depth 7 | ForEach-Object { [IO.File]::WriteAllText((Join-Path $output 'guest-exec-lifecycle-ledger-inputs-manifest.json'), $_ + [Environment]::NewLine, [Text.UTF8Encoding]::new($false)) }
Write-Host "Staged source-built T225 S48 guest lifecycle-ledger fixture inputs: $output"