param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'BuildRoot must be below repository build'
}
New-Item -ItemType Directory -Path $build -Force | Out-Null
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$lines=& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && set')
if($LASTEXITCODE){throw 'VS initialization failed'}
foreach($line in $lines){$i=$line.IndexOf('=');if($i -gt 0){Set-Item -LiteralPath ('env:'+$line.Substring(0,$i)) -Value $line.Substring($i+1)}}
& cl.exe /nologo /MT /W4 /c (Join-Path $PSScriptRoot 'run16_unpaired_native_test.c') "/Fo:$build\test.obj"
if($LASTEXITCODE){throw 'Fixture compilation failed'}
& link.exe /nologo /subsystem:console "/out:$build\unpaired-cui.exe" "$build\test.obj" kernel32.lib
if($LASTEXITCODE){throw 'CUI link failed'}
& link.exe /nologo /subsystem:windows /entry:wmainCRTStartup "/out:$build\unpaired-gui.exe" "$build\test.obj" kernel32.lib
if($LASTEXITCODE){throw 'GUI link failed'}
Get-FileHash (Join-Path $build 'unpaired-cui.exe'),(Join-Path $build 'unpaired-gui.exe')
