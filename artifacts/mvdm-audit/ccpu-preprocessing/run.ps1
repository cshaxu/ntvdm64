$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$worker=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$out=Join-Path $repo 'build/t473-s10/ccpu-selected-preprocessing'
if(Test-Path -LiteralPath $out){throw 'Fresh preprocessing directory required'}
$null=New-Item -ItemType Directory $out
$ninja=(Get-Command ninja.exe -CommandType Application|Select-Object -First 1).Source
$target='obj/ccpu/c_main.obj'
$command=@(& $ninja -C $worker -t commands $target)[-1]
if($LASTEXITCODE -or !$command.Contains('/Fo'+$target)){throw 'Actual selected CCPU command unavailable'}
$normal=@('obj/ccpu/c_main.obj','original-ccpu386.lib','ntvdm.exe','ntvdm.exe.map')|ForEach-Object {Get-FileHash (Join-Path $worker $_)}
$command|Set-Content "$out/original-command.txt"
$modified=$command.Replace('/Fo'+$target,('/P /Fi"'+($out+'/c_main.i').Replace('\','/')+'"'))
if($modified.Contains('/Fo'+$target) -or $modified -eq $command){throw 'Preprocessing-only output substitution failed'}
$modified|Set-Content "$out/preprocess-command.txt"
Push-Location $out
try {
    & cmd.exe /d /s /c ('call "'+$repo+'/build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd" '+$modified) *> "$out/preprocess.log"
    if($LASTEXITCODE){throw 'Selected preprocessing failed'}
} finally {Pop-Location}
if(!(Test-Path -LiteralPath "$out/c_main.i") -or (Get-Item "$out/c_main.i").Length -lt 10000){throw 'No actual preprocessor output'}
foreach($record in $normal){if((Get-FileHash $record.Path).Hash -ne $record.Hash){throw 'Normal cached product input changed'}}
$normal|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/normal-cache-inputs.json"
Get-FileHash $PSCommandPath,"$worker/build.ninja","$repo/src/mvdm/softpc.new/base/ccpu386/c_main.c","$out/c_main.i"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$out/inputs.json"
@{kind='selected-preprocessor-observation';product_source_changed=$false;guest_changed=$false;cached_product_changed=$false;bytes=(Get-Item "$out/c_main.i").Length;scope='Actual CPU40/SPC386/PROD compiler-selected declaration/body tokens, not object generation or behavior'}|ConvertTo-Json|Set-Content "$out/result.json"
Get-Content "$out/result.json"
