[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
# Run as an owned hidden background job. Exact original source is selected;
# allocation/memory doubles supplement, never replace, real guest acceptance.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh disposable root required'}
$null=New-Item -ItemType Directory $root
$steps=@()
function Run($name,$file,$Arguments,$expected,$seconds){
 $options=@{FilePath=$file;WorkingDirectory=$root;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$root/$name.stdout.log";RedirectStandardError="$root/$name.stderr.log"};if($Arguments.Count){$options.ArgumentList=$Arguments}
 $p=Start-Process @options;$null=$p.Handle
 $row=[ordered]@{name=$name;pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');file=$file;arguments=$Arguments;deadlineSeconds=$seconds}
 $script:steps+=,$row;$script:steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if(!$p.WaitForExit($seconds*1000)){$p.Kill($true);throw "$name deadline"};$p.Refresh();$row.exit=$p.ExitCode;$row.end=[DateTime]::UtcNow.ToString('o');$script:steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if($p.ExitCode -ne $expected){throw "$name actual $($p.ExitCode), expected $expected"}
}
try{
 $paths=@('src/mvdm-host/inc/wow.h','src/mvdm-host/wow32/walias.h','src/mvdm-host/wow32/wstruc.h','src/mvdm-host/wow32/wstruc.c','src/mvdm-host/wow32/wmsgem.c','src/mvdm-host/wow32/wmsglb.c','src/mvdm-host/wow32/wmsg16.c','src/non-mvdm/windows/core/ntuser/inc/user.h','src/non-mvdm/windows/core/ntuser/client/lboxrare.c','test/observation/wow_reverse_construction_fixture.c','test/observation/verify-wow-reverse-construction.ps1')
 $paths|ForEach-Object{Get-FileHash "$repo/$_"}|Export-Csv -NoTypeInformation "$root/inputs.csv"
 & "$repo/tools/build-fixtures.cmd" x86 wow-reverse-construction *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native reverse construction build failed'}
 $native="$repo/build/.ninja/fixtures-x86"
 Copy-Item "$native/wow-reverse-construction.exe" "$root/unit.exe"
 Get-FileHash "$root/unit.exe","$native/reverse_types.h","$native/reverse_exact.h"|Export-Csv -NoTypeInformation "$root/artifacts.csv"
 Run 'body-candidate' "$root/unit.exe" @() 0 30
 if((Get-Content "$root/body-candidate.stdout.log" -Raw) -notmatch 'REVERSE_CONSTRUCTION failures=0 scenarios=21 buffer=\d+ record=\d+'){throw 'Incomplete candidate'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Source changed'}}
 @{passed=$true;originalHelp='allocation failure preserves successful NULL conversion and empty cleanup';scope='exact-source service doubles, not runtime acceptance';end=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$root/result.json"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
