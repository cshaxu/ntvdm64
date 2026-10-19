param([Parameter(Mandatory)][string]$BuildRoot,[string]$System32)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
$null=New-Item -ItemType Directory -Force $root
foreach($arch in @('x86','x64')) {
    $dir=Join-Path $root $arch
    $null=New-Item -ItemType Directory -Force $dir
    $compile="$repo/build/.ninja/msvc-$arch-release/style-fixtures"
    & "$repo/tools/build/New-Ntw16sNinja.ps1" -BuildRoot $compile -Architecture $arch -IncludeTests
    & $env:ComSpec /d /c "$compile/build-style.cmd" *> "$dir/build.txt"
    if($LASTEXITCODE){throw "Style build failed: $arch"}
    Copy-Item "$compile/*.exe","$compile/*.dll" $dir
    $name=if($arch -eq 'x86'){'ntw16s32.dll'}else{'ntw16s64.dll'}
    $dll=if($System32){(Resolve-Path (Join-Path $System32 $name)).Path}else{"$dir/$name"}
    Get-FileHash $dll,"$dir/style-attachment-test.exe","$repo/tests/app/style_attachment.c"|ConvertTo-Json|Set-Content "$dir/attachment-inputs.json"
    $p=Start-Process "$dir/style-attachment-test.exe" -ArgumentList ('"'+$dll+'"') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/attachment.out" -RedirectStandardError "$dir/attachment.err"
    $null=$p.Handle
    if(!$p.WaitForExit(60000)){$p.Kill();throw "Attachment test timeout: $arch"}
    $p.WaitForExit()
    if($p.ExitCode -ne 0){throw "Attachment test failed: $arch exit=$($p.ExitCode)"}
    Get-Content "$dir/attachment.out"
}
