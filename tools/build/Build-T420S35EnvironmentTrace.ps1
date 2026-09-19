[CmdletBinding()]
param([Parameter(Mandatory)][string]$FormalRoot,
      [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot\..\..").Path
$formal = (Resolve-Path $FormalRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (!$build.StartsWith("$repo\build\", [StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $build)) { throw 'Use a fresh repository build directory' }
New-Item -ItemType Directory $build | Out-Null
$graph = Get-Content (Join-Path $formal 'build.ninja')
$flags = ($graph | Where-Object { $_.StartsWith('cflags = ') }).Substring(9).Replace('$:', ':')
$object = Join-Path $build 'cmdenv.obj'
$library = Join-Path $build 'original-mvdm-command.lib'
$exe = Join-Path $build 'ntvdm.exe'
$commandObjects = ($graph | Where-Object { $_.StartsWith('build original-mvdm-command.lib: lib ') }).Split(' ',4)[3].Split(' ')
$workerInputs = ($graph | Where-Object { $_.StartsWith('build ntvdm.exe | ntvdm.lib: worker_link ') }).Substring('build ntvdm.exe | ntvdm.lib: worker_link '.Length).Split(' ')
$inputs = @($commandObjects + $workerInputs | Sort-Object -Unique)
$inputs | ForEach-Object { Get-FileHash (Join-Path $formal $_) } |
    Select-Object Path,Hash | ConvertTo-Json | Set-Content "$build\reused-inputs.json"
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
Push-Location $formal
try {
    $compile = "call `"$vs`" -arch=x86 -host_arch=x86 >nul && cl $flags /Fo`"$object`" `"$repo\tests\observation\command_environment_trace.c`""
    cmd.exe /d /s /c $compile *> "$build\compile.log"
    if ($LASTEXITCODE) { throw "Compile failed; see $build\compile.log" }
    $members = ($commandObjects | ForEach-Object {
        if ($_ -eq 'obj/command/cmdenv.obj') { "`"$object`"" } else { $_ }
    }) -join ' '
    cmd.exe /d /s /c "call `"$vs`" -arch=x86 -host_arch=x86 >nul && lib /nologo /out:`"$library`" $members" *> "$build\archive.log"
    if ($LASTEXITCODE) { throw 'Archive failed' }
    $members = ($workerInputs | ForEach-Object {
        if ($_ -eq 'original-mvdm-command.lib') { "`"$library`"" } else { $_ }
    }) -join ' '
    cmd.exe /d /s /c "call `"$vs`" -arch=x86 -host_arch=x86 >nul && link /nologo /subsystem:console /opt:ref /out:`"$exe`" /map:`"$exe.map`" /implib:`"$build\ntvdm.lib`" /def:generated/ntvdm-wow32-provider.def $members rpcrt4.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib" *> "$build\link.log"
    if ($LASTEXITCODE) { throw 'Link failed' }
} finally { Pop-Location }
Get-FileHash $exe
