param([Parameter(Mandatory)][string]$OriginalImage,
      [Parameter(Mandatory)][string]$Driver,
      [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned fixture root required'
}
$original=(Resolve-Path $OriginalImage).Path;$driverPath=(Resolve-Path $Driver).Path
$imageHash=(Get-FileHash $original).Hash;$driverHash=(Get-FileHash $driverPath).Hash
$installer=Join-Path $repo 'tools/win101-setup/install-mouse101.ps1'
New-Item -ItemType Directory -Path $root | Out-Null
& $installer -OriginalImage $original -Driver $driverPath -OutputRoot "$root/positive"
$proof=Get-Content "$root/positive/installation.json" -Raw | ConvertFrom-Json
if(!$proof.allOtherBytesUnchanged -or $proof.originalSha256 -ne $imageHash -or
   $proof.outputSha256 -ne (Get-FileHash "$root/positive/WIN100.BIN").Hash) {
    throw 'Positive installation identity failed'
}
foreach($case in @('bad-mz','cycle','truncated','imports')) {
    $image=[IO.File]::ReadAllBytes($original);$addon=[IO.File]::ReadAllBytes($driverPath)
    switch($case) {
        'bad-mz' {$image[0]=0}
        'cycle' {
            $ne=[BitConverter]::ToUInt32($image,0x3c)
            [Array]::Copy([BitConverter]::GetBytes([uint16]($ne/16)),0,$image,$ne+8,2)
        }
        'truncated' {$image=[byte[]]$image[0..63]}
        'imports' {
            $ne=[BitConverter]::ToUInt32($addon,0x3c)
            [Array]::Copy([BitConverter]::GetBytes([uint16]1),0,$addon,$ne+0x1e,2)
        }
    }
    [IO.File]::WriteAllBytes("$root/$case.bin",$image)
    [IO.File]::WriteAllBytes("$root/$case.drv",$addon)
    $rejected=$false
    try {& $installer -OriginalImage "$root/$case.bin" -Driver "$root/$case.drv" -OutputRoot "$root/reject-$case"}
    catch {$rejected=$true;$_ | Out-String | Set-Content "$root/reject-$case.txt"}
    if(!$rejected -or (Test-Path "$root/reject-$case/WIN100.BIN")){throw "Unsafe installer accepted $case"}
}
if((Get-FileHash $original).Hash -ne $imageHash -or (Get-FileHash $driverPath).Hash -ne $driverHash) {
    throw 'Immutable installer inputs changed'
}
'PASS localized installation, unchanged outside bytes, four malformed-input refusals and immutable inputs'
