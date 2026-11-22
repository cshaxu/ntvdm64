param(
    [Parameter(Mandatory)][string]$Generator,
    [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../../..")
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path -LiteralPath $out)){throw 'Require a fresh build-owned test directory'}
$tool=(Resolve-Path -LiteralPath $Generator).Path
$rom=Join-Path $repo 'src/mvdm-guest/softpc.new/roms/v7vga.rom'
$sourceHash=(Get-FileHash -LiteralPath $rom).Hash
New-Item -ItemType Directory -Path $out|Out-Null
$header=Join-Path $out 'native_pc_font.h'
$cases=[Collections.Generic.List[string]]::new()
& $tool $rom $header
if($LASTEXITCODE){throw 'Pinned ROM generation failed'}
# Independently retained identity of the existing production-generated header.
$expected='B295271C7D37D9D409B77FB9E01D429FBA8091F428AD7746A6A5073963790D83'
if((Get-FileHash -LiteralPath $header).Hash -ne $expected){throw 'Generated header differs from production baseline'}
$cases.Add('exact-production-font-header')
$stamp=([datetime]'2001-01-01T00:00:00Z').ToUniversalTime()
[IO.File]::SetLastWriteTimeUtc($header,$stamp)
& $tool $rom $header
if($LASTEXITCODE -or [IO.File]::GetLastWriteTimeUtc($header) -ne $stamp){throw 'Unchanged generation invalidated cache input'}
$cases.Add('unchanged-output-preserves-mtime')
$bad=Join-Path $out 'wrong.rom'
$bytes=[IO.File]::ReadAllBytes($rom)
$bytes[0]=$bytes[0] -bxor 1
[IO.File]::WriteAllBytes($bad,$bytes)
& $tool $bad $header 2>&1 | ForEach-Object { "negative: $_" }
if(!$LASTEXITCODE -or (Get-FileHash $header).Hash -ne $expected -or
   [IO.File]::GetLastWriteTimeUtc($header) -ne $stamp){throw 'Wrong ROM was accepted or changed existing output'}
$cases.Add('wrong-rom-preserves-output')
[IO.File]::WriteAllBytes($bad,[byte[]]@(1,2,3))
& $tool $bad $header 2>&1 | ForEach-Object { "negative: $_" }
if(!$LASTEXITCODE -or (Get-FileHash $header).Hash -ne $expected){throw 'Short ROM was accepted or changed output'}
$cases.Add('truncated-rom-preserves-output')
& $tool $rom (Join-Path $out 'missing/child.h') 2>&1 | ForEach-Object { "negative: $_" }
if(!$LASTEXITCODE){throw 'Missing destination directory reported success'}
$cases.Add('output-error-propagates')
if((Get-FileHash $rom).Hash -ne $sourceHash){throw 'Original ROM changed'}
if(Test-Path ($header+'.tmp')){throw 'Temporary output leaked'}
$cases.Add('immutable-rom-and-no-temporary-residue')
[ordered]@{cases=@($cases);count=$cases.Count;generatorSha256=(Get-FileHash $tool).Hash;
    romSha256=$sourceHash;headerSha256=$expected}|ConvertTo-Json|Set-Content "$out/result.json"
"PASS font generator: $($cases.Count) cases"
