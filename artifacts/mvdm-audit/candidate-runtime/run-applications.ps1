$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$hostExe=(Get-Process -Id $PID).Path
$phase=Join-Path $repo 'build/t473-s10/candidate-validation'
$package=Join-Path $phase 'package'
$compiler=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
$observer=Join-Path $repo 'build/.ninja/console-observers-x64/console-startup-observer.exe'
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS')
Get-ChildItem Env: | Where-Object {$_.Name -notin $keep} | ForEach-Object {Remove-Item -LiteralPath ('Env:'+$_.Name)}
$env:PATH='C:\Windows\System32;C:\Windows'
$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
$cases=@(
    @{Name='mine';Script='verify-wow-mine-gameplay.ps1';Observer=$true},
    @{Name='sol';Script='verify-wow-sol-gameplay.ps1';Observer=$true},
    @{Name='sol-options';Script='verify-wow-sol-options.ps1';Observer=$false},
    @{Name='write';Script='verify-wow-write-lifecycle.ps1';Observer=$false}
)
$results=@()
foreach($case in $cases){
    $script=Join-Path $repo ('tests/observation/'+$case.Script)
    $output=Join-Path $phase ($case.Name+'-r1')
    if(Test-Path -LiteralPath $output){throw "Evidence already exists: $output"}
    $arguments=@('-NoProfile','-ExecutionPolicy','Bypass','-File',$script,'-PackageRoot',$package,'-CompilerDriver',$compiler,'-BuildRoot',$output)
    if($case.Observer){$arguments+=@('-Observer',$observer)}
    $stdout=Join-Path $phase ($case.Name+'.stdout.txt')
    $stderr=Join-Path $phase ($case.Name+'.stderr.txt')
    $started=[DateTime]::UtcNow.ToString('o')
    $child=Start-Process -FilePath $hostExe -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    $record=[ordered]@{name=$case.Name;pid=$child.Id;started=$started;command=($hostExe+' '+($arguments -join ' '));stdout=$stdout;stderr=$stderr;script_sha256=(Get-FileHash -LiteralPath $script).Hash;package_manifest_sha256=(Get-FileHash -LiteralPath (Join-Path $package 'product-system32-manifest.json')).Hash;exit=$null}
    $owner=Join-Path $phase ($case.Name+'-owner.json')
    $record|ConvertTo-Json -Depth 5|Set-Content -LiteralPath $owner
    $child.WaitForExit();$child.Refresh()
    $record.exit=$child.ExitCode;$record.finished=[DateTime]::UtcNow.ToString('o')
    $record|ConvertTo-Json -Depth 5|Set-Content -LiteralPath $owner
    $results+=,$record
    $results|ConvertTo-Json -Depth 5|Set-Content -LiteralPath (Join-Path $phase 'application-results.json')
    if($child.ExitCode -ne 0){throw "Application case failed: $($case.Name), exit $($child.ExitCode)"}
}
