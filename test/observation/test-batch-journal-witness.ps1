$ErrorActionPreference='Stop'
. "$PSScriptRoot/batch_journal_witness.ps1"
$expected=@('BEGIN:current','N32:current|bits=32|hook=1','N64:current|bits=64|hook=1','DONE:current')
Assert-BatchJournal $expected $expected
$negatives=@(
 @{Name='missing';Lines=@($expected[0],$expected[1],$expected[3])},
 @{Name='duplicate';Lines=@($expected[0],$expected[1],$expected[1],$expected[2],$expected[3])},
 @{Name='order';Lines=@($expected[0],$expected[2],$expected[1],$expected[3])},
 @{Name='stale';Lines=@($expected|ForEach-Object {$_.Replace('current','old')})},
 @{Name='width';Lines=@($expected[0],$expected[1].Replace('bits=32','bits=64'),$expected[2],$expected[3])},
 @{Name='hook';Lines=@($expected[0],$expected[1].Replace('hook=1','hook=0'),$expected[2],$expected[3])},
 @{Name='extra';Lines=@($expected)+@('DONE:current')}
)
foreach($case in $negatives){$rejected=$false;try{Assert-BatchJournal $case.Lines $expected}catch{$rejected=$true};if(!$rejected){throw "False execution proof accepted: $($case.Name)"}}
'PASS journal witness: exact current sequence and seven false-positive negatives; unit evidence only'
