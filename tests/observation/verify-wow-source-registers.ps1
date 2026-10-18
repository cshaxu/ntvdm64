[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$ProviderBuildRoot,
    [Parameter(Mandatory)][string]$EvidenceRoot,
    [string]$OriginalRoot='O:/repos.external/OpenNT'
)
# Source/registration inventory, not semantic/runtime acceptance. Invoke as
# an owned background audit; outputs and all retained evidence stay in build/.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$provider=(Resolve-Path -LiteralPath $ProviderBuildRoot).Path
$out=[IO.Path]::GetFullPath($EvidenceRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $out)){throw 'Require fresh build-owned evidence'}
$null=New-Item -ItemType Directory $out
try {
    $pin='5c5b979ec08c17d3ca2eb70e8aad62d26515d01c'
    $revision=& git --no-optional-locks -C $OriginalRoot rev-parse HEAD
    if($LASTEXITCODE){throw 'Cannot read original source revision; no source-identity result'}
    if($revision -ne $pin){throw 'Original source revision changed'}
    $originalChanges=@(& git --no-optional-locks -C $OriginalRoot diff --name-only HEAD)
    if($LASTEXITCODE -or $originalChanges.Count){throw 'Original source has tracked modifications'}
    $selected=@{};$paths=@{}
    foreach($line in Get-Content "$provider/build.ninja"){
        if($line -match '^build (.+): cc (.+)$'){
            $selected[$Matches[1]]=$true
            $paths[$Matches[2].Replace('$:',':').Trim('"')]=$true
        }
    }
    & ninja.exe -C $provider -t deps > "$out/ninja-deps.txt"
    if($LASTEXITCODE){throw 'Cannot read selected dependency ownership'}
    $active=$false
    foreach($line in Get-Content "$out/ninja-deps.txt"){
        if($line -match '^(.+): #deps '){$active=$selected.ContainsKey($Matches[1]);continue}
        if($active -and $line -match '^    (.+)$'){$paths[$Matches[1]]=$true}
    }
    $topology=@{}
    $manifest="$repo/artifacts/documentation-archive/20260910/etc/operations/ledgers/m0-t274-s5-mvdm-host-topology-manifest.tsv"
    foreach($row in Import-Csv $manifest -Delimiter "`t"){$topology[$row.target_path.Replace('\','/')]=$row}
    $registers=@{
        'MVDM-HOST-DIV-'="$repo/src/mvdm/README.md"
        'MVDM-SUPPORT-DIV-'="$repo/src/mvdm/README.md"
        'OPENNT-HOST-'="$repo/src/opennt-host/README.md"
        'ADAPTER-WIN32-'="$repo/src/opennt-abi/host-compat/README.md"
        'ADAPTER-WOW-'="$repo/src/wow32-dll/README.md"
    }
    $rows=@();$issues=@();$single=[Text.Encoding]::GetEncoding(28591)
    foreach($path in $paths.Keys|Sort-Object -Unique){
        $inputPath=if([IO.Path]::IsPathRooted($path)){$path}else{Join-Path $provider $path}
        $absolute=[IO.Path]::GetFullPath($inputPath).Replace('\','/')
        $prefix=$repo.Replace('\','/')+'/'
        if(!$absolute.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){continue}
        $relative=$absolute.Substring($prefix.Length)
        if($relative.StartsWith('src/opennt-host/')){
            $original=$relative.Substring(16)
            if($original.StartsWith('netapi/')){$original='ds/'+$original}
            $source=Join-Path $OriginalRoot $original
        }elseif($relative.StartsWith('src/mvdm/')){
            $original=$relative.Substring(9)
            if(!$topology.ContainsKey($original)){throw "Missing original provenance $relative"}
            $source=$topology[$original].selected_source_path
            if((Get-FileHash $source).Hash -ne $topology[$original].selected_source_sha256){throw "Original identity changed $relative"}
        }else{continue}
        $hash=(Get-FileHash $absolute).Hash;$originalHash=(Get-FileHash $source).Hash
        $current=$single.GetString([IO.File]::ReadAllBytes($absolute))
        $upstream=$single.GetString([IO.File]::ReadAllBytes($source))
        $kind=if($hash -eq $originalHash){'byte-exact'}elseif($current.Replace("`r`n","`n") -ceq $upstream.Replace("`r`n","`n")){'format-only'}else{'content-different'}
        $ids=@();$locations=@();$referenced=@()
        if($kind -eq 'format-only'){$issues+=@{path=$relative;issue='Format-only difference must be restored'}}
        if($kind -eq 'content-different'){
            # Older valid markers use "DIVERGENCE ID:" or put the ID earlier
            # in the same comment. Do not rewrite source to suit one syntax.
            foreach($comment in [regex]::Matches($current,'(?s)/\*.*?\*/')){
                $commentIds=@([regex]::Matches($comment.Value,'(?:MVDM-HOST-DIV-|MVDM-SUPPORT-DIV-|OPENNT-HOST-|ADAPTER-WIN32-|ADAPTER-WOW-)\d{3}')|ForEach-Object Value)
                $referenced+=@($commentIds)
                if(!$comment.Value.Contains('DIVERGENCE')){continue}
                $ids+=@($commentIds)
            }
            $ids=@($ids|Sort-Object -Unique)
            if(!$ids.Count){$issues+=@{path=$relative;issue='No registered local DIVERGENCE identity'}}
            foreach($id in $ids){
                $family=$id.Substring(0,$id.Length-3)
                $register=$registers[$family]
                if((Get-Content $register -Raw) -notmatch ([regex]::Escape($id)+'(?!\d)')){
                    $issues+=@{path=$relative;issue="Missing canonical register $id"}
                }
                $locations+=$register
            }
        }
        $rows+=[pscustomobject]@{path=$relative;source=$source;sha256=$hash;original_sha256=$originalHash;kind=$kind;
            local_ids=($ids -join ';');reference_only_ids=((@($referenced|Sort-Object -Unique|Where-Object {$_ -notin $ids})) -join ';');
            registers=(($locations|Sort-Object -Unique) -join ';')}
    }
    $rows=@($rows|Sort-Object path -Unique)
    $rows|Export-Csv -NoTypeInformation "$out/selected-mirrors.csv"
    ConvertTo-Json -InputObject @($issues) -Depth 4|Set-Content "$out/issues.json"
    $inventory=@("$provider/build.ninja",$manifest)+@($registers.Values)
    Get-FileHash -LiteralPath $inventory|
        Export-Csv -NoTypeInformation "$out/inventory-inputs.csv"
    @{head=(& git -C $repo rev-parse HEAD);original=$pin;compileOwners=$selected.Count;selectedMirrorFiles=$rows.Count;
        groups=@($rows|Group-Object kind|Select-Object Name,Count);issues=$issues.Count;
        limit='Pairing and local/canonical registration coverage only; retained-body and runtime semantics require the fixed-chain source review and acceptance'}|
        ConvertTo-Json -Depth 4|Set-Content "$out/summary.json"
    if(!$rows.Count -or $issues.Count){throw 'Selected source/registration audit has finite recorded issues'}
    '0'|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";'1'|Set-Content "$out/exit.txt";exit 1}
