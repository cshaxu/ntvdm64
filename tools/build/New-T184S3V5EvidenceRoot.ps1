[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)][string]$EvidenceRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$evidence = [IO.Path]::GetFullPath($EvidenceRoot)
if (Test-Path -LiteralPath $evidence) { throw "Refusing to overwrite existing evidence root: $evidence" }
$sourceRoot = Join-Path $repository 'artifacts\analysis\t182-s1-share-normal-return-direct-observation-001\byob'
$quitSource = Join-Path $repository 'artifacts\toolchain-runs\quit-tools16-opennt-v1\base\mvdm\v86\util\QUIT.COM'

function Copy-Component([string]$Source, [string]$Destination, [string]$Role) {
    if (-not (Test-Path -LiteralPath $Source -PathType Leaf)) { throw "Missing $Role input: $Source" }
    $hash = (Get-FileHash -LiteralPath $Source -Algorithm SHA256).Hash.ToLowerInvariant()
    $bytes = (Get-Item -LiteralPath $Source).Length
    Copy-Item -LiteralPath $Source -Destination $Destination -ErrorAction Stop
    if ((Get-FileHash -LiteralPath $Destination -Algorithm SHA256).Hash.ToLowerInvariant() -ne $hash -or
        (Get-Item -LiteralPath $Destination).Length -ne $bytes) { throw "Copy verification failed: $Role" }
    return [ordered]@{ role=$Role; file_name=(Split-Path -Leaf $Destination); required=$true; bytes=$bytes; sha256=$hash; version=$null }
}

New-Item -ItemType Directory -Path $evidence -ErrorAction Stop | Out-Null
$components = @(
    (Copy-Component (Join-Path $sourceRoot 'NTIO.SYS') (Join-Path $evidence 'NTIO.SYS') 'ntio'),
    (Copy-Component (Join-Path $sourceRoot 'NTDOS.SYS') (Join-Path $evidence 'NTDOS.SYS') 'ntdos'),
    (Copy-Component (Join-Path $sourceRoot 'COMMAND.COM') (Join-Path $evidence 'COMMAND.COM') 'command'),
    (Copy-Component (Join-Path $sourceRoot 'TARGET.EXE') (Join-Path $evidence 'TARGET.EXE') 'target'),
    (Copy-Component $quitSource (Join-Path $evidence 'QUIT.COM') 'terminal-quit')
)
if ($components[4].bytes -ne 3 -or $components[4].sha256 -ne '06a37dff559df7325de8b003f4df53c188f733e0ca312aad961c34dae48d7b83') {
    throw 'QUIT.COM does not have the approved source-built identity.'
}
$metadata = [ordered]@{
    command=[ordered]@{attributes=32;dos_time=1;dos_date=33}
    target=[ordered]@{attributes=32;dos_time=2;dos_date=34}
    'terminal-quit'=[ordered]@{attributes=32;dos_time=3;dos_date=35}
    config=[ordered]@{attributes=32;dos_time=4;dos_date=36}
    autoexec=[ordered]@{attributes=32;dos_time=5;dos_date=37}
}
$profile = [ordered]@{
    schema='ntdos64-byob-profile-v5'; profile='nt4-en-us-command-smoke-v5'; architecture='x86'; locale='en-US'
    compatibility_group='t184-v5-finite-sequence'; components=$components; features=@(); owner_note=$null
    guest_command_placement=[ordered]@{drive_index=2;path='\COMMAND.COM'}
    guest_boot_files=[ordered]@{config=[ordered]@{path='\CONFIG.SYS';materialization='minimal-comment-v1'};autoexec=[ordered]@{path='\AUTOEXEC.BAT';materialization='empty-v1'}}
    guest_declared_targets=@(
        [ordered]@{role='target';placement=[ordered]@{path='\TARGET.EXE';drive_index=2}},
        [ordered]@{role='terminal-quit';placement=[ordered]@{path='\QUIT.COM';drive_index=2}}
    )
    guest_search_metadata=$metadata
    host_drive_inventory=[ordered]@{types=@(1,1,3,3,3,1,3,1,5,1,1,1,1,1,3,1,1,1,1,1,1,1,1,3,3,1)}
    machine_startup_plan=$true; machine_startup_entry='ntio-v0'
    machine_startup_handoff=[ordered]@{exception_vector=6;instruction_bytes='0f0b';cs=51840;ip=3}
}
$profilePath = Join-Path $evidence 'profile.json'
[IO.File]::WriteAllText($profilePath, ($profile | ConvertTo-Json -Compress -Depth 8), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared T184 v5 evidence root: $evidence"
