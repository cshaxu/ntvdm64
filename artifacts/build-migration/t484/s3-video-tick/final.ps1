$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$started=[DateTimeOffset]::UtcNow.ToString('o');$code=1
try {
 $text=(Get-Content src/mvdm-host/softpc.new/host/src/nt_graph.c -Raw).Replace("`r`n","`n")
 $bodies=@()
 foreach($signature in 'static void publish_text_update(void)','void nt_graphics_tick(void)','void nt_flush_screen(void)') {
  $at=$text.IndexOf($signature);$open=$text.IndexOf('{',$at);$depth=1;$end=$open+1
  while($end -lt $text.Length -and $depth){if($text[$end] -eq '{'){$depth++};if($text[$end] -eq '}'){$depth--};$end++}
  if($depth){throw 'Unbalanced original projection'}
  $bodies+=$text.Substring($at,$end-$at)
 }
 $expected=($bodies -join "`n")+"`n"
 foreach($arch in 'x86','x64') {
  $root="build/.ninja/fixtures-$arch"
  if([IO.File]::ReadAllText("$root/blocked_video_tick.inc").Replace("`r`n","`n") -cne $expected){throw 'Function projection differs from original extraction'}
  $hash=(Get-FileHash "$root/blocked-video-tick.exe").Hash;$time=(Get-Item "$root/blocked-video-tick.exe").LastWriteTimeUtc.Ticks
  & cmd.exe /d /c tools\build-fixtures.cmd $arch blocked-video-tick
  if($LASTEXITCODE){throw 'Cache build failed'}
  if((Get-FileHash "$root/blocked-video-tick.exe").Hash -ne $hash -or (Get-Item "$root/blocked-video-tick.exe").LastWriteTimeUtc.Ticks -ne $time){throw 'Cache output changed'}
 }
 & cmd.exe /d /c build\.ninja\msvc-x86-release\worker\run-ninja-parallel.cmd ccpu-bit-fault-test.exe ccpu-bit-matrix-test.exe ccpu-bit-native-oracle.exe
 if($LASTEXITCODE){throw 'Native CCPU fixture build failed'}
 & 'C:/Program Files/Python313/python.exe' -X utf8 test/observation/verify-ccpu-bit-fault.py --build-root build/t484-s3-video-tick/bit-fault
 if($LASTEXITCODE){throw 'CCPU bit fault assertions failed'}
 & 'C:/Program Files/Python313/python.exe' -X utf8 test/observation/verify-ccpu-bit-matrix.py --build-root build/t484-s3-video-tick/bit-matrix
 if($LASTEXITCODE){throw 'CCPU bit matrix assertions failed'}
 $source=Get-Content build/t484-s3-addon/deliver.ps1 -Raw
 $a=$source.IndexOf(' $rows=Get-Content');$b=$source.IndexOf(' $args=@{}')
 & ([scriptblock]::Create($source.Substring($a,$b-$a)))
 $code=0
} catch {$_|Out-String|Write-Output}
@{pid=$PID;started=$started;finished=[DateTimeOffset]::UtcNow.ToString('o');exit=$code}|ConvertTo-Json|Set-Content "$PSScriptRoot/final-result.json"
exit $code