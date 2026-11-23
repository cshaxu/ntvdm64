$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$started=[DateTimeOffset]::UtcNow.ToString('o');$code=1
try {
 $rows=@()
 foreach($arch in 'x86','x64') {
  foreach($name in 'base-classifier-path','base-resource','base-vdm-config') {
   $image="build/.ninja/fixtures-$arch/$name.exe"
   $hash=(Get-FileHash $image).Hash;$time=(Get-Item $image).LastWriteTimeUtc.Ticks
   & cmd.exe /d /c tools\build-fixtures.cmd $arch $name
   if($LASTEXITCODE){throw 'Cache build failed'}
   if((Get-FileHash $image).Hash -ne $hash -or (Get-Item $image).LastWriteTimeUtc.Ticks -ne $time){throw 'Cache output changed'}
   $rows+=@{architecture=$arch;target=$name;sha256=$hash;time=$time}
  }
 }
 $rows|ConvertTo-Json|Set-Content "$PSScriptRoot/cache-identity.json"
 $source=Get-Content build/t484-s3-addon/deliver.ps1 -Raw
 $a=$source.IndexOf(' $rows=Get-Content');$b=$source.IndexOf(' $args=@{}')
 & ([scriptblock]::Create($source.Substring($a,$b-$a)))
 & tools/governance/Verify-DocumentationGovernance.ps1
 if($LASTEXITCODE){throw 'Governance failed'}
 & git diff --check
 if($LASTEXITCODE){throw 'Diff check failed'}
 $code=0
} catch {$_|Out-String|Write-Output}
@{pid=$PID;started=$started;finished=[DateTimeOffset]::UtcNow.ToString('o');exit=$code}|ConvertTo-Json|Set-Content "$PSScriptRoot/final-result.json"
exit $code