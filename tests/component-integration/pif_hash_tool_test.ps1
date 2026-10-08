param(
    [Parameter(Mandatory)][string]$Pif,
    [Parameter(Mandatory)][string]$Hash,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$ReleaseRoot
)
$ErrorActionPreference = 'Stop'
New-Item -ItemType Directory -Force -Path $BuildRoot | Out-Null
$sample = Join-Path $BuildRoot 'sample.pif'
$invalid = Join-Path $BuildRoot 'invalid.pif'
$tampered = Join-Path $BuildRoot 'tampered.pif'
& $Pif create $sample --title Sample --program 'C:\DOS\COMMAND.COM' --directory 'C:\DOS' --arguments '/P' --config 'C:\PATCH\CONFIG.NT' --autoexec 'C:\PATCH\AUTOEXEC.NT'
if ($LASTEXITCODE -ne 0) { throw 'PIF create failed' }
$initial = & $Pif show $sample
if ($LASTEXITCODE -ne 0 -or
    $initial -notcontains 'TITLE=Sample' -or
    $initial -notcontains 'STANDARD.MAXMEM_KB=0' -or
    $initial -notcontains 'W386.MAX_XMS_KB=1024' -or
    $initial -notcontains 'NT31.FLAGS=0x00000000') {
    throw 'PIF show did not expose standard, Windows 386, and NT 3.1 fields'
}
& $Pif update $sample --title Edited --arguments '/C VER'
if ($LASTEXITCODE -ne 0) { throw 'PIF update failed' }
$edited = & $Pif show $sample
if ($LASTEXITCODE -ne 0 -or $edited -notcontains 'TITLE=Edited' -or $edited -notcontains 'ARGUMENTS=/C VER') { throw 'PIF update did not persist fields' }
[IO.File]::WriteAllBytes($invalid, [Text.Encoding]::ASCII.GetBytes('not a pif'))
$priorPreference = $ErrorActionPreference
$ErrorActionPreference = 'Continue'
& $Pif show $invalid 2>$null
$invalidExit = $LASTEXITCODE
$ErrorActionPreference = $priorPreference
if ($invalidExit -eq 0) { throw 'PIF show accepted malformed input' }
$bytes = [IO.File]::ReadAllBytes($sample)
$bytes[10] = $bytes[10] -bxor 1
[IO.File]::WriteAllBytes($tampered, $bytes)
$ErrorActionPreference = 'Continue'
& $Pif show $tampered 2>$null
$tamperedExit = $LASTEXITCODE
$ErrorActionPreference = $priorPreference
if ($tamperedExit -eq 0) { throw 'PIF show accepted a bad fixed-record checksum' }
$legacy = Join-Path $BuildRoot 'legacy-no-checksum.pif'
Copy-Item -LiteralPath $sample -Destination $legacy
$bytes = [IO.File]::ReadAllBytes($legacy)
$bytes[1] = 0
[IO.File]::WriteAllBytes($legacy, $bytes)
& $Pif show $legacy | Out-Null
if ($LASTEXITCODE -ne 0) { throw 'PIF show rejected a valid zero-checksum legacy PIF' }
& $Pif update $legacy --title Legacy
if ($LASTEXITCODE -ne 0) { throw 'PIF update rejected a valid zero-checksum legacy PIF' }
if (([IO.File]::ReadAllBytes($legacy))[1] -ne 0) { throw 'PIF update invented a checksum for a legacy zero-checksum PIF' }
$expected = (Get-FileHash -Algorithm SHA256 -LiteralPath $sample).Hash
$actual = (& $Hash $sample).Trim()
if ($LASTEXITCODE -ne 0 -or $actual -ne $expected -or $actual -notmatch '^[0-9A-F]{64}$') { throw 'HASH output differs from SHA-256 reference' }
$ErrorActionPreference = 'Continue'
& $Hash (Join-Path $BuildRoot 'missing.bin') 2>$null
$missingHashExit = $LASTEXITCODE
$ErrorActionPreference = $priorPreference
if ($missingHashExit -eq 0) { throw 'HASH accepted a nonexistent file' }
if ($ReleaseRoot) {
    $manifestPath = Join-Path $ReleaseRoot 'utility-manifest.json'
    $manifest = Get-Content -Raw -LiteralPath $manifestPath | ConvertFrom-Json
    if ($manifest.schema -ne 1 -or $manifest.utilities.Count -ne 2) { throw 'Invalid utility release manifest' }
    foreach ($utility in $manifest.utilities) {
        if ($utility.architecture -ne 'amd64') { throw "Unexpected utility architecture: $($utility.file)" }
        $image = Join-Path $ReleaseRoot $utility.file
        if (!(Test-Path -LiteralPath $image -PathType Leaf)) { throw "Missing released utility: $($utility.file)" }
        if ((Get-FileHash -LiteralPath $image).Hash -ne $utility.sha256) { throw "Released utility hash mismatch: $($utility.file)" }
    }
}
'PASS PIF create/show/update and HASH single-file output'
