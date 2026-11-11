# Test-only observations: never concatenate pages into claimed scroll history.
function Get-TypeaheadMemReports([string]$Text) {
    $plain=$Text -replace '(?m)^\[\d+\]\s?',''
    $pattern='(?ms)^[A-Za-z]:[^\r\n]*>(?<command>run16 mem|mem)\s*\r?\n(?<body>(?:(?!^[A-Za-z]:[^\r\n]*>).)*?)^\s*MS-DOS resident in High Memory Area\s*$'
    foreach($report in [regex]::Matches($plain,$pattern)){
        $body=$report.Groups['body'].Value
        foreach($marker in @('bytes total conventional memory','bytes available to MS-DOS',
            'largest executable program size','bytes total contiguous extended memory',
            'bytes available contiguous extended memory','bytes available XMS memory')){
            if([regex]::Matches($body,[regex]::Escape($marker)).Count -ne 1){
                throw "Missing or duplicate actual MEM output: $marker"
            }
        }
        $size=[regex]::Match($body,'(?m)^\s*(\d+) largest executable program size\s*$')
        if(!$size.Success){throw 'Missing MEM execution identity'}
        $command=$report.Groups['command'].Value.ToLowerInvariant()
        [pscustomobject]@{Command=$command;Size=$size.Groups[1].Value;
            Key=($command+'|'+(($body -replace '\s','')))}
    }
}
function Assert-TypeaheadWitness {
    param([string]$Before,[object[]]$Snapshots,[string[]]$Commands,[int[]]$CommandLines)
    if(!$Commands.Count -or $Commands.Count -ne $CommandLines.Count){throw 'Invalid witness expectations'}
    $stale=[Collections.Generic.HashSet[string]]::new()
    foreach($report in @(Get-TypeaheadMemReports $Before)){[void]$stale.Add($report.Key)}
    $seen=[Collections.Generic.HashSet[string]]::new()
    $witnesses=@()
    foreach($snapshot in $Snapshots){
        $inPage=[Collections.Generic.HashSet[string]]::new()
        foreach($report in @(Get-TypeaheadMemReports $snapshot.Text)){
            if(!$inPage.Add($report.Key)){throw 'Duplicate MEM execution in one page'}
            if($stale.Contains($report.Key)){throw 'Stale pre-input MEM report'}
            if(!$seen.Add($report.Key)){continue}
            $index=$witnesses.Count
            if($index -ge $Commands.Count -or $report.Command -ne $Commands[$index] -or
                $snapshot.Line -lt $CommandLines[$index]){throw 'Extra, premature or reordered MEM execution'}
            # Each selected command must have a complete fresh report before
            # the next MEM command is submitted. This is observation only:
            # no wait or input delay is added to obtain this proof.
            if($index+1 -lt $Commands.Count -and $snapshot.Line -ge $CommandLines[$index+1]){
                throw 'Ambiguous MEM command attribution'
            }
            $witnesses+=[pscustomobject]@{Command=$report.Command;Size=$report.Size;FirstSnapshot=$snapshot.Line}
        }
    }
    if($witnesses.Count -ne $Commands.Count){throw "Missing complete MEM executions: $($witnesses.Count)/$($Commands.Count)"}
    $witnesses
}
