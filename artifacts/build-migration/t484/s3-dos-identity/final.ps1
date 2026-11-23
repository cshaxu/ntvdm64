$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$begin=[DateTimeOffset]::UtcNow.ToString('o');$code=1
try {
 $cache='build/.ninja/dos-identity'
 $before=@{}
 foreach($name in 'controller.exe','A.COM','B.COM'){$before[$name]=@{hash=(Get-FileHash "$cache/$name").Hash;time=(Get-Item "$cache/$name").LastWriteTimeUtc.Ticks}}
 & cmd.exe /d /c tools\build-dos-identity.cmd "$PSScriptRoot/cache-copy" 'O:\tmp\identity copy'
 if($LASTEXITCODE){throw 'Native cache invocation failed'}
 foreach($name in $before.Keys){if((Get-FileHash "$cache/$name").Hash -ne $before[$name].hash -or (Get-Item "$cache/$name").LastWriteTimeUtc.Ticks -ne $before[$name].time){throw 'Cache output changed'}}
 & cmd.exe /d /c tools\build-dos-identity.cmd "$PSScriptRoot/cache-copy" 'O:\tmp\identity copy'
 if($LASTEXITCODE -ne 2){throw 'Existing output accepted'}
 & cmd.exe /d /c tools\build-dos-identity.cmd 'O:\outside-identity-fixture' 'O:\tmp\identity copy'
 if($LASTEXITCODE -ne 2){throw 'Outside repository build accepted'}
 & cmd.exe /d /c tools\build-dos-identity.cmd
 if($LASTEXITCODE -ne 2){throw 'Missing arguments accepted'}
 $tokens=$null;$errors=$null
 [void][Management.Automation.Language.Parser]::ParseFile("$pwd/test/integration/verify-dos-identity-pair.ps1",[ref]$tokens,[ref]$errors)
 if($errors){throw ($errors|Out-String)}
 if((Get-Content test/integration/verify-dos-identity-pair.ps1 -Raw) -match '& nasm|prepare-dos-identity-probe.py'){throw 'Retired producer reference'}
 $source=Get-Content build/t484-s3-addon/deliver.ps1 -Raw
 $a=$source.IndexOf(' $rows=Get-Content');$b=$source.IndexOf(' $args=@{}')
 & ([scriptblock]::Create($source.Substring($a,$b-$a)))
 & tools/governance/Verify-DocumentationGovernance.ps1
 if($LASTEXITCODE){throw 'Governance failed'}
 & git diff --check
 if($LASTEXITCODE){throw 'Whitespace failed'}
 $code=0
} catch {$_|Out-String|Write-Output}
@{pid=$PID;started=$begin;finished=[DateTimeOffset]::UtcNow.ToString('o');exit=$code}|ConvertTo-Json|Set-Content "$PSScriptRoot/final-result.json"
exit $code