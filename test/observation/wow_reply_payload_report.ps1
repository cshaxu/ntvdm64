# Test-only fixed-width RPLY report; decoding and assertions are separate so
# deliberately failing native controls cannot become successful guest runs.
function Read-WowReplyPayloadReport([string]$Path){
 $bytes=[IO.File]::ReadAllBytes($Path)
 $signature=[Text.Encoding]::ASCII.GetString($bytes,0,[Math]::Min(4,$bytes.Length))
 if(!(($bytes.Length -eq 84 -and $signature -eq 'RPLY') -or ($bytes.Length -eq 104 -and $signature -eq 'TMOP'))){throw 'Invalid early-reply payload report'}
 $names=@('signature','errors','senderThread','receiverThread','accepted','replyFlags','senderReturned','replyResult','protected','unreadable','continued','fullReturn','inputHeader','inputPayload','receiverAlias','oldProtection','pageBytes','senderWindow','receiverWindow','receiverHeader','receiverPayload')
 $row=[ordered]@{}
 if($signature -eq 'TMOP'){$names+=@('mode','sendStatus','sendError','afterFlags','receiverAlive')}
 for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt32($bytes,4*$i)}
 return [pscustomobject]$row
}
function Assert-WowReplyPayloadReport($Report,[switch]$NativeControl,[switch]$Timeout){
 if($Report.errors -or !$Report.senderThread -or !$Report.receiverThread -or
    $Report.senderThread -eq $Report.receiverThread -or $Report.accepted -ne 1 -or
    $Report.senderReturned -ne 1 -or $Report.protected -ne 1 -or
    $Report.unreadable -ne 1 -or $Report.continued -ne 1 -or $Report.fullReturn -ne 1 -or
    !$Report.inputHeader -or $Report.inputPayload -ne $Report.inputHeader+64 -or
    $Report.oldProtection -ne 4 -or $Report.pageBytes -lt 4096 -or
    $Report.inputHeader % $Report.pageBytes -ne 0 -or !$Report.senderWindow -or !$Report.receiverWindow){
  throw 'Early-reply payload assertions failed'
 }
 if($Timeout){
  if($Report.mode -ne 1 -or $Report.sendStatus -ne 0 -or $Report.sendError -ne 1460 -or $Report.receiverAlive -ne 1){throw 'Actual entered/live receiver timeout not proved'}
 }elseif($Report.PSObject.Properties['mode'] -or !($Report.replyFlags -band 8) -or $Report.replyResult -ne 0x447a0051){
  throw 'Early-reply payload assertions failed'
 }
 if($NativeControl){
  if($Report.receiverAlias -notin @(0,1) -or !$Report.receiverHeader -or !$Report.receiverPayload -or
     (($Report.receiverHeader -eq $Report.inputHeader) -ne [bool]$Report.receiverAlias)){
   throw 'Native receiver input observation inconsistent'
  }
 }elseif($Report.receiverAlias -ne [uint32]::MaxValue -or $Report.receiverHeader -or $Report.receiverPayload){
  throw 'Unobserved private WOW capture was fabricated'
 }
}
