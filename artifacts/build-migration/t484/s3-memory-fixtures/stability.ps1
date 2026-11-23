$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$r=@{pid=$PID;started=[DateTimeOffset]::UtcNow.ToString('o');exit=1}
try {
 $before=@{}
 foreach($arch in @('x86','x64')){
  Get-ChildItem "build/.ninja/fixtures-$arch" -File -Filter '*.exe'|ForEach-Object {
   $before[$_.FullName]=@((Get-FileHash $_.FullName).Hash,$_.LastWriteTimeUtc.Ticks)
  }
  & cmd /d /c "tools\build-fixtures.cmd $arch guest-memory"
  if($LASTEXITCODE){throw 'Cached fixture build failed'}
 }
 & cmd /d /c 'tools\build-fixtures.cmd x86 redirector-async'
 if($LASTEXITCODE){throw 'Cached async fixture build failed'}
 foreach($path in $before.Keys){if((Get-FileHash $path).Hash -ne $before[$path][0] -or (Get-Item $path).LastWriteTimeUtc.Ticks -ne $before[$path][1]){throw 'Unexpected binary rebuild'}}
 $projections=@('build/.ninja/msvc-x86-release/worker/guest-memory-coordination.c','build/.ninja/fixtures-x86/guest-memory-coordination.c','build/.ninja/fixtures-x64/guest-memory-coordination.c')
 $hashes=@($projections|ForEach-Object {(Get-FileHash $_).Hash}|Select-Object -Unique)
 if($hashes.Count -ne 1 -or $hashes[0] -ne '85B6B504C334177918B839C8069590D25A29858E7CA597CC96DDF90D73571F81'){throw 'Projection changed'}
 foreach($arch in @('x86','x64')){
  $commands=& ninja -C "build/.ninja/fixtures-$arch" -t commands guest-memory
  if($LASTEXITCODE -or ($commands -match '(?i)(?:^|[\s"\\/])(python(?:3)?|pwsh|powershell|node|cscript|wscript)(?:\.exe)?[\s"]')){throw 'Interpreted fixture command'}
 }
 & cmd /d /c 'tools\build-fixtures.cmd invalid guest-memory'
 if($LASTEXITCODE -ne 2){throw 'Invalid architecture accepted'}
 & cmd /d /c 'tools\build-fixtures.cmd x86 invalid'
 if($LASTEXITCODE -ne 2){throw 'Invalid target accepted'}
 $r.exit=0;$r.binaries=$before.Count;$r.projectionSha256=$hashes[0]
}catch{$r.error="$_";Write-Output $_}
$r.finished=[DateTimeOffset]::UtcNow.ToString('o')
$r|ConvertTo-Json|Set-Content "$PSScriptRoot/stability-result.json"
exit $r.exit
