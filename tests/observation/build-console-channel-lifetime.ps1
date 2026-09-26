param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'BuildRoot must be below repository build'
}
New-Item -ItemType Directory -Path $build -ErrorAction Stop | Out-Null
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$lines=& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && set')
if($LASTEXITCODE){throw 'VS toolchain initialization failed'}
foreach($line in $lines){$i=$line.IndexOf('=');if($i -gt 0){Set-Item -LiteralPath ('env:'+$line.Substring(0,$i)) -Value $line.Substring($i+1)}}
& cl.exe /nologo /MT /W4 /c /I (Join-Path $root 'src') `
    (Join-Path $root 'src/opennt-abi/host-compat/console_grid.c') "/Fo:$build/grid.obj"
if($LASTEXITCODE){throw 'Production grid compilation failed'}
& cl.exe /nologo /MT /W4 /I (Join-Path $root 'src') `
    (Join-Path $root 'tests/app/console_channel_lifetime_test.c') `
    "/Fo:$build/test.obj" "/Fe:$build/test.exe" /link "$build/grid.obj" user32.lib gdi32.lib
if($LASTEXITCODE){throw 'Fixture compilation failed'}
Get-FileHash (Join-Path $build 'test.exe')
