$ErrorActionPreference='Stop'
. "$PSScriptRoot/typeahead_witness.ps1"
function Page([string]$Command,[int]$Size){
    "Z:\>$Command`n655360 bytes total conventional memory`n655360 bytes available to MS-DOS`n$Size largest executable program size`n24117248 bytes total contiguous extended memory`n0 bytes available contiguous extended memory`n15434752 bytes available XMS memory`nMS-DOS resident in High Memory Area`n"
}
$one=Page 'run16 mem' 42
$two=Page 'mem' 42
$shots=@([pscustomobject]@{Line=3;Text=$one},[pscustomobject]@{Line=5;Text=$two})
$commands=@('run16 mem','mem');$lines=@(2,4)
$proof=@(Assert-TypeaheadWitness '' $shots $commands $lines)
if($proof.Count -ne 2){throw 'Wrong successful witness count'}
$null=Assert-TypeaheadWitness '' @($shots[0],$shots[0],$shots[1]) $commands $lines
$bad=@(
    @{Before=$one;Shots=$shots},
    @{Before='';Shots=@($shots[0])},
    @{Before='';Shots=@($shots[1],$shots[0])},
    @{Before='';Shots=@([pscustomobject]@{Line=3;Text=$one+$one},$shots[1])},
    @{Before='';Shots=@([pscustomobject]@{Line=1;Text=$one},$shots[1])},
    @{Before='';Shots=@($shots[0],[pscustomobject]@{Line=5;Text=$two.Replace('bytes available XMS memory','missing')})},
    @{Before='';Shots=@($shots[0],$shots[1],[pscustomobject]@{Line=6;Text=(Page 'mem' 43)})},
    @{Before='';Shots=@([pscustomobject]@{Line=5;Text=$one},$shots[1])}
)
foreach($case in $bad){
    $rejected=$false
    try {$null=Assert-TypeaheadWitness $case.Before $case.Shots $commands $lines} catch {$rejected=$true}
    if(!$rejected){throw 'Typeahead witness false pass'}
}
'PASS typeahead: identical output, repeated samples; eight stale/missing/reordered/duplicate/premature/output/extra/ambiguous negatives'
