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
