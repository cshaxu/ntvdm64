param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path,
    [string]$OutputDirectory = (Join-Path (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path 'docs')
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$out = [System.IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Force -Path $out | Out-Null

# This exports evidence, rather than attempting a historical build.  In
# particular, a binplace rule is never promoted to a source-build route.
$sourceManifest = Get-Content -Raw (Join-Path $root 'src/opennt/SOURCE-MANIFEST.json') | ConvertFrom-Json
$revision = $sourceManifest.upstream_revision
$records = [System.Collections.Generic.List[object]]::new()

function Relative([string]$path) {
    return $path.Substring($root.Length + 1).Replace('\', '/')
}

function Add-Record([object]$record) { $records.Add([pscustomobject]$record) }

# The retained stage manifests are the authoritative output identity records.
# Their sibling historical-*-manifest.json files retain the actual MASM/LINK16
# invocations and post-link logs, when a stage has those steps.
Get-ChildItem -LiteralPath (Join-Path $root 'artifacts/toolchain-runs') -Recurse -Filter '*-build-manifest.json' |
    Sort-Object FullName | ForEach-Object {
        $stageManifestPath = $_.FullName
        $manifest = Get-Content -Raw $stageManifestPath | ConvertFrom-Json
        $stage = $_.Directory.Name
        if ($stage -eq 'msdos6-msherc-native-v1') { return } # research sample, never NT4 closure evidence
        $isWow = $stage -like 'wow16-*'
        $commandEvidence = @(Get-ChildItem -LiteralPath $_.Directory.FullName -Recurse -Filter 'historical-*-manifest.json' -ErrorAction SilentlyContinue |
            Sort-Object FullName | ForEach-Object { Relative $_.FullName })
        $output = Split-Path $manifest.Output -Leaf
        $sourceAnchor = 'retained stage input tree'
        if ($manifest.Output -match '(?i)(base[\\/]mvdm[\\/].+)[\\/]([^\\/]+)$') {
            $sourceAnchor = ($Matches[1] -replace '\\','/')
        }
        Add-Record @{
            Artifact = $output.ToUpperInvariant(); Scope = if ($isWow) { 'WOW16 driver' } else { 'DOS guest payload' }
            Classification = 'source-built'; SourceRevision = $revision
            SourceAnchor = $sourceAnchor
            Evidence = @{ BuildManifest = Relative $stageManifestPath; CommandManifests = $commandEvidence; Stage = $stage
                OutputSha256 = $manifest.Sha256; OutputBytes = $manifest.ByteCount; Tools = @($manifest.Tools)
                GeneratedInputs = @($manifest.HistoricalLibraryInputs) + @($manifest.HistoricalStubInput.Path) }
            FirstBlocker = $null; RuntimeAdmission = 'not runtime-admitted by source-build evidence'
        }
    }

foreach ($name in 'GRAPHICS.PRO','QBASIC.HLP','EDIT.HLP','GORILLA.BAS','MONEY.BAS','NIBBLES.BAS','REMLINE.BAS') {
    Add-Record @{ Artifact=$name; Scope='DOS guest payload'; Classification='static-data'; SourceRevision=$revision
        SourceAnchor='base/mvdm/dos/v86/cmd/{graphics,qbasic}/makefile install rule'; Evidence=@{Kind='source snapshot companion/data'}; FirstBlocker=$null; RuntimeAdmission='not runtime-admitted' }
}

$prebuilt = Get-Content -Raw (Join-Path $root 'artifacts/toolchain-runs/nt4-prebuilt-guest-inputs-v1/prebuilt-inputs-manifest.json') | ConvertFrom-Json
foreach ($item in $prebuilt.Records) {
    Add-Record @{ Artifact=$item.Name; Scope='DOS guest payload'; Classification='excluded-prebuilt'; SourceRevision=$revision
        SourceAnchor=$item.BuildRule; Evidence=@{Manifest='artifacts/toolchain-runs/nt4-prebuilt-guest-inputs-v1/prebuilt-inputs-manifest.json'; SourceBuilt=$item.SourceBuilt; OutputSha256=$item.StagedSha256; OutputBytes=$item.Bytes}; FirstBlocker='Fixed OpenNT tree has only binplace ownership; matching source/build rule is absent.'; RuntimeAdmission='not runtime-admitted' }
}

$full = Get-Content -Raw (Join-Path $root 'docs/opennt-mvdm-artifact-inventory.json') | ConvertFrom-Json
$alreadyCovered = @('ansi.sys','append.exe','command.com','debug.exe','edlin.exe','exe2bin.exe','fastopen.exe','himem.sys','kb16.com','keyboard.sys','loadfix.com','mem.exe','nlsfunc.exe','ntdos.sys','ntio.sys','redir.exe','setver.exe','share.exe','dosx.exe')
$sampleNames = @('adlibvdd','ddemlcl','ddemlsv','ddemo','ddestrs','fax32','vcdex','vdd','vsbd','buildidx','convert','dasm386','dat2obj','getsize','rename','stripdd','stripz')
foreach ($descriptor in $full) {
    foreach ($declared in @($descriptor.DeclaredOutputs)) {
        $value = ($declared -split '=',2)[1]
        $normalized = ($value -replace '^.*\\','' -replace '^.*\/','').ToLowerInvariant()
        if ($descriptor.Descriptor -eq 'dpmi/makefile' -and $value -eq '.') { continue } # resolved by the DOSX.EXE retained stage
        if ($alreadyCovered -contains $normalized) { continue }
        $path = $descriptor.Descriptor
        $isWow = $path -like 'wow16/*'
        $isSample = $path -match '(^|/)(samples|test|tests)/' -or $sampleNames -contains $value
        $classification = if ($isSample) { 'sample/development-only' } elseif ($isWow) { 'source-blocked' } else { 'host/support' }
        $blocker = if ($isSample) { 'Excluded from the research runtime profile; no build campaign is admitted.' }
                   elseif ($isWow) { "First missing input: fixed src/opennt does not contain original descriptor/source directory base/mvdm/$($path -replace '/(makefile|sources)$','')." }
                   else { 'No standalone installed artifact is defined by the retained target; final historical x86 product composition/runtime admission is unrecovered.' }
        $scope = if ($isWow) { 'WOW16 runtime/tool candidate' } elseif ($path -match '(^|/)(dpmi|sim16|v86/)') { 'non-DOS/WOW16 guest-side candidate' } else { 'VDM host/support candidate' }
        Add-Record @{ Artifact="$($descriptor.Descriptor):$declared"; Scope=$scope; Classification=$classification; SourceRevision=$revision
            SourceAnchor="full-reference descriptor census: base/mvdm/$($descriptor.Descriptor)"; Evidence=@{DescriptorInventory='docs/opennt-mvdm-artifact-inventory.json'; ToolTokens=@($descriptor.ToolTokens)}; FirstBlocker=$blocker; RuntimeAdmission='not runtime-admitted' }
    }
}

# Most WOW16 makefiles in the full-reference census deliberately have no
# TARGETNAME/dest declaration.  They are still output candidates: preserving a
# logical record here prevents the five driver outputs from being mistaken for
# the entire WOW16 runtime merely because the historical makefile is implicit.
$rebuiltDriverDescriptors = @(
    'wow16/drivers/comm/makefile', 'wow16/drivers/display/makefile',
    'wow16/drivers/keyboard/makefile', 'wow16/drivers/mouse/makefile',
    'wow16/drivers/sound/makefile'
)
foreach ($descriptor in $full | Where-Object { $_.Descriptor -like 'wow16/*/makefile' -and @($_.DeclaredOutputs).Count -eq 0 }) {
    if ($rebuiltDriverDescriptors -contains $descriptor.Descriptor) { continue }
    $unit = $descriptor.Descriptor -replace '/makefile$',''
    Add-Record @{ Artifact="WOW16 logical output set: $unit"; Scope='WOW16 runtime/tool candidate'; Classification='source-blocked'; SourceRevision=$revision
        SourceAnchor="full-reference descriptor census: base/mvdm/$($descriptor.Descriptor)"; Evidence=@{DescriptorInventory='docs/opennt-mvdm-artifact-inventory.json'; ToolTokens=@($descriptor.ToolTokens)}
        FirstBlocker="First missing input: fixed src/opennt does not contain original descriptor/source directory base/mvdm/$unit."; RuntimeAdmission='not runtime-admitted' }
}

$jsonPath = Join-Path $out 'dos-wow16-source-closure-inventory.json'
$records | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $jsonPath -Encoding utf8
$summary = $records | Group-Object Classification | Sort-Object Name | ForEach-Object { "| ``$($_.Name)`` | $($_.Count) |" }
$md = @(
    '# DOS/WOW16 and Guest-Side Source-Closure Inventory', '',
    "Generated by ``tools/governance/Export-DosWow16SourceClosureInventory.ps1`` from source revision ``$revision``.", '',
    'This is a closure ledger, not a runtime claim. `source-built` records retain output hash/size and command-manifest paths; `source-blocked` records preserve the first missing input. The four `excluded-prebuilt` records are never counted as rebuilt.', '',
    '| Classification | Records |', '| --- | ---: |'
) + $summary + @('',
    'The inventory includes direct DOS payloads, all retained full-reference descriptor declarations, WOW16 runtime/tool candidates, and non-DOS/WOW16 guest-side candidates. A `host/support` classification names scope; its `FirstBlocker` records why it is not a runnable product.', '',
    "Machine-readable records: ``docs/$(Split-Path $jsonPath -Leaf)``."
)
$mdPath = Join-Path $out 'DOS-WOW16-SOURCE-CLOSURE-INVENTORY.md'
$md | Set-Content -LiteralPath $mdPath -Encoding utf8
[pscustomobject]@{ Records=$records.Count; Json=$jsonPath; Markdown=$mdPath; SourceRevision=$revision } | ConvertTo-Json
