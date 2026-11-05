[CmdletBinding()]
param([Parameter(Mandatory)][string]$LauncherBuildRoot)
$ErrorActionPreference='Stop'
$graph=Get-Content (Join-Path $LauncherBuildRoot 'build.ninja') -Raw
if($graph -notmatch 'src/common/native-hook/context\.c' -or
   $graph -match 'src/nthook32-dll/context\.cpp') {
    throw 'Launcher context provider is not common-owned'
}
$line=[regex]::Match($graph,'(?m)^build run16\.exe[^\r\n]*')
if(!$line.Success -or $line.Value -notmatch 'common-native-hook-context\.lib'){
    throw 'Launcher does not select the shared context archive'
}
$deps=@(& ninja -C $LauncherBuildRoot -t deps obj/run16/frontend_scope.obj 2>&1)
if($LASTEXITCODE -or ($deps -join "`n") -notmatch '#deps \d+.*\(VALID\)'){
    throw 'Missing launcher compiler dependencies'
}
$text=($deps -join "`n").Replace('\','/')
function Assert-Boundary([string]$value){
    if($value -notmatch 'src/common/native-hook/context\.h' -or
       $value -match 'src/nthook32-dll/hook\.h'){throw 'Private NTHook context reached launcher'}
}
Assert-Boundary $text
$rejected=$false
try{Assert-Boundary ($text+"`nsrc/nthook32-dll/hook.h")}catch{$rejected=$true}
if(!$rejected){throw 'Private-header negative control accepted'}
Write-Output 'PASS launcher shared context ownership and private-header negative control'
