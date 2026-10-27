$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$hostExe=(Get-Process -Id $PID).Path
$phase=Join-Path $repo 'build/t473-s10/candidate-validation'
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS')
Get-ChildItem Env:|Where-Object{$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+$_.Name)}
$env:PATH='C:\Windows\System32;C:\Windows';$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
$cases=@(
 @{Name='diagnostic-options-r2';Script='verify-wow-sol-options.ps1';Package='build/t473-s10/candidate-validation/creation-diagnostic-package'},
 @{Name='diagnostic-options-r3';Script='verify-wow-sol-options.ps1';Package='build/t473-s10/candidate-validation/creation-diagnostic-package'}
)
$records=@()
foreach($case in $cases){
 $script=Join-Path $repo ('tests/observation/'+$case.Script)
 $package=Join-Path $repo $case.Package
 $output=Join-Path $phase $case.Name
 if(Test-Path -LiteralPath $output){throw 'Fresh evidence directory required'}
 $arguments=@('-NoProfile','-ExecutionPolicy','Bypass','-File',$script,'-PackageRoot',$package,'-CompilerDriver',(Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'),'-BuildRoot',$output)
 $stdout=Join-Path $phase ($case.Name+'.stdout.txt');$stderr=Join-Path $phase ($case.Name+'.stderr.txt')
 $started=[DateTime]::UtcNow.ToString('o')
 $child=Start-Process -FilePath $hostExe -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
 $record=[ordered]@{name=$case.Name;pid=$child.Id;started=$started;command=($hostExe+' '+($arguments -join ' '));stdout=$stdout;stderr=$stderr;script_sha256=(Get-FileHash $script).Hash;package_manifest_sha256=(Get-FileHash (Join-Path $package 'product-system32-manifest.json')).Hash;exit=$null}
 $owner=Join-Path $phase ($case.Name+'-owner.json');$record|ConvertTo-Json|Set-Content $owner
 $child.WaitForExit();$child.Refresh();$record.exit=$child.ExitCode;$record.finished=[DateTime]::UtcNow.ToString('o');$record|ConvertTo-Json|Set-Content $owner
 $records+=,$record;$records|ConvertTo-Json -Depth 5|Set-Content (Join-Path $phase 'diagnostic-series-results.json')
}
if(@($records|Where-Object{$_.exit -ne 0}).Count){exit 1}
