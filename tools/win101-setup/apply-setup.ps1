param([string]$MediaRoot,
      [string]$ReleaseRoot,
      [string]$SetverPath,
      [string]$PifTemplate)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
if(!$MediaRoot){$MediaRoot=Read-Host 'Original Windows 1.01 installation media directory'}
if(!$MediaRoot){throw 'No installation media directory supplied'}
$media=(Resolve-Path -LiteralPath $MediaRoot).Path
if((Get-Item -LiteralPath $media).Attributes -band [IO.FileAttributes]::ReparsePoint){throw 'Media directory must not be a link'}
if(!$ReleaseRoot){$ReleaseRoot=Join-Path $repo 'assets/release'}
$release=(Resolve-Path -LiteralPath $ReleaseRoot).Path
foreach($name in @('SETUP.EXE','SETUP.LBL','KERNEL.EXE','USER.EXE','GDI.EXE','DISK1','DISK2','DISK3','DISK4','DISK5')) {
    if(!(Test-Path -LiteralPath (Join-Path $media $name) -PathType Leaf)){throw "Missing original media: $name"}
}
$manifest=Get-Content -LiteralPath (Join-Path $release 'addon-manifest.json') -Raw|ConvertFrom-Json
$entry=@($manifest.drivers|Where-Object {$_.id -eq 'win101'})
if($manifest.schema -ne 1 -or $entry.Count -ne 1 -or $entry[0].file -ne 'MOUSE101.DRV') {
    throw 'Invalid release mouse-driver manifest'
}
$driver=Join-Path $release $entry[0].file
if((Get-FileHash -LiteralPath $driver).Hash -ne $entry[0].sha256){throw 'Release mouse-driver hash mismatch'}

# All inputs are read before creating PATCH. Existing original media are never
# replaced; the accepted archive supplies only its PIF/SETVER support files.
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=[IO.Compression.ZipFile]::OpenRead((Join-Path $repo 'assets/win101-setup.zip'))
$files=@{};$baseline=@{}
try {
    foreach($name in @('MOUSE.DRV','SETVER.EXE','WIN31-TEMPLATE.PIF','configure-launch.ps1',
        'run-setup.ps1','win.cmd.template','SETUP.CMD','README.txt')) {
        $original=$archive.GetEntry('win101-setup/PATCH/'+$name)
        if($original) {
            $stream=$original.Open();$memory=[IO.MemoryStream]::new()
            try{$stream.CopyTo($memory);$baseline[$name]=$memory.ToArray()}
            finally{$stream.Dispose();$memory.Dispose()}
        }
    }
    foreach($name in @('SETVER.EXE','WIN31-TEMPLATE.PIF')) {
        if(!$baseline.ContainsKey($name)){throw "Missing accepted installer support: $name"}
        $files[$name]=$baseline[$name]
    }
} finally {$archive.Dispose()}
if($SetverPath){$files['SETVER.EXE']=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $SetverPath).Path)}
if($PifTemplate){$files['WIN31-TEMPLATE.PIF']=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $PifTemplate).Path)}
$files['MOUSE.DRV']=[IO.File]::ReadAllBytes($driver)
foreach($name in @('configure-launch.ps1','run-setup.ps1','win.cmd.template')) {
    $files[$name]=[IO.File]::ReadAllBytes((Join-Path $PSScriptRoot $name))
}
$files['SETUP.CMD']=[IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'setup.cmd.template'))
$files['README.txt']=[IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'setup-readme.txt'))
function HashBytes([byte[]]$bytes) {
    $sha=[Security.Cryptography.SHA256]::Create()
    try {return ([BitConverter]::ToString($sha.ComputeHash($bytes))).Replace('-','')}
    finally {$sha.Dispose()}
}
$patch=Join-Path $media 'PATCH';$owned=@{}
if(Test-Path -LiteralPath $patch) {
    if((Get-Item -LiteralPath $patch).Attributes -band [IO.FileAttributes]::ReparsePoint){throw 'PATCH must not be a link'}
    $record=Join-Path $patch 'addon-files.json'
    if(Test-Path -LiteralPath $record) {
        if((Get-Item -LiteralPath $record).Attributes -band [IO.FileAttributes]::ReparsePoint){throw 'PATCH manifest must not be a link'}
        $prior=Get-Content -LiteralPath $record -Raw|ConvertFrom-Json
        if($prior.id -ne 'win101'){throw 'PATCH belongs to a different add-on'}
        foreach($item in $prior.files){$owned[$item.name]=$item.sha256}
    }
}
foreach($name in $files.Keys) {
    $target=Join-Path $patch $name
    if(Test-Path -LiteralPath $target) {
        if((Get-Item -LiteralPath $target).Attributes -band [IO.FileAttributes]::ReparsePoint){throw "Linked PATCH file: $name"}
        $hash=(Get-FileHash -LiteralPath $target).Hash
        if($hash -ne (HashBytes $files[$name]) -and $hash -ne $owned[$name] -and
            (!$baseline.ContainsKey($name) -or $hash -ne (HashBytes $baseline[$name]))) {
            throw "Preserve changed PATCH file: $name"
        }
    }
}
New-Item -ItemType Directory -Path $patch -Force|Out-Null
$records=@(foreach($name in ($files.Keys|Sort-Object)) {
    [IO.File]::WriteAllBytes((Join-Path $patch $name),$files[$name])
    [ordered]@{name=$name;sha256=(HashBytes $files[$name])}
})
[ordered]@{id='win101';releaseDriver=$entry[0];files=$records}|ConvertTo-Json -Depth 5|
    Set-Content -LiteralPath (Join-Path $patch 'addon-files.json') -Encoding UTF8
Write-Host "Prepared $patch; original media remain unchanged."
Write-Host 'Run PATCH\SETUP.CMD. Select Microsoft Mouse (Bus/Serial) in original Setup.'
Write-Host 'After installation, confirm its actual directory and use installed PATCH\win.cmd.'
