param(
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [Parameter(Mandatory=$true)][string]$LogPath,
    [string]$OriginalSource = 'O:\repos.external\OpenNT\windows\core\ntuser\kernel\sendmsg.c',
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$root = [IO.Path]::GetFullPath($BuildRoot)
if (!$root.StartsWith((Join-Path $repo 'build\'), [StringComparison]::OrdinalIgnoreCase)) { throw 'Build root must be below repository build.' }
if (Test-Path -LiteralPath $root) { throw 'Use a fresh build root.' }
if (Test-Path -LiteralPath $LogPath) { throw 'Do not overwrite prior observations.' }
$hash = (Get-FileHash -LiteralPath $OriginalSource).Hash
if ($hash -ne 'AF68BADCB022EFCB3947968FC6E3DBF148B697072EBEB2898F97FBCB774039F3') { throw 'Original source identity mismatch.' }
$source = [IO.File]::ReadAllText($OriginalSource)
$start = $source.IndexOf('BOOL _ReplyMessage(')
$end = $source.IndexOf('BOOL xxxSendBSMtoDesktop(', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Original source boundaries missing.' }
New-Item -ItemType Directory -Path $root | Out-Null
# Generated test input only; no production or guest file is modified.
[IO.File]::WriteAllText((Join-Path $root 'original_reply_body.inc'), $source.Substring($start, $end-$start), [Text.Encoding]::ASCII)
$fixture = Join-Path $repo 'tests\adapter-mvdm-host-out\wow\wow_reply_contract_fixture.c'
$cmd = 'call "'+$VsDevCmd+'" -arch=x86 -host_arch=x86 >nul && cl /nologo /TC /MT /W4 /I"'+$root+'" /Fo"'+$root+'\fixture.obj" /Fe"'+$root+'\fixture.exe" "'+$fixture+'" /link /MACHINE:X86'
& cmd.exe /d /s /c $cmd *> (Join-Path $root 'build.log')
if ($LASTEXITCODE) { Get-Content (Join-Path $root 'build.log'); throw 'Original reply fixture build failed.' }
& (Join-Path $root 'fixture.exe') *> $LogPath
$result = $LASTEXITCODE
Get-Content -LiteralPath $LogPath
Get-FileHash -LiteralPath (Join-Path $root 'fixture.exe'),$LogPath
if ($result -or !(Select-String -LiteralPath $LogPath -SimpleMatch 'WOW_ORIGINAL_REPLY_CONTRACT cases=12 errors=0')) { throw 'Original reply contract failed.' }
