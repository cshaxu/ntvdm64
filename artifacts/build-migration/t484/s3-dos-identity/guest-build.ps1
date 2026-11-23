$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$begin=[DateTimeOffset]::UtcNow.ToString('o');$code=1
try {
 & 'C:/Program Files/Python313/python.exe' -X utf8 "$PSScriptRoot/reference.py"
 if($LASTEXITCODE){throw 'Reference composition failed'}
 $results=@()
 foreach($case in @(@{name='dos0';dest='O:\tmp\i310'},@{name='dos1';dest='O:\tmp\i311'},@{name='space';dest='O:\tmp\identity copy'})) {
  $out="$PSScriptRoot/$($case.name)"
  & cmd.exe /d /c tools\build-dos-identity.cmd $out $case.dest
  if($LASTEXITCODE){throw 'Native composition failed'}
  & nasm -f bin "-DW31_INSTALL='$($case.dest)'" "$PSScriptRoot/reference/controller.asm" -o "$PSScriptRoot/reference/controller.exe"
  if($LASTEXITCODE){throw 'Reference controller failed'}
  if((Get-FileHash "$out/controller.exe").Hash -ne (Get-FileHash "$PSScriptRoot/reference/controller.exe").Hash){throw 'Controller bytes differ'}
  foreach($role in 'A','B') {
   & nasm -f bin "-DW31_INSTALL='$($case.dest)'" "-DROLE='$role'" test/observation/dos_identity_child.asm -o "$PSScriptRoot/reference/$role.COM"
   if($LASTEXITCODE){throw 'Reference child failed'}
   if((Get-FileHash "$out/$role.COM").Hash -ne (Get-FileHash "$PSScriptRoot/reference/$role.COM").Hash){throw 'Child bytes differ'}
  }
  $results+=@{case=$case.name;images=3;identical=$true}
 }
 $results|ConvertTo-Json|Set-Content "$PSScriptRoot/byte-identity.json"
 $code=0
} catch {$_|Out-String|Write-Output}
@{pid=$PID;started=$begin;finished=[DateTimeOffset]::UtcNow.ToString('o');exit=$code}|ConvertTo-Json|Set-Content "$PSScriptRoot/guest-build-result.json"
exit $code