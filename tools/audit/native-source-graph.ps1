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
        $primaryOutputs=(($outputs -split '\s+\|\s+')[0] -split '\s+')
        $imageOutput=@($primaryOutputs | Where-Object {$_ -match '\.(exe|dll)$'})
        foreach($output in $primaryOutputs){
            # A library emitted by the same link as an image is its import
            # library, not a static copy of that image's implementation.
            $providerInputs=if($imageOutput.Count -and $output -match '\.lib$'){@()}else{$inputs}
            $edges[($output -replace '\\','/' -replace '\$:',':')]= @($providerInputs)
        }
    }
    return $edges
}
function Get-GraphSources([string]$Target,[hashtable]$Graph) {
    if(!$Graph.ContainsKey($Target)){throw "Missing graph target: $Target"}
    $pending=[Collections.Generic.Stack[string]]::new()
    $seen=[Collections.Generic.HashSet[string]]::new()
    $sources=[Collections.Generic.HashSet[string]]::new()
    $pending.Push($Target)
    while($pending.Count){
        $node=$pending.Pop()
        if(!$seen.Add($node)){continue}
        if($node -match '\.(c|cpp|cxx|asm)$'){[void]$sources.Add($node)}
        elseif($Graph.ContainsKey($node)){foreach($input in $Graph[$node]){$pending.Push($input)}}
    }
    if(!$sources.Count){throw "No source providers reached: $Target"}
    return @($sources | Sort-Object)
}
function Get-ProductCompilerDependencies([string]$root,[string]$product,[string]$source) {
    $graph=Read-Graph $root
    if(!$graph.ContainsKey($product)){throw "Missing product target: $product"}
    $selected=[Collections.Generic.HashSet[string]]::new()
    $pending=[Collections.Generic.Stack[string]]::new()
    $pending.Push($product)
    while($pending.Count){
        $node=$pending.Pop()
        if($selected.Add($node) -and $graph.ContainsKey($node)){
            foreach($input in $graph[$node]){$pending.Push($input)}
        }
    }
    $objects=@($selected | Where-Object {
        $_ -match '\.obj$' -and @($graph[$_] | Where-Object {$_ -like "*/$source"}).Count
    })
    if($objects.Count -ne 1){throw "Expected one production compiler for $source; found $($objects.Count)"}
    $target=$objects[0]
    $output=@(& ninja -C $root -t deps $target 2>&1)
    if($LASTEXITCODE -ne 0 -or ($output -join "`n") -notmatch '#deps \d+.*\(VALID\)') {
        throw "Missing compiler dependency data for $target"
    }
    return (($output -join "`n").Replace('\','/'))
}
