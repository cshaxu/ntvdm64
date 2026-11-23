$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$started=[DateTimeOffset]::UtcNow.ToString('o');$code=1
try {
 foreach($name in 'base-classifier-path','base-resource','base-vdm-config') {
  & "test/observation/verify-$name.ps1" -BuildRoot "$PSScriptRoot/$name"
 }
 $code=0
} catch {$_|Out-String|Write-Output}
@{pid=$PID;started=$started;finished=[DateTimeOffset]::UtcNow.ToString('o');exit=$code}|ConvertTo-Json|Set-Content "$PSScriptRoot/contracts-result.json"
exit $code