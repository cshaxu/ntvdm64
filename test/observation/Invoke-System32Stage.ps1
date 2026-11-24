# Assertion-runner adapter only; CMD owns all package production.
function Invoke-System32Stage([hashtable]$Arguments) {
 $repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
 $argsList=@()
 foreach($name in $Arguments.Keys){if([string]$Arguments[$name] -ne ''){$argsList+=('-'+$name);$argsList+=[string]$Arguments[$name]}}
 $output=& "$repo/tools/stage-system32.cmd" @argsList 2>&1
 if($LASTEXITCODE){throw ($output|Out-String)}
 $output|Write-Output
}
