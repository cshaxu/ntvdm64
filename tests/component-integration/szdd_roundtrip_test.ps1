param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned fixture required'}
New-Item -ItemType Directory -Path $root|Out-Null
$tool=Join-Path $repo 'assets/release/SZDD.EXE'
foreach($length in @(0,1,7,8,9,1025)){
    $inputFile=Join-Path $root "$length.bin";$packed=Join-Path $root "$length.bi_";$expanded=Join-Path $root "$length.out"
    $bytes=[byte[]]::new($length)
    for($i=0;$i -lt $length;++$i){$bytes[$i]=($i*37)%256}
    [IO.File]::WriteAllBytes($inputFile,$bytes)
    & $tool $inputFile $packed
    if($LASTEXITCODE){throw "SZDD failed for $length bytes"}
    & expand.exe $packed $expanded|Out-Null
    if($LASTEXITCODE -or (Get-FileHash $inputFile).Hash -ne (Get-FileHash $expanded).Hash){throw "SZDD roundtrip failed for $length bytes"}
    $before=(Get-FileHash $packed).Hash
    & $tool $inputFile $packed 2>$null
    if(!$LASTEXITCODE -or (Get-FileHash $packed).Hash -ne $before){throw 'Existing output was overwritten'}
}
$missing=Join-Path $root 'missing.bi_'
& $tool (Join-Path $root 'absent.bin') $missing 2>$null
if(!$LASTEXITCODE -or (Test-Path $missing)){throw 'Missing input produced an output'}
$global:LASTEXITCODE=0
'PASS SZDD exact expansion, literal-block boundaries, empty input and failure preservation'
