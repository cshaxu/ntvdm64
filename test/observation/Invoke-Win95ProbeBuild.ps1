param([Parameter(Mandatory)][string]$OutputRoot,
      [Parameter(Mandatory)][ValidateSet('loader','menu','dos_restart')][string]$Kind)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh repository build output required'}
New-Item -ItemType Directory $out|Out-Null
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 win95-probes"'
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/build.stdout" -RedirectStandardError "$out/build.stderr"
try {
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$out/build.owner.json"
    if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'Win95 probe build deadline'}
    $p.WaitForExit();$p.ExitCode|Set-Content "$out/build.exit"
    if($p.ExitCode){throw 'Native Win95 probe build failed'}
} finally {$p.Dispose()}
$image=if($Kind -eq 'dos_restart'){'DOSRET'}else{'MENU95'}
$cache="$repo/build/.ninja/fixtures-x86/win95-$Kind"
Copy-Item "$cache/$image.EXE","$cache/$image.MAP" $out
$path="$out/$image.EXE";$bytes=[IO.File]::ReadAllBytes($path)
$pe=[BitConverter]::ToInt32($bytes,60);$opt=$pe+24
if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'Expected x86 test image'}
foreach($at in @(($opt+40),($opt+48))){
    if([BitConverter]::ToUInt16($bytes,$at) -ne 4 -or [BitConverter]::ToUInt16($bytes,$at+2) -ne 0){throw 'Wrong Win95 version metadata'}
}
@{sha256=(Get-FileHash $path).Hash;source=(Get-FileHash "$PSScriptRoot/win95_${Kind}_probe.c").Hash;role="independent Win95 $Kind diagnostic"}|ConvertTo-Json|Set-Content "$out/build.json"
'PASS native Win95 probe production and PE version metadata; runtime not exercised'
