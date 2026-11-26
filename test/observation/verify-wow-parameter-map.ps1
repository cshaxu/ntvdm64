[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
# Exact original algorithms plus scoped service fault doubles; not a guest pass.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh disposable evidence required'}
$null=New-Item -ItemType Directory $root
$steps=@()
function Run($name,$file,$arguments,$expected,$seconds){
 $options=@{FilePath=$file;WorkingDirectory=$root;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$root/$name.stdout.log";RedirectStandardError="$root/$name.stderr.log"};if($arguments.Count){$options.ArgumentList=$arguments}
 $p=Start-Process @options;$null=$p.Handle;$row=[ordered]@{name=$name;pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');file=$file;arguments=$arguments;deadlineSeconds=$seconds};$script:steps+=,$row;$script:steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json"
 if(!$p.WaitForExit($seconds*1000)){$p.Kill($true);throw "$name deadline"};$p.Refresh();$row.exit=$p.ExitCode;$row.end=[DateTime]::UtcNow.ToString('o');$script:steps|ConvertTo-Json -Depth 5|Set-Content "$root/steps.json";if($p.ExitCode -ne $expected){throw "$name actual $($p.ExitCode), expected $expected"}
}
try{
 $paths=@('src/mvdm-host/inc/wow.h','src/mvdm-host/inc/wowusr.h','src/mvdm-host/inc/wowcmpat.h','src/mvdm-host/wow32/walias.h','src/mvdm-host/wow32/wmdisp32.h','src/mvdm-host/wow32/wparam.h','src/mvdm-host/wow32/wcall16.h','src/non-mvdm/public/internal/windows/inc/wowuserp.h','src/mvdm-host/wow32/wparam.c','src/mvdm-host/wow32/wmsg16.c','src/mvdm-host/wow32/wmsgcb.c','src/mvdm-host/wow32/wmdisp32.c','src/mvdm-host/wow32/wcntl32.c','src/mvdm-host/wow32/wuwind.c','src/compat/include/wow_message_lifetime.h','src/compat/include/wow_message_progress.h','test/observation/wow_parameter_map_fixture.c','test/observation/verify-wow-parameter-map.ps1')
 $paths|ForEach-Object{Get-FileHash "$repo/$_"}|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Get-FileHash "$repo/src/compat/include/wow_control_message_abi.h"|Export-Csv -NoTypeInformation "$root/control-abi-input.csv"
 & "$repo/tools/build-fixtures.cmd" x86 wow-parameter-map *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native parameter map build failed'}
 $native="$repo/build/.ninja/fixtures-x86"
 Copy-Item "$native/wow-parameter-map.exe" "$root/unit.exe"
 Run body "$root/unit.exe" @() 0 30
 if((Get-Content "$root/body.stdout.log" -Raw) -notmatch 'PARAMETER_MAP failures=0 scenarios=\d+ reverse=148 buffer=84 forward=72'){throw 'Incomplete current result/layout'}
 Get-FileHash "$native/parameter-map/parameter_types.h","$native/parameter-map/parameter_map_records.h","$native/parameter-map/parameter_cache.h","$native/parameter-map/parameter_exact.h","$root/unit.exe"|Export-Csv -NoTypeInformation "$root/projections.csv"
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Input changed during cohort'}}
 foreach($row in Import-Csv "$root/control-abi-input.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Control ABI input changed'}}

 @{passed=$true;scope='selected original release cache and exact extracted bodies with actual finalizer; lower services doubled, not actual guest';end=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$root/result.json"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
