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
$driver=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
$rows=@()
foreach($case in @('current','original')){
    $source=if($case -eq 'current'){Join-Path $repo 'src/mvdm-host'}else{$original}
    $bodyPath=Join-Path $source 'dos/dem/demgset.c'
    $typePath=Join-Path $source 'dos/dem/dosdef.h'
    $body=[regex]::Match([IO.File]::ReadAllText($bodyPath),'(?s)BOOL\s*\r?\nGetMediaId\(.*?return TRUE;\s*\}')
    $type=[regex]::Match([IO.File]::ReadAllText($typePath),'(?s)#pragma pack\(1\)\s*typedef struct _VOLINFO.*?typedef VOLINFO \*PVOLINFO;')
    if(!$body.Success -or !$type.Success){throw 'Exact owner/type extraction failed'}
    $dir=Join-Path $out $case;$null=New-Item -ItemType Directory $dir
    [IO.File]::WriteAllText("$dir/dem-media-body.inc",$body.Value)
    [IO.File]::WriteAllText("$dir/dem-media-type.inc",$type.Value)
    $cmd='@call "'+$driver+'" cl.exe /nologo /MT /W4 /I"'+$dir+'" /Fo"'+$dir+'/media.obj" /Fe"'+$dir+'/media.exe" "'+$fixture+'"'
    [IO.File]::WriteAllText("$dir/build.cmd",$cmd+"`r`n@exit /b %errorlevel%`r`n")
    foreach($step in @('build','run')){
        $image=if($step -eq 'build'){$env:ComSpec}else{"$dir/media.exe"}
        $arguments=if($step -eq 'build'){'/d /c "'+$dir+'\build.cmd"'}else{''}
        $p=Start-Process $image -ArgumentList $arguments -WorkingDirectory $dir -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/$step.stdout" -RedirectStandardError "$dir/$step.stderr"
        $null=$p.Handle
        @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments=$arguments}|ConvertTo-Json|Set-Content "$dir/$step.owner.json"
        try{if(!$p.WaitForExit(120000)){$p.Kill();$p.WaitForExit();throw 'Owned fixture deadline'};$p.WaitForExit();$exitCode=$p.ExitCode}finally{$p.Dispose()}
        $exitCode|Set-Content "$dir/$step.exit"
        if($step -eq 'build' -and $exitCode){throw 'Owner fixture compile failed'}
    }
    $text=Get-Content "$dir/run.stdout" -Raw
    if($case -eq 'current'){
        if($exitCode -ne 0 -or $text -notmatch 'S37_DEM_MEDIA_PADDING_LABEL_CANARY_FAILURE_OK'){throw 'Current owner failed'}
    }elseif($exitCode -ne 1 -or $text -notmatch 'S37_MEDIA_FAIL line=42'){throw 'Original did not fail at the expected volume-label assertion'}
    $rows+=@{case=$case;exit=$exitCode;output=$text.Trim();source_sha256=(Get-FileHash $bodyPath).Hash;type_sha256=(Get-FileHash $typePath).Hash}
}
@{scope='Exact source owner extraction with mocked volume-query endpoint; not a fresh DOS integration test';fixture_sha256=(Get-FileHash $fixture).Hash;cases=$rows}|ConvertTo-Json -Depth 5|Set-Content "$out/result.json"
Write-Output 'PASS current owner; original rejected at overwritten volume label'
