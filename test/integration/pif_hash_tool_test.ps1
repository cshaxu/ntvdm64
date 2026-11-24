param(
    [Parameter(Mandatory)][string]$Pif,
    [Parameter(Mandatory)][string]$Hash,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$ReleaseRoot
)
$ErrorActionPreference = 'Stop'
New-Item -ItemType Directory -Force -Path $BuildRoot | Out-Null
$sample = Join-Path $BuildRoot 'sample.pif'
$closeOnExit = Join-Path $BuildRoot 'close-on-exit.pif'
$invalid = Join-Path $BuildRoot 'invalid.pif'
$tampered = Join-Path $BuildRoot 'tampered.pif'
& $Pif create $sample --title Sample --program 'C:\DOS\COMMAND.COM' --directory 'C:\DOS' --arguments '/P' --config 'C:\PATCH\CONFIG.NT' --autoexec 'C:\PATCH\AUTOEXEC.NT'
if ($LASTEXITCODE -ne 0) { throw 'PIF create failed' }
$initial = & $Pif show $sample
if ($LASTEXITCODE -ne 0 -or
    $initial -notcontains 'TITLE=Sample' -or
    $initial -notcontains 'STANDARD.MAXMEM_KB=0' -or
    $initial -notcontains 'W386.MAX_XMS_KB=1024' -or
    $initial -notcontains 'NT31.FLAGS=0x00000000') {
    throw 'PIF show did not expose standard, Windows 386, and NT 3.1 fields'
}
& $Pif create $closeOnExit --title Setup --program 'C:\DOS\SETUP.EXE' --directory 'C:\DOS' --arguments '' --config 'C:\PATCH\CONFIG.NT' --autoexec 'C:\PATCH\AUTOEXEC.NT' --close-on-exit --w386-max-xms 65535 --w386-flags 0x1003 --nt31-flags 1
if ($LASTEXITCODE -ne 0) { throw 'PIF close-on-exit create failed' }
$closeOnExitView = & $Pif show $closeOnExit
if ($LASTEXITCODE -ne 0 -or $closeOnExitView -notcontains 'STANDARD.MS_FLAGS=0x10' -or
    $closeOnExitView -notcontains 'STANDARD.CLOSE_ON_EXIT=YES' -or
    $closeOnExitView -notcontains 'W386.MAX_XMS_KB=65535' -or
    $closeOnExitView -notcontains 'W386.FLAGS=0x00001003' -or
    $closeOnExitView -notcontains 'NT31.FLAGS=0x00000001') {
    throw 'PIF close-on-exit did not persist the standard MS flag'
}
& $Pif update $sample --title Edited --arguments '/C VER' --close-on-exit --w386-max-xms 2048 --w386-flags 0x20 --nt31-flags 0x4
if ($LASTEXITCODE -ne 0) { throw 'PIF update failed' }
$edited = & $Pif show $sample
if ($LASTEXITCODE -ne 0 -or $edited -notcontains 'TITLE=Edited' -or $edited -notcontains 'ARGUMENTS=/C VER' -or
    $edited -notcontains 'STANDARD.CLOSE_ON_EXIT=YES' -or $edited -notcontains 'W386.MAX_XMS_KB=2048' -or
    $edited -notcontains 'W386.FLAGS=0x00000020' -or $edited -notcontains 'NT31.FLAGS=0x00000004') { throw 'PIF update did not persist fields or CloseOnExit' }
[IO.File]::WriteAllBytes($invalid, [Text.Encoding]::ASCII.GetBytes('not a pif'))
$priorPreference = $ErrorActionPreference
$ErrorActionPreference = 'Continue'
& $Pif show $invalid 2>$null
$invalidExit = $LASTEXITCODE
$ErrorActionPreference = $priorPreference
if ($invalidExit -eq 0) { throw 'PIF show accepted malformed input' }
$bytes = [IO.File]::ReadAllBytes($sample)
$bytes[10] = $bytes[10] -bxor 1
[IO.File]::WriteAllBytes($tampered, $bytes)
$ErrorActionPreference = 'Continue'
& $Pif show $tampered 2>$null
$tamperedExit = $LASTEXITCODE
$ErrorActionPreference = $priorPreference
if ($tamperedExit -eq 0) { throw 'PIF show accepted a bad fixed-record checksum' }
$legacy = Join-Path $BuildRoot 'legacy-no-checksum.pif'
Copy-Item -LiteralPath $sample -Destination $legacy
$bytes = [IO.File]::ReadAllBytes($legacy)
$bytes[1] = 0
[IO.File]::WriteAllBytes($legacy, $bytes)
& $Pif show $legacy | Out-Null
if ($LASTEXITCODE -ne 0) { throw 'PIF show rejected a valid zero-checksum legacy PIF' }
& $Pif update $legacy --title Legacy
if ($LASTEXITCODE -ne 0) { throw 'PIF update rejected a valid zero-checksum legacy PIF' }
if (([IO.File]::ReadAllBytes($legacy))[1] -ne 0) { throw 'PIF update invented a checksum for a legacy zero-checksum PIF' }
$expected = (Get-FileHash -Algorithm SHA256 -LiteralPath $sample).Hash
$actual = (& $Hash $sample).Trim()
if ($LASTEXITCODE -ne 0 -or $actual -ne $expected -or $actual -notmatch '^[0-9A-F]{64}$') { throw 'HASH output differs from SHA-256 reference' }
$ErrorActionPreference = 'Continue'
& $Hash (Join-Path $BuildRoot 'missing.bin') 2>$null
$missingHashExit = $LASTEXITCODE
$ErrorActionPreference = $priorPreference
if ($missingHashExit -eq 0) { throw 'HASH accepted a nonexistent file' }
if ($ReleaseRoot) {
    $manifestPath = Join-Path $ReleaseRoot 'addon-manifest.json'
    $manifest = Get-Content -Raw -LiteralPath $manifestPath | ConvertFrom-Json
    $utilities=@($manifest.utilities)
    $required=@('pif','hash')
    if ($manifest.schema -ne 1 -or $utilities.Count -lt $required.Count) {
        throw 'Release manifest is missing required PIF/HASH utilities'
    }
    foreach($id in $required){if(@($utilities|Where-Object {$_.id -eq $id}).Count -ne 1){throw "Release manifest has no unique required utility: $id"}}
    foreach ($utility in $manifest.utilities) {
        if ($utility.architecture -ne 'amd64') { throw "Unexpected utility architecture: $($utility.file)" }
        $image = Join-Path $ReleaseRoot $utility.file
        if (!(Test-Path -LiteralPath $image -PathType Leaf)) { throw "Missing released utility: $($utility.file)" }
        if ((Get-FileHash -LiteralPath $image).Hash -ne $utility.sha256) { throw "Released utility hash mismatch: $($utility.file)" }
    }
}
'PASS PIF create/show/update and HASH single-file output'
# Conventional memory is a native format edit, replacing test-owned byte patchers.
$memory="$BuildRoot/memory.pif"
$plain="$BuildRoot/memory-plain.pif"
$updated="$BuildRoot/memory-updated.pif"
foreach($title in @('VM identity A','VM reuse','Prompt probe B','Guest DOS','Scheduling prompt')){
 $fields=@('--title',$title,'--program','O:\tmp\FIXTURE\PATCH\A.COM','--directory','O:\tmp\FIXTURE\PATCH','--arguments','','--config','','--autoexec','','--close-on-exit','--w386-flags','0x21002')
 & $Pif create $plain @fields
 if($LASTEXITCODE){throw 'Memory reference create failed'}
 $expected=[IO.File]::ReadAllBytes($plain)
 $text=[Text.Encoding]::ASCII.GetString($expected)
 $std=$text.IndexOf('MICROSOFT PIFEX');$header=$text.IndexOf('WINDOWS 386 3.0')
 $body=[BitConverter]::ToUInt16($expected,$header+18)
 foreach($offset in @(32,$body)){[Array]::Copy([BitConverter]::GetBytes([uint16]640),0,$expected,$offset,2)}
 foreach($offset in @(34,($body+2))){[Array]::Copy([BitConverter]::GetBytes([uint16]128),0,$expected,$offset,2)}
 $sum=0;for($i=2;$i -lt $std;$i++){$sum=($sum+$expected[$i]) -band 255};$expected[1]=[byte]$sum
 & $Pif create $memory @fields --min-memory 128 --max-memory 640
 if($LASTEXITCODE -or [Convert]::ToBase64String([IO.File]::ReadAllBytes($memory)) -cne [Convert]::ToBase64String($expected)){throw 'Native memory create differs from former patch'}
 Copy-Item $plain $updated -Force
 & $Pif update $updated --min-memory 128 --max-memory 640
 if($LASTEXITCODE){throw 'Native memory update failed'}
 # update preserves the documented zero-checksum form, unlike explicit create.
 if(([IO.File]::ReadAllBytes($plain))[1] -eq 0){$expected[1]=0}
 if([Convert]::ToBase64String([IO.File]::ReadAllBytes($updated)) -cne [Convert]::ToBase64String($expected)){throw 'Native memory update changed unrelated bytes'}
 $view=& $Pif show $memory
 foreach($field in @('STANDARD.MAXMEM_KB=640','STANDARD.MINMEM_KB=128','W386.MAXMEM_KB=640','W386.MINMEM_KB=128')){if($view -notcontains $field){throw "Missing memory field: $field"}}
}
foreach($arguments in @(@('--min-memory','65536'),@('--max-memory','-1'),@('--min-memory','bad'),@('--max-memory'))){
 $before=(Get-FileHash $memory).Hash
 & $Pif update $memory @arguments 2>$null
 if(!$LASTEXITCODE -or (Get-FileHash $memory).Hash -ne $before){throw 'Invalid memory option changed file'}
}
& $Pif update $legacy --min-memory 128 --max-memory 640
if($LASTEXITCODE -or ([IO.File]::ReadAllBytes($legacy))[1] -ne 0){throw 'Memory update changed zero-checksum convention'}
'PASS conventional memory create/update, five complete byte comparisons, invalid-input no-write and legacy checksum'
