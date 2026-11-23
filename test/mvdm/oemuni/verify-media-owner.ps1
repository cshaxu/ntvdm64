[CmdletBinding()]
param([Parameter(Mandatory)][string]$OriginalRoot,
      [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../..'))
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $out
$original=(Resolve-Path $OriginalRoot).Path
$fixture=Join-Path $PSScriptRoot 'oemuni_media_fixture.c'
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 media-owner "'+$original+'""'
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/build.stdout" -RedirectStandardError "$out/build.stderr"
try {
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$out/build.owner.json"
    if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'Media producer deadline'}
    $p.WaitForExit();$p.ExitCode|Set-Content "$out/build.exit"
    if($p.ExitCode){throw 'Owner fixture compile failed'}
} finally {$p.Dispose()}
$cache=Join-Path $repo 'build/.ninja/fixtures-x86'
$rows=@()
foreach($case in @('current','original')){
    $source=if($case -eq 'current'){Join-Path $repo 'src/mvdm-host'}else{$original}
    $bodyPath=Join-Path $source 'dos/dem/demgset.c'
    $typePath=Join-Path $source 'dos/dem/dosdef.h'
    $dir=Join-Path $out $case;$null=New-Item -ItemType Directory $dir
    Copy-Item "$cache/media-owner/$case/dem-media-body.inc","$cache/media-owner/$case/dem-media-type.inc" $dir
    Copy-Item "$cache/media-owner-$case.exe" "$dir/media.exe"
    $image="$dir/media.exe"
    $p=Start-Process $image -WorkingDirectory $dir -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/run.stdout" -RedirectStandardError "$dir/run.stderr"
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments=''}|ConvertTo-Json|Set-Content "$dir/run.owner.json"
    try{if(!$p.WaitForExit(120000)){$p.Kill();$p.WaitForExit();throw 'Owned fixture deadline'};$p.WaitForExit();$exitCode=$p.ExitCode}finally{$p.Dispose()}
    $exitCode|Set-Content "$dir/run.exit"
    $text=Get-Content "$dir/run.stdout" -Raw
    if($case -eq 'current'){
        if($exitCode -ne 0 -or $text -notmatch 'S37_DEM_MEDIA_PADDING_LABEL_CANARY_FAILURE_OK'){throw 'Current owner failed'}
    }elseif($exitCode -ne 1 -or $text -notmatch 'S37_MEDIA_FAIL line=42'){throw 'Original did not fail at the expected volume-label assertion'}
    $rows+=@{case=$case;exit=$exitCode;output=$text.Trim();source_sha256=(Get-FileHash $bodyPath).Hash;type_sha256=(Get-FileHash $typePath).Hash}
}
@{scope='Exact source owner extraction with mocked volume-query endpoint; not a fresh DOS integration test';fixture_sha256=(Get-FileHash $fixture).Hash;cases=$rows}|ConvertTo-Json -Depth 5|Set-Content "$out/result.json"
Write-Output 'PASS current owner; original rejected at overwritten volume label'
