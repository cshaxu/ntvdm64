[CmdletBinding()]
param(
    [string]$FormalRoot = 'build/M0-T420/S35/formal-native-wait-r2',
    [string]$BuildRoot = 'build/M0-T420/S36/original-xms-compile-r1'
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot\..\..").Path
$graph = Get-Content (Join-Path $FormalRoot 'build.ninja') -Raw
$match = [regex]::Match($graph, '(?m)^build obj/xms/xmsblock.obj:[^\r\n]+\r?\n  cflags = ([^\r\n]+)')
if (!$match.Success) { throw 'Formal XMS flags not found' }
$flags = $match.Groups[1].Value.Replace('$:', ':')
if (Test-Path $BuildRoot) { throw 'Use a fresh compile-probe root' }
$build = (New-Item -ItemType Directory -Path $BuildRoot).FullName
$source = Join-Path $repo 'src\mvdm\xms.486\xmsmemr.c'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$command = 'call "' + $vs + '" -arch=x86 -host_arch=x64 >nul && cl.exe ' +
    $flags + ' /Fo"' + $build + '\xmsmemr.obj" "' + $source + '"'
& cmd.exe /d /s /c $command *> (Join-Path $build 'compile.log')
$result = $LASTEXITCODE
@{ Source = $source; Sha256 = (Get-FileHash $source).Hash;
   FormalGraphSha256 = (Get-FileHash (Join-Path $FormalRoot 'build.ninja')).Hash;
   Flags = $flags; ExitCode = $result } | ConvertTo-Json |
    Set-Content (Join-Path $build 'result.json') -Encoding utf8
Get-Content (Join-Path $build 'compile.log') | Select-String 'error |warning '
Write-Host "Original XMS compile exit=$result; evidence=$build"
exit $result
