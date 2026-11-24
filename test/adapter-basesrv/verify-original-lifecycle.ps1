param([Parameter(Mandatory)][string]$Fixture,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$fixturePath=(Resolve-Path $Fixture).Path
$case=[IO.Path]::GetFullPath($BuildRoot)
if(!$case.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Require evidence directory under build'}
New-Item -ItemType Directory -Force "$case/system32"|Out-Null
$media=@{
 'MEM.EXE'='dos/v86/cmd/mem/mem.exe'
 'system32/WRITE.EXE'='wow16/write/WRITE.EXE'
 'system32/KRNL386.exe'='wow16/kernel31/retail3/krnl386.exe'
}
$identities=@()
foreach($entry in $media.GetEnumerator()){
 $source=Join-Path "$repo/src/mvdm-guest" $entry.Value
 $target=Join-Path $case $entry.Key
 Copy-Item -LiteralPath $source -Destination $target
 $hash=(Get-FileHash $source).Hash
 if((Get-FileHash $target).Hash -ne $hash){throw 'Media identity mismatch'}
 $identities+=@{source=$entry.Value;target=$entry.Key;sha256=$hash}
}
$p=Start-Process $fixturePath -WorkingDirectory $case -WindowStyle Hidden -PassThru -RedirectStandardOutput "$case/stdout.txt" -RedirectStandardError "$case/stderr.txt"
if(!$p.WaitForExit(30000)){$p.Kill();throw 'Original lifecycle timed out'}
@{fixture=$fixturePath;sha256=(Get-FileHash $fixturePath).Hash;pid=$p.Id;exit=$p.ExitCode;media=$identities}|ConvertTo-Json -Depth 5|Set-Content "$case/result.json"
Get-Content "$case/stdout.txt"
if($p.ExitCode){Get-Content "$case/stderr.txt";throw "Original lifecycle failed: $($p.ExitCode)"}
