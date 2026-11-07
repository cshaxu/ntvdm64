$ErrorActionPreference='Stop'
$d='O:/repos.hobby/ntvdm64/build/t479-s6-livecmd'
function Send-Command([string]$text) {
 foreach($ch in $text.ToCharArray()) {
  $key=if($ch -eq '.'){190}else{[int]$ch}
  & "$d/key190.exe" 7048 Default $key 0
  if($LASTEXITCODE){throw 'Owned input failed'}
 }
 & "$d/key190.exe" 7048 Default 13 0
 if($LASTEXITCODE){throw 'Owned Enter failed'}
}
function Receipt([string]$name) {
 $deadline=[DateTime]::UtcNow.AddSeconds(25)
 while(!((Test-Path "O:/tmp/w479m/$name") -and (Get-Content "O:/tmp/w479m/$name" -Raw).Contains('error=0'))) {
  if([DateTime]::UtcNow -ge $deadline){throw "Receipt missing: $name"}
  Start-Sleep -Milliseconds 100
 }
 Copy-Item "O:/tmp/w479m/$name" "$d/$name"
}
try {
 if((Test-Path O:/tmp/w479m/HB190.TXT) -or (Test-Path O:/tmp/w479m/HB191.TXT)){throw 'Existing receipt'}
 Send-Command 'HB172 HB190.TXT'
 Receipt 'HB190.TXT'
 Start-Sleep -Seconds 2
 Send-Command 'HB172 HB191.TXT'
 Start-Sleep -Seconds 3
 if(Test-Path O:/tmp/w479m/HB191.TXT){throw 'Returned Z was lost: unprefixed command executed'}
 & "$d/key190.exe" 7048 Default 27 0
 if($LASTEXITCODE){throw 'Owned Escape failed'}
 Send-Command 'HB172 HB191.TXT'
 Receipt 'HB191.TXT'
 [IO.File]::WriteAllText("$d/handback190.exit",'0')
} catch {$_|Out-String|Write-Error -ErrorAction Continue;[IO.File]::WriteAllText("$d/handback190.exit",'1');exit 1}
