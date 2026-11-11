param([Parameter(Mandatory)][string]$ReportRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=(Resolve-Path $ReportRoot).Path
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Reports must be under build/'}
$order=@('baseline','candidate','candidate','baseline','baseline','candidate')
$identities=@{};$rows=@()
for($index=0;$index -lt $order.Count;++$index){
    $variant=$order[$index]
    $directory=Join-Path $root "sample-$index-$variant"
    $record=Get-Content (Join-Path $directory 'measurements.json') -Raw|ConvertFrom-Json
    foreach($key in @('Worker','Observer','Hook','Guest','Workload','Profile')){
        if($index -eq 0){$identities[$key]=$record.$key}
        elseif($identities[$key] -ne $record.$key){throw "Comparison identity changed: $key"}
    }
    $case=@($record.Cases|Where-Object {$_.Enabled -and !$_.Warmup})
    if($case.Count -ne 1){throw 'Exactly one measured case per fixed sample required'}
    $text=($case[0].Metrics|ForEach-Object {Get-Content $_ -Raw}) -join "`n"
    foreach($phase in @('frontend-codepage-total','frontend-video-commit','video-transfer')){
        $samples=@([regex]::Matches($text,"(?m)^phase=$phase elapsed-us=\d+ duration-us=(\d+) count=(\d+) error=0\r?$")|
            ForEach-Object {[pscustomobject]@{Duration=[long]$_.Groups[1].Value;Count=[long]$_.Groups[2].Value}})
        if($phase -eq 'frontend-codepage-total'){
            $samples=@($samples|Where-Object Count -gt 0)
            if($variant -eq 'candidate' -and @($samples|Where-Object Count -ne 1).Count){throw 'Candidate did not query once per imported frame'}
            if($variant -eq 'baseline' -and @($samples|Where-Object Count -lt 2000).Count){throw 'Baseline is not the per-cell query implementation'}
        }
        if(!$samples.Count){throw "Missing successful phase: $phase"}
        $sorted=@($samples.Duration|Sort-Object)
        $rows+=[pscustomobject]@{Sample=$index;Variant=$variant;Phase=$phase;Samples=$samples.Count;
            MedianUs=$sorted[[int][Math]::Floor(($sorted.Count-1)/2)];
            P95Us=$sorted[[int][Math]::Ceiling($sorted.Count*0.95)-1];
            MaximumUs=$sorted[-1];TotalCount=(($samples|ForEach-Object Count)|Measure-Object -Sum).Sum;
            CountUnit=$(if($phase -eq 'frontend-codepage-total'){'queries'}else{'frame-bytes'});
            CaseElapsedMs=$case[0].ElapsedMs}
    }
}
[ordered]@{FixedOrder=$order;Identities=$identities;Samples=$rows;
    Limits='Test-only wrappers, injected200-event actual EDIT; no physical/RDP latency or universal speed claim'}|
    ConvertTo-Json -Depth 6|Set-Content (Join-Path $root 'comparison.json')
$rows|Format-Table -AutoSize
