[CmdletBinding()]
param([string]$OutputDirectory='build/M0-T405/S1/all-roots-001')
$ErrorActionPreference='Stop'
$repo=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$output=[IO.Path]::GetFullPath((Join-Path $repo $OutputDirectory))
$build=[IO.Path]::GetFullPath((Join-Path $repo 'build'))+[IO.Path]::DirectorySeparatorChar
if (-not $output.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)) { throw 'Output must be below build.' }
if (Test-Path -LiteralPath $output) { throw 'Output already exists.' }
$files=@(& git -C $repo ls-files src)
if ($LASTEXITCODE -ne 0) { throw 'Cannot enumerate tracked source.' }
$roles=@{
    'mvdm-host'='S2-original-mvdm'; 'mvdm-host-overlay'='S2-overlay';
    'mvdm-softpc-patch'='S2-adopted-patch'; 'opennt-host'='S3-original-non-mvdm';
    'adapter-mvdm-host-out'='S2-S3-assign-by-original-owner';
    'adapter-opennt-host'='S3-private-host-binding'; 'opennt-abi'='S2-S3-declaration-provenance';
    'app'='S2-S3-composition-or-replacement'; 'session'='S2-S3-neutral-or-replacement';
    'broker'='S3-coordination-or-replacement'; 'mvdm-guest'='load-only-guest-evidence';
    'mvdm-softpc-firmware'='load-only-firmware-evidence'; 'mvdm-tools'='independent-tool-evidence'
}
$textExtensions=@('.c','.h','.cpp','.cxx','.hpp','.asm','.inc','.def','.rc','.patch')
$rows=foreach($name in $files){
    $root=($name -split '/')[1]
    if (-not $roles.ContainsKey($root)) { throw "Unclassified source root: $root" }
    $absolute=Join-Path $repo $name
    $bytes=[IO.File]::ReadAllBytes($absolute)
    $extension=[IO.Path]::GetExtension($name).ToLowerInvariant()
    $kind='other-data-or-build-input'; $lines=$null
    if ($extension -eq '.md') { $kind='documentation' }
    elseif ($textExtensions -contains $extension) {
        if ($bytes -contains 0) { $kind='binary-or-non-byte-text-review' }
        else {
            $kind='source-text'
            # Physical lines including comments/blanks, not SLOC or upstream diff.
            $lines=0
            foreach($byte in $bytes){if($byte -eq 10){$lines++}}
            if ($bytes.Length -gt 0 -and $bytes[$bytes.Length-1] -ne 10){$lines++}
        }
    }
    [pscustomobject]@{path=$name;root=$root;review_assignment=$roles[$root];kind=$kind;
        bytes=$bytes.Length;physical_lines=$lines;sha256=(Get-FileHash -LiteralPath $absolute).Hash;
        actual_build_selection='not-established-by-inventory'}
}
if (@($rows).Count -ne $files.Count) { throw 'Inventory is incomplete.' }
$null=New-Item -ItemType Directory -Path $output
$rows | Export-Csv -LiteralPath (Join-Path $output 'inventory.csv') -NoTypeInformation -Encoding UTF8
$rows | Group-Object root | ForEach-Object {
    $source=@($_.Group | Where-Object kind -eq 'source-text')
    [pscustomobject]@{root=$_.Name;tracked=$_.Count;source_text_files=$source.Count;
        physical_source_lines=($source | Measure-Object physical_lines -Sum).Sum}
} | ConvertTo-Json
