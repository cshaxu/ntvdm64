# Shared test-only reader; the caller builds the native tool through CMD.
function Read-TestLogonEnvironment([string]$Image,[string]$ProcessRecord) {
 $environmentInfo=[Diagnostics.ProcessStartInfo]::new($Image)
 $environmentInfo.UseShellExecute=$false;$environmentInfo.CreateNoWindow=$true
 $environmentInfo.RedirectStandardOutput=$true
 $environmentInfo.StandardOutputEncoding=[Text.Encoding]::Unicode
 $environmentProcess=[Diagnostics.Process]::Start($environmentInfo)
 try {
  $null=$environmentProcess.Handle
  @{pid=$environmentProcess.Id;start=$environmentProcess.StartTime.ToUniversalTime().ToString('o');image=$environmentInfo.FileName}|ConvertTo-Json|Set-Content $ProcessRecord
  $read=$environmentProcess.StandardOutput.ReadToEndAsync()
  if(!$environmentProcess.WaitForExit(10000)){$environmentProcess.Kill();$environmentProcess.WaitForExit();throw 'Environment reader deadline'}
  $environmentProcess.WaitForExit()
  if($environmentProcess.ExitCode){throw 'Current-token environment query failed'}
  $block=$read.GetAwaiter().GetResult()
  if(!$block.EndsWith([string][char]0+[char]0)){throw 'Invalid environment block terminator'}
  $entries=@($block.Split([char]0)|Where-Object {$_.Length})
 } finally {$environmentProcess.Dispose()}
 return $entries
}
