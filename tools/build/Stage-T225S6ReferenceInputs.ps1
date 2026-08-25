[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$SourceRoot,
    [Parameter(Mandatory = $true)][string]$OutputDirectory
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$source = (Resolve-Path -LiteralPath $SourceRoot).Path
$output = [IO.Path]::GetFullPath($OutputDirectory)
if (Test-Path -LiteralPath $output) {
    throw "Refusing to overwrite reference-input root: $output"
}

function Get-Sha256([string]$Path) {
    $stream = [IO.File]::OpenRead($Path)
    $sha = [Security.Cryptography.SHA256]::Create()
    try {
        return ([BitConverter]::ToString($sha.ComputeHash($stream))).Replace('-', '')
    } finally {
        $sha.Dispose()
        $stream.Dispose()
    }
}

$inputs = @(
    [ordered]@{ role = 'ntio'; file = 'NTIO.SYS'; relative = 'base\mvdm\dos\v86\doskrnl\bios\NTIO.SYS'; bytes = 33792; sha256 = 'CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937' },
    [ordered]@{ role = 'ntdos'; file = 'NTDOS.SYS'; relative = 'base\mvdm\dos\v86\doskrnl\dos\NTDOS.SYS'; bytes = 27858; sha256 = '957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84' },
    [ordered]@{ role = 'command'; file = 'COMMAND.COM'; relative = 'base\mvdm\dos\v86\cmd\command\COMMAND.COM'; bytes = 50384; sha256 = '908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43' },
    [ordered]@{ role = 'target'; file = 'SHARE.EXE'; relative = 'base\mvdm\dos\v86\cmd\share\share.EXE'; bytes = 882; sha256 = '69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC' }
)

$revision = (& git -C $source rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0 -or $revision -ne '5c5b979ec08c17d3ca2eb70e8aad62d26515d01c') {
    throw "Unexpected OpenNT reference revision: $revision"
}
foreach ($input in $inputs) {
    $path = Join-Path $source $input.relative
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing approved reference input: $path"
    }
    $bytes = (Get-Item -LiteralPath $path).Length
    $hash = Get-Sha256 $path
    if ($bytes -ne $input.bytes -or $hash -ne $input.sha256) {
        throw "Reference identity mismatch for $($input.file): bytes=$bytes sha256=$hash"
    }
}

New-Item -ItemType Directory -Path $output | Out-Null
foreach ($input in $inputs) {
    Copy-Item -LiteralPath (Join-Path $source $input.relative) -Destination (Join-Path $output $input.file)
}

$profile = [ordered]@{
    schema = 'runner-byob-profile-v8'
    profile = 'nt4-en-us-cli-stream-v8'
    architecture = 'x86'
    locale = 'en-US'
    compatibility_group = 't225-s6-reference-identity-fixture'
    components = @($inputs | ForEach-Object { [ordered]@{ role = $_.role; file_name = $_.file; required = $true; bytes = $_.bytes; sha256 = $_.sha256.ToLowerInvariant(); version = $null } })
    features = @()
    owner_note = 'Approved local reference-binary research input; identity matches retained source-built ledger but this stage is not a new source build.'
    guest_command_placement = [ordered]@{ path = '\COMMAND.COM'; drive_index = 2 }
    guest_boot_files = [ordered]@{
        config = [ordered]@{ path = '\CONFIG.SYS'; materialization = 'minimal-comment-v1' }
        autoexec = [ordered]@{ path = '\AUTOEXEC.BAT'; materialization = 'empty-v1' }
    }
    guest_declared_targets = @([ordered]@{ role = 'target'; placement = [ordered]@{ path = '\SHARE.EXE'; drive_index = 2 } })
    guest_search_metadata = [ordered]@{
        command = [ordered]@{ attributes = 32; dos_time = 1; dos_date = 33 }
        target = [ordered]@{ attributes = 32; dos_time = 2; dos_date = 34 }
        config = [ordered]@{ attributes = 32; dos_time = 3; dos_date = 35 }
        autoexec = [ordered]@{ attributes = 32; dos_time = 4; dos_date = 36 }
    }
    guest_display_state = 'stream-io-v1'
    machine_external_initial_state_evidence_sha256 = '168219f4c7d73abee06b62b1423298a2f111685e1bf36da23b2c16a642310195'
}
$profileJson = $profile | ConvertTo-Json -Depth 8
[IO.File]::WriteAllText((Join-Path $output 'profile.json'), $profileJson + [Environment]::NewLine, [Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText((Join-Path $output 'fixture-config.nt'), 'files=20' + [Environment]::NewLine, [Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText((Join-Path $output 'fixture-autoexec.nt'), '', [Text.UTF8Encoding]::new($false))

[ordered]@{
    schema = 'runner.t225.s6.reference-guest-inputs.v1'
    classification = 'approved-reference-binary-research-input'
    sourceTree = $source
    sourceRevision = $revision
    inputs = @($inputs | ForEach-Object { [ordered]@{ role = $_.role; source = $_.relative; staged = $_.file; bytes = $_.bytes; sha256 = $_.sha256 } })
    profile = 'profile.json'
    allowedUse = @('T225 S6 fixture installation verification', 'T225 S6 explicit startup-configuration fixture input')
    forbiddenUse = @('default runtime discovery', 'release packaging', 'source-build claim for this staging run', 'guest trace admission')
} | ConvertTo-Json -Depth 6 | ForEach-Object { [IO.File]::WriteAllText((Join-Path $output 'reference-input-manifest.json'), $_ + [Environment]::NewLine, [Text.UTF8Encoding]::new($false)) }

Write-Host "Staged approved T225 S6 reference inputs: $output"
