[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerX86BuildRoot,
      [Parameter(Mandatory)][string]$WorkerX64BuildRoot)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/native-source-graph.ps1"
function Assert-Boundary([string]$text) {
    if($text -notmatch 'src/common/native-hook/installer\.h' -or
       $text -match 'src/nthook-dll/hook\.h'){
        throw 'Private installer header reached worker'
    }
}
foreach($case in @(
    @($WorkerX86BuildRoot,'src/ntvdm-exe/win32/command_process_compat.c','ntvdm.exe'),
    @($WorkerX64BuildRoot,'src/ntvwm-exe/execution.c','ntvwm.exe'))) {
    $graph=Get-Content (Join-Path $case[0] 'build.ninja') -Raw
    $link=[regex]::Match($graph,'(?m)^build '+[regex]::Escape($case[2])+'[^\r\n]*')
    if(!$link.Success -or $link.Value -notmatch 'common-native-hook-installer\.lib' -or
        $graph -match 'src/nthook-dll/installer\.cpp'){
        throw "Installer not shared for $($case[2])"
    }
    $text=Get-ProductCompilerDependencies $case[0] $case[2] $case[1]
    Assert-Boundary $text
    $rejected=$false
    try {Assert-Boundary ($text+"`nsrc/nthook-dll/hook.h")} catch {$rejected=$true}
    if(!$rejected){throw 'Private-header negative control accepted'}
}
Write-Output 'PASS both worker installer ownership and private-header negative controls'
