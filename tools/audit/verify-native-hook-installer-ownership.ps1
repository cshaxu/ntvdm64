[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerX86BuildRoot,
      [Parameter(Mandatory)][string]$WorkerX64BuildRoot)
$ErrorActionPreference='Stop'
function Assert-Boundary([string]$text) {
    if($text -notmatch 'src/common/native-hook/installer\.h' -or
       $text -match 'src/nthook32-dll/hook\.h'){
        throw 'Private installer header reached worker'
    }
}
foreach($case in @(
    @($WorkerX86BuildRoot,'obj/adapter-win32/command_process_compat.obj','ntvdm.exe'),
    @($WorkerX64BuildRoot,'obj/ntvwm/execution.obj','ntvwm.exe'))) {
    $graph=Get-Content (Join-Path $case[0] 'build.ninja') -Raw
    $link=[regex]::Match($graph,'(?m)^build '+[regex]::Escape($case[2])+'[^\r\n]*')
    if(!$link.Success -or $link.Value -notmatch 'common-native-hook-installer\.lib' -or
        $graph -match 'src/nthook32-dll/installer\.cpp'){
        throw "Installer not shared for $($case[2])"
    }
    $deps=@(& ninja -C $case[0] -t deps $case[1] 2>&1)
    if($LASTEXITCODE -or ($deps -join "`n") -notmatch '#deps \d+.*\(VALID\)'){
        throw "Missing compiler dependencies for $($case[1])"
    }
    $text=($deps -join "`n").Replace('\','/')
    Assert-Boundary $text
    $rejected=$false
    try {Assert-Boundary ($text+"`nsrc/nthook32-dll/hook.h")} catch {$rejected=$true}
    if(!$rejected){throw 'Private-header negative control accepted'}
}
Write-Output 'PASS both worker installer ownership and private-header negative controls'
