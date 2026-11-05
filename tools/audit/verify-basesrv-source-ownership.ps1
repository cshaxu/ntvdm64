[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$lines=@(Get-Content (Join-Path $BuildRoot 'build.ninja'))
$edges=@{}
foreach($line in $lines){
    if($line -notmatch '^build (.+?):\s+\S+\s+(.*)$'){continue}
    $outputs=($Matches[1] -split '\s+\|\s+')[0] -split '\s+'
    $inputs=($Matches[2] -split '\s+\|\|?\s+')[0] -split '\s+'
    foreach($output in $outputs){$edges[$output]=@($inputs)}
}
function Assert-NoServiceSources([string]$Target,[hashtable]$Graph){
    if(!$Graph.ContainsKey($Target)){throw "Missing product graph: $Target"}
    $pending=[Collections.Generic.Stack[string]]::new()
    $seen=[Collections.Generic.HashSet[string]]::new()
    $pending.Push($Target)
    while($pending.Count){
        $node=$pending.Pop()
        if(!$seen.Add($node)){continue}
        foreach($item in $Graph[$node]){
            $input=$item -replace '\$:',':'
            if($input -match '[/\\]src[/\\]ntsrv-exe[/\\]'){
                throw "$Target depends on NTSRV-private source: $input"
            }
            if($Graph.ContainsKey($item)){$pending.Push($item)}
        }
    }
}
foreach($target in @('run16-source-closure','ntcon-source-closure','ntvwm-source-closure','ntmon-source-closure','ntvdm.exe','nthook32.dll')){
    Assert-NoServiceSources $target $edges
}
# Hook64 and WOW32 have separately generated production graphs.
foreach($extra in @(@('hook64','nthook64.dll'),@('wow32','wow32.dll'))){
    $path=Join-Path (Split-Path (Resolve-Path $BuildRoot).Path -Parent) ($extra[0]+'/build.ninja')
    if(!(Test-Path -LiteralPath $path)){throw "Missing production graph: $path"}
    $other=@{}
    foreach($line in Get-Content $path){
        if($line -notmatch '^build (.+?):\s+\S+\s+(.*)$'){continue}
        $outputs=($Matches[1] -split '\s+\|\s+')[0] -split '\s+'
        $inputs=($Matches[2] -split '\s+\|\|?\s+')[0] -split '\s+'
        foreach($output in $outputs){$other[$output]=@($inputs)}
    }
    Assert-NoServiceSources $extra[1] $other
}
foreach($owner in @('lib-nxvm','lib-win32','common','worker-base','run16-exe','ntcon-exe','ntvdm-exe','ntvwm-exe','ntmon-exe','nthook32-dll','wow32-dll','vdmredir-dll')){
    foreach($file in Get-ChildItem (Join-Path $repo "src/$owner") -File -Recurse | Where-Object Extension -in '.c','.h','.cpp'){
        if((Get-Content $file.FullName -Raw) -match '#include\s*[<"]ntsrv-exe/'){
            throw "NTSRV-private include in $($file.FullName)"
        }
    }
}
$mutated=$edges.Clone()
$mutated['run16-source-closure']=@($edges['run16-source-closure'])+@('obj/broker-transport/rpc_security.obj')
$rejected=$false
try { Assert-NoServiceSources 'run16-source-closure' $mutated } catch { $rejected=$true }
if(!$rejected){throw 'NTSRV-private object negative control accepted'}
'PASS BaseSrv source ownership and negative control'
