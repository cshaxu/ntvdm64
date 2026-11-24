param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 wow-controlled-boundaries *> "$out/build.log"
if($LASTEXITCODE){throw 'Boundary fixtures failed to build'}
foreach($case in @(@('clipboard','--expect-fixed','cases=24 strict=1 failures=0'),@('keyboard','--controlled','cases=7 failures=0'))){
 $name=$case[0]
 & "$repo/build/.ninja/fixtures-x86/wow-boundary-$name.exe" $case[1] *> "$out/$name.txt"
 if($LASTEXITCODE){throw "Boundary failed: $name"}
 $text=Get-Content "$out/$name.txt" -Raw
 if(!$text.Contains($case[2])){throw "Missing assertion receipt: $name"}
 $text
}
Get-FileHash "$repo/src/mvdm-host/wow32/wuclip.c","$repo/src/mvdm-host/wow32/wkbman.c",
 "$repo/build/.ninja/fixtures-x86/selected-set-clipboard.inc","$repo/build/.ninja/fixtures-x86/selected-keyboard.inc" |
 Export-Csv -NoTypeInformation "$out/inputs.csv"
