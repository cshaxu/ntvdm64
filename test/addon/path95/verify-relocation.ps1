[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
$source=(Resolve-Path $InstallSource).Path.TrimEnd('\')
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out | Out-Null
$tool="$repo/assets/release/PATH95.EXE"
$target='Z:'+('\'+('q'*($source.Length-3)))
foreach($name in 'SYSTEM.DAT','USER.DAT'){
 $inputFile=Join-Path $source $name
 $hash=(Get-FileHash $inputFile).Hash
 $candidate=Join-Path $out $name
 & $tool replace-equal $inputFile $candidate $source $target *> "$candidate.log"
 if($LASTEXITCODE -or !(Test-Path $candidate)){throw "Relocation failed: $name"}
 if((Get-Item $candidate).Length -ne (Get-Item -Force $inputFile).Length){throw 'Length changed'}
 if((Get-FileHash $candidate).Hash -eq $hash){throw 'Fixture has no matching paths'}
 # Replacement is case-insensitive and writes the requested root casing.
 & $tool replace-equal $inputFile "$candidate.canonical" $source $source *> "$candidate.canonical.log"
 if($LASTEXITCODE){throw 'Canonical comparison failed'}
 & $tool replace-equal $candidate "$candidate.reverse" $target $source *> "$candidate.reverse.log"
 if($LASTEXITCODE -or (Get-FileHash "$candidate.reverse").Hash -ne (Get-FileHash "$candidate.canonical").Hash){throw 'Reverse differs outside relocated values/checksums'}
 & $tool replace $inputFile "$candidate.noop" 'Z:\not-a-root' 'Z:\replacement' *> "$candidate.noop.log"
 if($LASTEXITCODE -or (Get-FileHash "$candidate.noop").Hash -ne $hash){throw 'Default no-op changed bytes'}
 $candidateHash=(Get-FileHash $candidate).Hash
 & $tool replace-equal $inputFile $candidate $source $target *> "$candidate.existing.log"
 if(!$LASTEXITCODE -or (Get-FileHash $candidate).Hash -ne $candidateHash){throw 'Existing output was accepted/changed'}
 & $tool replace-equal $inputFile "$candidate.unequal" $source ($target+'x') *> "$candidate.unequal.log"
 if(!$LASTEXITCODE -or (Test-Path "$candidate.unequal")){throw 'Unequal roots accepted'}
 $bytes=[IO.File]::ReadAllBytes($inputFile)
 [IO.File]::WriteAllBytes("$candidate.truncated",$bytes[0..47])
 & $tool replace-equal "$candidate.truncated" "$candidate.invalid" $source $target *> "$candidate.invalid.log"
 if(!$LASTEXITCODE -or (Test-Path "$candidate.invalid")){throw 'Truncated hive accepted'}
 if((Get-FileHash $inputFile).Hash -ne $hash){throw 'Source changed'}
}
'PASS equal relocation, canonical reverse, default no-op, existing output, unequal paths, truncated input and source identity' | Set-Content "$out/result.txt"
$global:LASTEXITCODE=0
