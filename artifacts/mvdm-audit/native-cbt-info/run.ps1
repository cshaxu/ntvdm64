$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$out=Join-Path $repo 'build/t473-s10/native-cbt-info'
if(Test-Path -LiteralPath $out){throw 'Fresh evidence directory required'}
$null=New-Item -ItemType Directory $out
Push-Location $out
try {
    & "$repo/build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd" cl.exe /nologo /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS /Foprobe.obj /Feprobe.exe "$repo/build/t473-s10/native-cbt-info.c" /link user32.lib *> compile.txt
    if($LASTEXITCODE){throw 'Native probe compilation failed'}
    & ./probe.exe "$out/result.txt"
    $code=$LASTEXITCODE
    Get-Content result.txt
    exit $code
} finally {Pop-Location}
