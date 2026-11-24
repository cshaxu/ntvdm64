param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot,$repo)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $root|Out-Null
foreach($arch in 'x86','x64'){
 & "$repo/tools/build-fixtures.cmd" $arch native-hook-packet *> "$root/$arch-build.log"
 if($LASTEXITCODE){throw "Packet build failed: $arch"}
 & "$repo/build/.ninja/fixtures-$arch/native-hook-packet.exe" *> "$root/$arch.txt"
 if($LASTEXITCODE){throw "Packet contract failed: $arch"}
 Get-Content "$root/$arch.txt"
}
'PASS native-hook packet unit on x86 and x64'
