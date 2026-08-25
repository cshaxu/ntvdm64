[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Sha256([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
$build = [IO.Path]::GetFullPath($BuildRoot)
if (-not $build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) { throw "BuildRoot must remain below ${buildBase}: $build" }
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }

$sourceMvdm = Join-Path $repository 'src\opennt-guest\base\mvdm'
$tools = Join-Path $repository 'tools\historical\opennt-4.5'
$runner = Join-Path $repository 'tools\build\Invoke-DosBoxBatchWithCompletion.ps1'
foreach ($path in @($sourceMvdm, (Join-Path $tools 'buildmsg.exe'), (Join-Path $tools 'masm.exe'), (Join-Path $tools 'link16.exe'), (Join-Path $tools 'reloc.exe'), $runner)) { if (-not (Test-Path -LiteralPath $path)) { throw "Required primary COMMAND input missing: $path" } }

$stageMvdm = Join-Path $build 'base\mvdm'
New-Item -ItemType Directory -Path $stageMvdm | Out-Null
Copy-Item -LiteralPath (Join-Path $sourceMvdm 'dos\v86\cmd\command') -Destination (Join-Path $stageMvdm 'dos\v86\cmd\command') -Recurse
Copy-Item -LiteralPath (Join-Path $sourceMvdm 'dos\v86\inc') -Destination (Join-Path $stageMvdm 'dos\v86\inc') -Recurse
Copy-Item -LiteralPath (Join-Path $sourceMvdm 'dos\v86\messages') -Destination (Join-Path $stageMvdm 'dos\v86\messages') -Recurse
Copy-Item -LiteralPath (Join-Path $sourceMvdm 'inc') -Destination (Join-Path $stageMvdm 'inc') -Recurse

$command = Join-Path $stageMvdm 'dos\v86\cmd\command'
Remove-Item -LiteralPath (Join-Path $command 'command.exc') -Force
Copy-Item -LiteralPath (Join-Path $stageMvdm 'dos\v86\messages\usa-ms.msg'), (Join-Path $stageMvdm 'dos\v86\messages\usa-ms.idx') -Destination $command
Copy-Item -LiteralPath (Join-Path $tools 'buildmsg.exe') -Destination $command
[IO.File]::WriteAllText((Join-Path $command 'RUN.BAT'), "@echo off`r`nbuildmsg usa-ms command.skl > B.LOG`r`necho done > DONE.OK`r`n", [Text.ASCIIEncoding]::new())
& $runner -RepositoryRoot $repository -WorkingDirectory $command -BatchFile 'RUN.BAT' -CompletionFiles @('COMMAND.CTL','COMMAND.CL1','COMMAND.CL2','COMMAND.CL3','COMMAND.CL4','COMMAND.CLF') -TimeoutSeconds 60
foreach ($name in @('COMMAND.CTL','COMMAND.CL1','COMMAND.CL2','COMMAND.CL3','COMMAND.CL4','COMMAND.CLF')) { if (-not (Test-Path -LiteralPath (Join-Path $command $name))) { throw "BUILDMSG output missing: $name" } }

$modules = @('command1','command2','rucode','stub','rdata','init','iparse','uinit','tcode','tbatch','tbatch2','tfor','dir','tcmd1b','tcmd2a','tcmd2b','tenv','tenv2','tmisc1','tmisc2','tpipe','parse2','path1','path2','tucode','copy','copypr1','copypr2','cparse','tparse','tprintf','loadhi','tdata','tspc')
Push-Location $command
try {
    foreach ($module in $modules) {
        & (Join-Path $tools 'masm.exe') '/I.' '/I..\..\inc' '/I..\..\..\..\inc' '/I..\..\messages' '/Mx' '/t' '/W1' ("$module.asm,$module.obj;") 2>&1 | Tee-Object -FilePath (Join-Path $command "$module-masm.log")
        if ($LASTEXITCODE -ne 0) { throw "MASM failed for ${module}: $LASTEXITCODE" }
    }
    & (Join-Path $tools 'link16.exe') '@command.lnk' 2>&1 | Tee-Object -FilePath (Join-Path $command 'link16.log')
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath (Join-Path $command 'command.exc'))) { throw 'LINK16 did not produce COMMAND.EXC' }
} finally { Pop-Location }

Copy-Item -LiteralPath (Join-Path $tools 'reloc.exe') -Destination $command
[IO.File]::WriteAllText((Join-Path $command 'PACK.BAT'), "@echo off`r`ncopy command.exc command.exe > C.LOG`r`nreloc command command.com > R.LOG`r`necho done > PACK.OK`r`n", [Text.ASCIIEncoding]::new())
& $runner -RepositoryRoot $repository -WorkingDirectory $command -BatchFile 'PACK.BAT' -CompletionFiles @('command.com') -TimeoutSeconds 60

$output = Join-Path $command 'command.com'
$expectedHash = '908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43'
if (-not (Test-Path -LiteralPath $output) -or (Get-Item -LiteralPath $output).Length -ne 50384 -or (Get-Sha256 $output) -ne $expectedHash) { throw "Primary COMMAND identity mismatch: $output" }

$manifest = [ordered]@{
    schema = 'ntvdm64.t225.s8.command-primary-source-build.v1'
    classification = 'primary-original-toolchain-source-built'
    source = [ordered]@{ managedRoot = 'src/opennt-guest'; upstreamRevision = '5c5b979ec08c17d3ca2eb70e8aad62d26515d01c'; guestSourceTransforms = @() }
    tools = [ordered]@{ buildmsg = Get-Sha256 (Join-Path $tools 'buildmsg.exe'); masm = Get-Sha256 (Join-Path $tools 'masm.exe'); link16 = Get-Sha256 (Join-Path $tools 'link16.exe'); reloc = Get-Sha256 (Join-Path $tools 'reloc.exe') }
    modules = $modules
    output = [ordered]@{ relativePath = 'base/mvdm/dos/v86/cmd/command/COMMAND.COM'; bytes = 50384; sha256 = $expectedHash }
}
[IO.File]::WriteAllText((Join-Path $build 'command-build-manifest.json'), (($manifest | ConvertTo-Json -Depth 6) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Host "Primary COMMAND source build completed: $output"
