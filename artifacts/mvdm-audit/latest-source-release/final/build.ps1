$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$out=Join-Path $repo 'build/t473-s10/latest-source-release-r3'
$receipt=[ordered]@{source_commit=(& git rev-parse HEAD);started=[DateTime]::UtcNow.ToString('o');result='running';runs=@()}
foreach($arch in @('x86','x64')){
    $arguments='/d /s /c ""'+$out+'\'+$arch+'.cmd""'
    $p=Start-Process $env:ComSpec -ArgumentList $arguments -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$arch.stdout.txt" -RedirectStandardError "$out/$arch.stderr.txt"
    $receipt.runs+=@{architecture=$arch;pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$env:ComSpec;arguments=$arguments}
    $receipt|ConvertTo-Json -Depth 6|Set-Content "$out/build-result.json"
    $p.WaitForExit();$p.WaitForExit();$receipt.runs[-1].exit=$p.ExitCode;$p.Dispose()
    if($receipt.runs[-1].exit){$receipt.result='fail';$receipt|ConvertTo-Json -Depth 6|Set-Content "$out/build-result.json";exit 1}
}
$receipt.result='pass';$receipt.finished=[DateTime]::UtcNow.ToString('o');$receipt|ConvertTo-Json -Depth 6|Set-Content "$out/build-result.json"
'PASS cached x86 worker/WOW32/redirector/hook32 and x64 five native executables/hook64 build'
