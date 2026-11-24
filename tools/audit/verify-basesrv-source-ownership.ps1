[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot, [string]$Hook64BuildRoot,
      [string]$NativeBuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
function Read-Graph([string]$Root){
    $edges=@{}
    foreach($line in Get-Content (Join-Path $Root 'build.ninja')){
        if($line -notmatch '^build (.+?):\s+(\S+)\s+(.*)$'){continue}
         $outputs=$Matches[1]; $rule=$Matches[2]; $inputText=$Matches[3]; $parts=$inputText -split '\s+\|\|?\s+'
        $inputs=@($parts[0].Trim() -split '\s+')
        # Real object/archive dependencies of CMake custom link commands are
        # implicit inputs; order-only prerequisites are not linked providers.
        if($rule -eq 'CUSTOM_COMMAND' -and $inputText -match '\s\|\s'){
            $inputs+=@($parts[1].Trim() -split '\s+' | Where-Object {$_ -match '\.(obj|lib)$'})
        }
        $inputs=@($inputs | ForEach-Object {$_ -replace '\\','/' -replace '\$:',':'})
        foreach($output in (($outputs -split '\s+\|\s+')[0] -split '\s+')){
            $edges[($output -replace '\\','/' -replace '\$:',':')]= $inputs
        }
    }
    return $edges
}
function Assert-NoServiceSources([string]$Target,[hashtable]$Graph){
    if(!$Graph.ContainsKey($Target)){throw "Missing product graph: $Target"}
    $pending=[Collections.Generic.Stack[string]]::new()
    $seen=[Collections.Generic.HashSet[string]]::new()
    $pending.Push($Target)
    $sources=0
    while($pending.Count){
        $node=$pending.Pop()
        if(!$seen.Add($node)){continue}
        if($node -match '(^|/)src/ntsrv-exe/'){
            throw "$Target depends on NTSRV-private source: $node"
        }
        if($node -match '\.(c|cpp|cxx|asm)$'){$sources++}
        if($Graph.ContainsKey($node)){foreach($item in $Graph[$node]){$pending.Push($item)}}
    }
    if(!$sources){throw "No source providers reached: $Target"}
    "PASS $Target sources=$sources"
}
$workerPreset=Split-Path (Resolve-Path $BuildRoot).Path -Parent
if(!$NativeBuildRoot){$NativeBuildRoot=Join-Path (Split-Path $workerPreset -Parent) 'msvc-x64-release'}
if(!$Hook64BuildRoot){$Hook64BuildRoot=Join-Path $NativeBuildRoot 'hook'}
$products=@(
    @($BuildRoot,'ntvdm.exe'),@($BuildRoot,'nthook32.dll'),
    @((Join-Path $workerPreset 'wow32'),'wow32.dll'),
    @((Join-Path $NativeBuildRoot 'launcher'),'run16.exe'),
    @((Join-Path $NativeBuildRoot 'frontend'),'ntcon.exe'),
    @((Join-Path $NativeBuildRoot 'worker'),'ntvwm.exe'),
    @((Join-Path $NativeBuildRoot 'monitor'),'ntmon.exe'),
    @($Hook64BuildRoot,'nthook64.dll'))
foreach($product in $products){
    $graph=Read-Graph $product[0]
    Assert-NoServiceSources $product[1] $graph
    $mutated=$graph.Clone()
    $mutated[$product[1]]=@($graph[$product[1]])+@('injected-private.obj')
    $mutated['injected-private.obj']=@('O:/repo/src/ntsrv-exe/private.c')
    $rejected=$false
    try {Assert-NoServiceSources $product[1] $mutated|Out-Null}
    catch {if($_ -notmatch 'NTSRV-private source'){throw};$rejected=$true}
    if(!$rejected){throw "Private provider negative control accepted: $($product[1])"}
}
foreach($owner in @('lib-nxvm','lib-win32','common','frontend-base','worker-base','run16-exe','ntcon-exe','ntvdm-exe','ntvwm-exe','ntmon-exe','nthook-dll','wow32-dll','vdmredir-dll')){
    foreach($file in Get-ChildItem (Join-Path $repo "src/$owner") -File -Recurse | Where-Object Extension -in '.c','.h','.cpp'){
        if((Get-Content $file.FullName -Raw) -match '#include\s*[<"]ntsrv-exe/'){
            throw "NTSRV-private include in $($file.FullName)"
        }
    }
}
'PASS BaseSrv source ownership and eight negative controls'
