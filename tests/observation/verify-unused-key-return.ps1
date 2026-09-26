param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'BuildRoot must be below repository build'
}
New-Item -ItemType Directory -Path $build -ErrorAction Stop | Out-Null
$source=Join-Path $root 'src/mvdm/softpc.new/host/src/nt_event.c'
$body=[regex]::Match([IO.File]::ReadAllText($source),'(?ms)^void ReturnUnusedKeyEvents\(int UnusedKeyEvents\)\r?\n\{.*?^\}')
if(!$body.Success){throw 'Original function extraction failed'}
[IO.File]::WriteAllText((Join-Path $build 'unused_key_return.inc'),$body.Value)
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$lines=& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && set')
foreach($line in $lines){$i=$line.IndexOf('=');if($i -gt 0){Set-Item -LiteralPath ('env:'+$line.Substring(0,$i)) -Value $line.Substring($i+1)}}
& cl.exe /nologo /MT /W4 /I $build (Join-Path $root 'tests/app/unused_key_return_test.c') "/Fo:$build/test.obj" "/Fe:$build/test.exe"
if($LASTEXITCODE){throw 'Fixture compilation failed'}
Get-FileHash -LiteralPath $source,(Join-Path $build 'unused_key_return.inc') | Format-List
& (Join-Path $build 'test.exe')
if($LASTEXITCODE){throw 'Original key-return contract failed'}
