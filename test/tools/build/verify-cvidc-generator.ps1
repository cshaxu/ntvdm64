param([Parameter(Mandatory)][string]$Generator,[Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../../..")
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){
    throw 'Require a fresh build-owned test directory'
}
$tool=(Resolve-Path $Generator).Path
$base=Join-Path $repo 'src/mvdm-host/softpc.new/base'
New-Item -ItemType Directory $out|Out-Null
$header=Join-Path $out 'generated/cvidc_cpu_binding.inc'
$expected=[ordered]@{
    'cvidc_cpu_binding.inc'='F6F480D0BFF8A1CAB2C495506C831E69BF17276D82775F4037F435578C7D5E7E'
    'cvidc_selected_providers.inc'='E088FA7905FC642FFA2B2726A5D2301C2869335D7E3149FBBFDF42B52E836B67'
    'cvidc_accessor_cases.inc'='F69B1244CD763084239D4775C76A6D5414009DDAC7B1AD2C02BF926616CF3A49'
    'v7_selected_plane.h'='4B29B95AE0C805B0383A73F42306E60CBE16177B7F3D46A67BDC48B50BC0F856'
}
& $tool $header $base
if($LASTEXITCODE){throw 'Original metadata generation failed'}
$stamp=([datetime]'2001-01-01T00:00:00Z').ToUniversalTime()
foreach($name in $expected.Keys){
    $path=Join-Path $out "generated/$name"
    if((Get-FileHash $path).Hash -ne $expected[$name]){throw "Production projection changed: $name"}
    [IO.File]::SetLastWriteTimeUtc($path,$stamp)
}
& $tool $header $base
if($LASTEXITCODE){throw 'Unchanged generation failed'}
foreach($name in $expected.Keys){
    if([IO.File]::GetLastWriteTimeUtc((Join-Path $out "generated/$name")) -ne $stamp){throw "Cache invalidation: $name"}
}
# Mutate a test-owned metadata copy, never the original mirror.
$inputs=@('cvidc/c2cpusad.h','cvidc/cpu4gen.h','cvidc/evidgen.h','inc/sas.h',
    'cpu/src/evid/vglob.c','cvidc/sinit011.c','cvidc/sinit012.c','cvidc/sinit013.c','inc/egaports.h')
$copy=Join-Path $out 'source/src/mvdm-host/softpc.new/base'
foreach($relative in $inputs){
    $dest=Join-Path $copy $relative
    New-Item -ItemType Directory -Force (Split-Path $dest)|Out-Null
    Copy-Item (Join-Path $base $relative) $dest
}
$sas=Join-Path $out 'source/src/compat/include/generated/x86/prod/sas4gen.h'
New-Item -ItemType Directory -Force (Split-Path $sas)|Out-Null
Copy-Item "$repo/src/compat/include/generated/x86/prod/sas4gen.h" $sas
$cpu=Join-Path $copy 'cvidc/cpu4gen.h'
$text=[IO.File]::ReadAllText($cpu)
$marker=[regex]::Match($text,'(?m)^\s*IMPORT[^\r\n;]*\bc_[A-Za-z0-9_]+[^\r\n;]*;')
if(!$marker.Success){throw 'Missing negative-test declaration'}
[IO.File]::WriteAllText($cpu,$text.Remove($marker.Index,$marker.Length))
& $tool $header $copy 2>&1 | ForEach-Object { "negative: $_" }
if(!$LASTEXITCODE){throw 'Changed CPU declaration count accepted'}
foreach($name in $expected.Keys){
    $path=Join-Path $out "generated/$name"
    if((Get-FileHash $path).Hash -ne $expected[$name] -or [IO.File]::GetLastWriteTimeUtc($path) -ne $stamp){
        throw "Failed generation changed prior output: $name"
    }
}
[ordered]@{generatorSha256=(Get-FileHash $tool).Hash;outputs=$expected;
    checks=@('four-exact-production-projections','four-unchanged-mtimes','changed-metadata-rejected','four-prior-outputs-preserved')}|
    ConvertTo-Json -Depth 4|Set-Content "$out/result.json"
'PASS C-VID generation: four byte-identical outputs, stable cache and rejected changed metadata'
