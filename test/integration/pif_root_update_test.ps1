param(
    [Parameter(Mandatory)][string]$Pif,
    [Parameter(Mandatory)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$pif = (Resolve-Path -LiteralPath $Pif).Path
$root = [IO.Path]::GetFullPath($BuildRoot)
New-Item -ItemType Directory -Force -Path $root | Out-Null
$fixture = Join-Path $root 'ROOT.PIF'

& $pif create $fixture --title Root --program 'C:\OLD\WIN.COM' --directory 'C:\OLD' --arguments '/S' --config 'C:\OLD\CONFIG.NT' --autoexec 'C:\OLD\AUTOEXEC.NT'
if ($LASTEXITCODE -ne 0) { throw "PIF create failed: $LASTEXITCODE" }
& $pif replace-root $fixture --from 'C:\OLD' --to 'C:\NEW'
if ($LASTEXITCODE -ne 0) { throw "PIF replace-root failed: $LASTEXITCODE" }
$shown = & $pif show $fixture
foreach ($expected in @('PROGRAM=C:\NEW\WIN.COM', 'DIRECTORY=C:\NEW', 'CONFIG=C:\NEW\CONFIG.NT', 'AUTOEXEC=C:\NEW\AUTOEXEC.NT')) {
    if ($shown -notcontains $expected) { throw "Missing updated PIF field: $expected" }
}
'PASS PIF structured root update'
