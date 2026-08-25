$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$bochsRoot = Join-Path $repositoryRoot 'refs\bochs'
$mantleRoot = Join-Path $repositoryRoot 'src\adapter-bochs'
$expectedCounts = [ordered]@{
    '..\adapter-bochs\paramtree.cc' = 2
    'gui\siminterface.cc' = 2
    'iodev\hdimage\hdimage.cc' = 1
    'iodev\usb\scsi_device.cc' = 3
    'iodev\harddrv.cc' = 2
    'iodev\pit.cc' = 3
    'iodev\virt_timer.cc' = 1
}
$expectedForms = @(
    [pscustomobject]@{ File = '..\adapter-bochs\paramtree.cc'; Text = '"0x" FMT_LL "x"'; Matches = 2; Occurrences = 1 }
    [pscustomobject]@{ File = 'gui\siminterface.cc'; Text = 'FMT_LL "u\n"'; Matches = 1; Occurrences = 1 }
    [pscustomobject]@{ File = 'gui\siminterface.cc'; Text = '"0x" FMT_LL "x"'; Matches = 1; Occurrences = 1 }
    [pscustomobject]@{ File = 'iodev\hdimage\hdimage.cc'; Text = '"hd_size: " FMT_LL "u"'; Matches = 1; Occurrences = 1 }
    [pscustomobject]@{ File = 'iodev\usb\scsi_device.cc'; Text = '"Read (sector " FMT_LL "d, count %d)"'; Matches = 1; Occurrences = 1 }
    [pscustomobject]@{ File = 'iodev\usb\scsi_device.cc'; Text = '"Write (sector " FMT_LL "d, count %d)"'; Matches = 1; Occurrences = 1 }
    [pscustomobject]@{ File = 'iodev\usb\scsi_device.cc'; Text = '"Syncronise cache (sector " FMT_LL "d, count %d)"'; Matches = 1; Occurrences = 1 }
    [pscustomobject]@{ File = 'iodev\harddrv.cc'; Text = '"calc_log_addr: out of bounds (" FMT_LL "d/" FMT_LL "d)"'; Matches = 1; Occurrences = 2 }
    [pscustomobject]@{ File = 'iodev\pit.cc'; Text = '"s.last_usec=" FMT_LL "d"'; Matches = 3; Occurrences = 1 }
    [pscustomobject]@{ File = 'iodev\virt_timer.cc'; Text = '"diff: " FMT_LL "u\n"'; Matches = 1; Occurrences = 1 }
)

$adjacent = @()
Get-ChildItem -LiteralPath $bochsRoot,$mantleRoot -Recurse -File -Include *.c,*.cc,*.h | ForEach-Object {
    $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $_.FullName) {
        $lineNumber++
        if ($line.Contains('"FMT_LL') -or $line.Contains('FMT_LL"')) {
            $relative = $_.FullName.Substring($bochsRoot.Length + 1)
            $adjacent += "${relative}:$lineNumber"
        }
    }
}
if ($adjacent.Count -ne 0) { throw "Unspaced FMT_LL literal adjacency remains: $($adjacent -join ', ')" }

$actualCounts = @{}
foreach ($form in $expectedForms) {
    $path = Join-Path $bochsRoot $form.File
    $raw = Get-Content -LiteralPath $path -Raw
    $matchCount = [regex]::Matches($raw, [regex]::Escape($form.Text)).Count
    if ($matchCount -ne $form.Matches) { throw "Expected $($form.Matches) spaced form matches for '$($form.Text)' in $($form.File), found $matchCount." }
    if (-not $actualCounts.ContainsKey($form.File)) { $actualCounts[$form.File] = 0 }
    $actualCounts[$form.File] += $matchCount * $form.Occurrences
}
$actualFileSet = [string]::Join(',', @($actualCounts.Keys | Sort-Object))
$expectedFileSet = [string]::Join(',', @($expectedCounts.Keys | Sort-Object))
if ($actualFileSet -cne $expectedFileSet) { throw "FMT_LL repair set contains an unexpected or missing source file: actual '$actualFileSet', expected '$expectedFileSet'." }
foreach ($relative in $expectedCounts.Keys) {
    if ($actualCounts[$relative] -ne $expectedCounts[$relative]) { throw "Expected $($expectedCounts[$relative]) registered FMT_LL repairs in $relative, found $($actualCounts[$relative])." }
}

$osdep = Get-Content -LiteralPath (Join-Path $bochsRoot 'osdep.h') -Raw
$expectedDefinitions = @('#define FMT_LL "%ll"', '#define FMT_LL "%I64"', '#define FMT_LL "%I64"', '#define FMT_LL "%l"', '#define FMT_LL "%ll"')
if ([regex]::Matches($osdep, '(?m)^#define FMT_LL ".*"$').Count -ne $expectedDefinitions.Count) { throw 'FMT_LL macro-definition count changed.' }
foreach ($definition in $expectedDefinitions) {
    if ($osdep.IndexOf($definition, [System.StringComparison]::Ordinal) -lt 0) { throw "FMT_LL macro definition changed or missing: $definition" }
}

Write-Host 'Bochs FMT_LL lexical-boundary verification passed: 14 registered adjacencies repaired across 7 files.'
