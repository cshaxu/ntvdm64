param(
    [Parameter(Mandatory)][string]$InstallRoot,
    [Parameter(Mandatory)][string]$Apply,
    [Parameter(Mandatory)][string]$Unapply,
    [Parameter(Mandatory)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
$apply = (Resolve-Path -LiteralPath $Apply).Path
$unapply = (Resolve-Path -LiteralPath $Unapply).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
New-Item -ItemType Directory -Force -Path $build | Out-Null
$manifest = Join-Path $root 'PATCH\PATH-REPAIR.MANIFEST'

function Invoke-InteractiveCmd([string]$script, [string]$target, [string]$name) {
    $input = Join-Path $build "$name.in"
    [IO.File]::WriteAllText($input, "$target`r`n`r`n", [Text.Encoding]::ASCII)
    & cmd.exe /d /c "`"$script`" < `"$input`""
    if ($LASTEXITCODE -ne 0) { throw "$name failed: $LASTEXITCODE" }
}

function Test-BytesEqual([byte[]]$left, [byte[]]$right) {
    if ($left.Length -ne $right.Length) { return $false }
    for ($index = 0; $index -lt $left.Length; ++$index) {
        if ($left[$index] -ne $right[$index]) { return $false }
    }
    return $true
}

$before = @{}
Get-ChildItem -LiteralPath $root -File -Include *.INI,*.GRP | ForEach-Object {
    $before[$_.Name] = [IO.File]::ReadAllBytes($_.FullName)
}
Invoke-InteractiveCmd $apply $root 'apply-first'
if (!(Test-Path -LiteralPath $manifest -PathType Leaf)) { throw 'Apply did not create a recovery manifest' }
$entries = @(Get-Content -LiteralPath $manifest | ForEach-Object {
    if ($_ -notmatch '^[^\\/:*?"<>|]+\.(INI|GRP)$') { throw "Malformed recovery manifest entry: $_" }
    [pscustomobject]@{ Name = $_ }
})
if (!$entries.Count) { throw 'Recovery manifest was empty' }
foreach ($entry in $entries) {
    $live = Join-Path $root $entry.Name
    $backup = "$live.BAK"
    if (!(Test-Path -LiteralPath $backup -PathType Leaf)) { throw "Missing adjacent backup: $backup" }
    if (!(Test-BytesEqual ([IO.File]::ReadAllBytes($backup)) $before[$entry.Name])) { throw "Backup contents mismatch: $($entry.Name)" }
    if (Test-BytesEqual ([IO.File]::ReadAllBytes($live)) $before[$entry.Name]) { throw "Repaired file did not change: $($entry.Name)" }
}

Invoke-InteractiveCmd $apply $root 'apply-repeat'
Set-Content -LiteralPath (Join-Path $root 'PATCH\KEEP.TXT') -Value 'owner content' -NoNewline
Invoke-InteractiveCmd $unapply $root 'unapply-positive'
foreach ($entry in $entries) {
    $live = Join-Path $root $entry.Name
    if (!(Test-BytesEqual ([IO.File]::ReadAllBytes($live)) $before[$entry.Name])) { throw "Unapply did not restore: $($entry.Name)" }
    if (Test-Path -LiteralPath "$live.BAK") { throw "Unapply retained consumed backup: $($entry.Name)" }
}
if (!(Test-Path -LiteralPath (Join-Path $root 'PATCH\KEEP.TXT'))) { throw 'Unapply removed unknown PATCH content' }

'PASS Win31 path apply/repeat/unapply with adjacent backups'
