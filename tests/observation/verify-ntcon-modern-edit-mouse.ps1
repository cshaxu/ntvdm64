[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$ReportPath
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$build.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'BuildRoot must remain under repository build/'
}
$report=[IO.Path]::GetFullPath($ReportPath)
if([IO.Path]::GetDirectoryName($report) -notin @('O:\winnt\logs','O:\winnt\Logs2')){
    throw 'ReportPath must be an approved package log'
}
if((Test-Path $build) -or (Test-Path $report)){throw 'Use fresh build and report paths'}
[void](Get-Command cl.exe -ErrorAction Stop)
[void](New-Item -ItemType Directory -Path $build)
$exe=Join-Path $build 'ntcon-modern-edit-mouse.exe'
Push-Location $repo
try {
    & cl.exe /nologo /W4 /MT /Isrc `
        tests/observation/ntcon_modern_edit_mouse_test.c `
        src/ntcon-exe/console_state.c `
        "/Fo$build\" "/Fe$exe" /link /incremental:no user32.lib `
        *> (Join-Path $build 'build.log')
    if($LASTEXITCODE){throw "Compile failed: $build/build.log"}
    $process=Start-Process -FilePath $exe -ArgumentList ('"'+$report+'"') `
        -WindowStyle Hidden -PassThru
    if(!$process.WaitForExit(20000)){
        Stop-Process -Id $process.Id -Force
        throw 'Modern Edit input test timed out'
    }
    if($process.ExitCode -or
        !(Select-String -LiteralPath $report -SimpleMatch 'NATIVE-MOUSE-RECORD ignored by VT Edit' -Quiet) -or
        !(Select-String -LiteralPath $report -SimpleMatch 'EDIT-VT-MOUSE exit=0' -Quiet) -or
        !(Select-String -LiteralPath $report -SimpleMatch 'New File' -Quiet)){
        throw "Modern Edit menu click did not pass: $report"
    }
    Get-Content -LiteralPath $report | Select-Object -Last 2
} finally {Pop-Location}
