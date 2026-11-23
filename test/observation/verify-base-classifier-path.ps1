param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned root required'}
New-Item -ItemType Directory $out | Out-Null
foreach($arch in @('x86','x64')) {
    $part=if($arch -eq 'x86'){'worker'}else{'launcher'}
    $object="$repo/build/.ninja/msvc-$arch-release/$part/obj/opennt-base-bindings/classifier-path.obj"
    $dir=New-Item -ItemType Directory "$out/$arch"
    Get-FileHash $object,"$repo/test/compat/base_classifier_path.c","$repo/src/compat/base_classifier_path.c","$repo/src/compat/include/base_classifier_path.h" | Export-Csv "$dir/inputs.csv" -NoTypeInformation
    foreach($step in @('compile','run')) {
        $file=if($step -eq 'compile'){$env:ComSpec}else{"$repo/build/.ninja/fixtures-$arch/base-classifier-path.exe"}
        $options=@{FilePath=$file;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$dir/$step.log";RedirectStandardError="$dir/$step.err"}
        if($step -eq 'compile'){$options.ArgumentList=@('/d','/c',"`"$repo/tools/build-fixtures.cmd`" $arch base-classifier-path")}
        $p=Start-Process @options;$null=$p.Handle
        $p.Id | Set-Content "$dir/$step.pid"
        if(!$p.WaitForExit(60000)){throw "Owned $arch $step timed out; PID $($p.Id)"}
        $p.Refresh();$p.ExitCode | Set-Content "$dir/$step.exit"
        if($p.ExitCode){throw "$arch $step failed: $($p.ExitCode)"}
    }
    if((Get-Content "$dir/run.log" -Raw) -notmatch 'CLASSIFIER_PATH failures=0 layout=(12|24) cases=3'){throw 'Incomplete classifier result'}
    foreach($row in Import-Csv "$dir/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Input drift'}}
    Get-Content "$dir/run.log"
}
