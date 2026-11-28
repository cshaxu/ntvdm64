# Shared immutable guest input supply for fixtures; no loose-source fallback.
function Copy-VerifiedGuestArchiveMembers {
    param([Parameter(Mandatory)][string]$Destination,
          [Parameter(Mandatory)][Collections.IDictionary]$Members)
    $repo=(Resolve-Path "$PSScriptRoot/../..").Path
    $archive=Join-Path $repo 'assets/winnt.zip'
    $manifest=Get-Content "$repo/assets/winnt-guest-manifest.json" -Raw|ConvertFrom-Json
    if((Get-FileHash $archive).Hash -ne $manifest.archive_sha256){throw 'Guest archive hash mismatch'}
    New-Item -ItemType Directory -Force $Destination|Out-Null
    $destinationRoot=(Resolve-Path $Destination).Path
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $zip=[IO.Compression.ZipFile]::OpenRead($archive)
    try {
        foreach($member in $Members.Keys){
            $row=@($manifest.files|Where-Object path -eq $member)
            $entry=$zip.GetEntry($member)
            if($row.Count -ne 1 -or !$entry){throw "Missing declared guest: $member"}
            $target=[IO.Path]::GetFullPath((Join-Path $destinationRoot $Members[$member]))
            if(!$target.StartsWith($destinationRoot+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Guest target outside fixture root'}
            New-Item -ItemType Directory -Force ([IO.Path]::GetDirectoryName($target))|Out-Null
            [IO.Compression.ZipFileExtensions]::ExtractToFile($entry,$target,$true)
            $hash=(Get-FileHash $target).Hash
            if($hash -ne $row[0].sha256 -or (Get-Item $target).Length -ne $row[0].bytes){throw "Guest identity mismatch: $member"}
            [pscustomobject]@{source="assets/winnt.zip:$member";target=$Members[$member];path=$target;sha256=$hash}
        }
    } finally {$zip.Dispose()}
}
