[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,
    [Parameter(Mandatory)][string]$RuntimeRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot,$repo)
$runtime=(Resolve-Path $RuntimeRoot).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $out)){throw 'Require a fresh evidence directory under build/.'}
foreach($file in 'system32/NTIO.SYS','system.ini'){
    if(!(Test-Path "$runtime/$file" -PathType Leaf)){throw "Missing runtime input: $file"}
}
$null=New-Item -ItemType Directory $out
$results=[Collections.Generic.List[object]]::new()
foreach($architecture in 'x86','x64'){
    & "$repo/tools/build-fixtures.cmd" $architecture package-resources 2>&1 |
        Tee-Object "$out/$architecture-build.log"
    if($LASTEXITCODE){throw "Build failed: $architecture"}
    $directory="$out/$architecture"
    $null=New-Item -ItemType Directory "$directory/system32","$directory/flat/system32"
    Copy-Item "$runtime/system32/NTIO.SYS" "$directory/system32/NTIO.SYS"
    Copy-Item "$runtime/system32/NTIO.SYS" "$directory/flat/NTIO.SYS"
    Copy-Item "$runtime/system.ini" "$directory/system.ini"
    foreach($name in 'package-layout-fixture','softpc-firmware-resource-fixture',
        'softpc-wow-boot-environment-fixture','softpc-media-resource-fixture'){
        $exe="$directory/system32/$name.exe"
        Copy-Item "$repo/build/.ninja/fixtures-$architecture/$name.exe" $exe
        $arguments=@()
        if($name -eq 'softpc-media-resource-fixture'){$arguments=@($directory)}
        $output=& $exe @arguments 2>&1
        $code=$LASTEXITCODE
        $results.Add([pscustomobject]@{architecture=$architecture;name=$name;
            exit=$code;sha256=(Get-FileHash $exe).Hash;output=($output -join "`n")})
        if($code){throw "Failed: $architecture/$name ($code)"}
    }
    Copy-Item "$directory/system32/softpc-media-resource-fixture.exe" "$directory/flat/system32/softpc-media-resource-fixture.exe"
    & "$directory/flat/system32/softpc-media-resource-fixture.exe" "$directory/flat" --flat-copy-only
    if($LASTEXITCODE){throw "Flat media rejection failed: $architecture ($LASTEXITCODE)"}
    $results.Add([pscustomobject]@{architecture=$architecture;name='flat-media-rejection';exit=0})
}
[pscustomobject]@{exit=0;cases=$results;inputs=@('system32/NTIO.SYS','system.ini'|
    ForEach-Object {[pscustomobject]@{path=$_;sha256=(Get-FileHash "$runtime/$_").Hash}})} |
    ConvertTo-Json -Depth 5 | Set-Content "$out/result.json"
Write-Host 'Package resources passed: ten x86/x64 runtime cases.'
