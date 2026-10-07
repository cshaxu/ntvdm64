param([Parameter(Mandatory)][string]$InstallRoot,
      [Parameter(Mandatory)][string]$Driver,
      [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned output required'
}
$install=(Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
$driverPath=(Resolve-Path -LiteralPath $Driver).Path
if(!$driverPath.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Authored build-owned driver required'
}
$profile=Join-Path $install 'PATCH'
$ini=Join-Path $install 'SYSTEM.INI'
$before=Get-Content -LiteralPath $ini -Raw
$boot=[regex]::Match($before,'(?ims)^\[boot\][^\r\n]*\r?\n(?<body>.*?)(?=^\[|\z)')
if(!$boot.Success -or [regex]::Matches($boot.Groups['body'].Value,'(?im)^mouse\.drv=[^\r\n]*').Count -ne 1) {
    throw 'Ambiguous mouse selection'
}
New-Item -ItemType Directory -Path $root|Out-Null
Copy-Item -LiteralPath $ini -Destination "$root/SYSTEM-before.INI"
Copy-Item -LiteralPath $driverPath -Destination "$root/MOUSE31.DRV"
$body=[regex]::Replace($boot.Groups['body'].Value,'(?im)^mouse\.drv=[^\r\n]*',
    ('mouse.drv='+$profile+'\MOUSE31.DRV'))
$start=$boot.Groups['body'].Index;$length=$boot.Groups['body'].Length
$after=$before.Substring(0,$start)+$body+$before.Substring($start+$length)
[IO.File]::WriteAllText("$root/SYSTEM.INI",$after,[Text.Encoding]::ASCII)
foreach($mode in @('Standard','Enhanced')) {
    & "$repo/src/addon/win31-launch/configure-launch.ps1" -InstallRoot $install -OutputDirectory "$root/$mode" -Mode $mode
}
[ordered]@{role='recoverable-installation-driver-selection-not-runtime-pass';
    installation=$install;originalIniSha256=(Get-FileHash $ini).Hash;
    driverSha256=(Get-FileHash $driverPath).Hash;
    targetDriver="$profile\MOUSE31.DRV";originalRetailMouseSha256=(Get-FileHash "$install/SYSTEM/MOUSE.DRV").Hash
} |ConvertTo-Json|Set-Content "$root/preparation.json"
'PASS staged test-only driver selection, recoverable INI and separate /S /3 PIFs; nothing deployed'
