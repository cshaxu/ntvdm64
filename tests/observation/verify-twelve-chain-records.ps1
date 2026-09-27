[CmdletBinding()]
param([Parameter(Mandatory)][string]$EvidenceRoot)
$ErrorActionPreference='Stop'
function Assert-FinalSnapshot([string[]]$Lines,[string]$Epoch){
    if(!$Lines.Count -or $Lines[0] -ne "EPOCH $Epoch"){throw 'Missing final snapshot or broker restarted before completion'}
    foreach($line in $Lines | Select-Object -Skip 1){
        if($line -notmatch '^RECORD (\d+) (\d+) (\d+) (\d+) (\d+) (.*)$'){throw 'Invalid final record'}
        # Original READY (8) residency is not an unfinished DOS task.
        # TO_TAKE_A_COMMAND (1) and BUSY (2) must have completed.
        if([int]$Matches[3] -eq 1 -and ([int]$Matches[5] -ne 0 -or [int]$Matches[4] -in @(1,2))){
            throw 'Unfinished DOS record remains after the twelve targets returned'
        }
    }
}
# Parser controls distinguish a legitimate empty/READY snapshot from missing
# evidence, changed broker identity and pending or busy original DOS records.
Assert-FinalSnapshot @('EPOCH 123') '123'
Assert-FinalSnapshot @('EPOCH 123','RECORD 5 0 1 8 0 <EMPTY>') '123'
foreach($negative in @(
    @{Lines=@()}, @{Lines=@('EPOCH 124')},
    @{Lines=@('EPOCH 123','RECORD 5 0 1 1 0 COMMAND.COM')},
    @{Lines=@('EPOCH 123','RECORD 5 0 1 2 1 COMMAND.COM')},
    @{Lines=@('EPOCH 123','RECORD 5 0 1 8 1 COMMAND.COM')},
    @{Lines=@('EPOCH 123','INVALID')})){
    $rejected=$false
    try { Assert-FinalSnapshot $negative.Lines '123' } catch {$rejected=$true}
    if(!$rejected){throw 'Final snapshot negative control was accepted'}
}
$cases=@(Get-Content (Join-Path $EvidenceRoot 'summary.json') -Raw | ConvertFrom-Json)
foreach($case in $cases){
    $parts=$case.Case.Split('-'); $kinds='GGG'+$parts[0]+'GGG'+$parts[1]
    $sequences=@{}; $epoch=$null; $identities=@{}
    foreach($stage in 4..12){
        if($kinds[$stage-1] -ne 'D'){continue}
        $group=if($stage -le 6){1}else{2}
        $start=if($group -eq 1){4}else{10}
        $depth=@($start..$stage | Where-Object {$kinds[$_-1] -eq 'D'}).Count
        foreach($phase in @('ENTER','RETURN')){
            $path=Join-Path $EvidenceRoot "$($case.Case).events.txt.records-$stage-$phase"
            $lines=Get-Content $path
            if($lines[0] -notmatch '^EPOCH ([0-9]+)$'){throw "Missing service epoch: $path"}
            if($null -eq $epoch){$epoch=$Matches[1]}elseif($epoch -ne $Matches[1]){throw 'Broker restarted during chain'}
            $rows=@(foreach($line in $lines | Select-Object -Skip 1){
                if($line -notmatch '^RECORD (\d+) (\d+) (\d+) (\d+) (\d+) (.+)$'){throw "Invalid snapshot row: $line"}
                [pscustomobject]@{Sequence=[uint32]$Matches[1];Task=[uint32]$Matches[2];Kind=[int]$Matches[3];
                    State=[uint32]$Matches[4];Depth=[int]$Matches[5];Image=$Matches[6]}
            })
            $active=@($rows | Where-Object {$_.Kind -eq 1 -and $_.Depth -gt 0})
            if(!$sequences.ContainsKey($group)){
                $new=@($active | Where-Object {$_.Sequence -notin @($sequences.Values)})
                if($new.Count -ne 1){throw 'Cannot identify newly entered character group worker'}
                $sequences[$group]=$new[0].Sequence
            }
            $current=@($active | Where-Object Sequence -eq $sequences[$group])
            if($current.Count -ne 1 -or $current[0].Depth -ne $depth -or
                $current[0].State -ne 2 -or $current[0].Image -notmatch '(?i)\\COMMAND\.COM$'){
                throw "Original DOS record depth/state/image mismatch: $path"
            }
            if($phase -eq 'ENTER'){$identities[$stage]=$current[0].Task}
            elseif($identities[$stage] -ne $current[0].Task){throw 'Parent DOS task identity not restored'}
            # Task zero is an original valid value, not missing evidence.
            if($group -eq 2){
                $first=@($active | Where-Object Sequence -eq $sequences[1])
                $firstDepth=@(4..6 | Where-Object {$kinds[$_-1] -eq 'D'}).Count
                if($first.Count -ne 1 -or $first[0].Depth -ne $firstDepth -or $first[0].State -ne 2){
                    throw 'First group original DOS stack lost while second group executes'
                }
            }
        }
    }
    if($sequences.Count -ne 2 -or $sequences[1] -eq $sequences[2]){throw 'Missing separate original DOS worker identities'}
    $final=@(Get-Content (Join-Path $EvidenceRoot "$($case.Case).events.txt.records-1-RETURN"))
    Assert-FinalSnapshot $final $epoch
    Write-Output "PASS $($case.Case) original DOS identities, nested return, retained first-group stack and no unfinished final DOS task"
}
