$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$result=@{pid=$PID;started=[DateTimeOffset]::UtcNow.ToString('o');exit=1}
try {
 $repo=$PWD.Path
 Push-Location $env:TEMP
 try {
  & cmd /d /c "`"$repo/src/addon/hash/BUILD.CMD`" `"$repo/build/t484-s3-addon/hash copy.exe`""
  if($LASTEXITCODE){throw 'Outside-cwd wrapper failed'}
 } finally {Pop-Location}
 if((Get-FileHash "$PSScriptRoot/hash copy.exe").Hash -ne (Get-FileHash build/.ninja/addon/hash/hash.exe).Hash){throw 'Wrapper copied a different image'}
 & cmd /d /c 'tools\build-addon.cmd unknown-target' 2>$null
 if($LASTEXITCODE -ne 2){throw 'Unknown target accepted'}
 & cmd /d /c "tools\build\Write-AddonReceipt.cmd build\.ninja\addon\hash\hash.exe build\t484-s3-addon\missing-file build\t484-s3-addon\negative-receipt build\.ninja\addon\compat95\source-inputs.json" 2>$null
 if($LASTEXITCODE -ne 1 -or (Test-Path "$PSScriptRoot/negative-receipt.sha256")){throw 'Failed receipt accepted'}
 $rejected=$false
 try {& tools/build/Stage-Compat95Release.ps1 -BuildRoot build/.ninja/addon/compat95 -Acceptance verified}
 catch {if("$_" -notmatch 'Verified publication requires the four-case evidence root'){throw};$rejected=$true}
 if(!$rejected){throw 'Unverified compat95 publication accepted'}
 $images=Get-ChildItem build/.ninja/addon -Recurse -File | Where-Object {$_.Extension -in '.exe','.386','.drv','.sys'}
 $before=@{};foreach($image in $images){$before[$image.FullName]=@((Get-FileHash $image.FullName).Hash,$image.LastWriteTimeUtc.Ticks)}
 & cmd /d /c tools\build-addon.cmd
 if($LASTEXITCODE){throw 'No-op build failed'}
 foreach($image in $images){$now=Get-Item $image.FullName;if((Get-FileHash $now.FullName).Hash -ne $before[$now.FullName][0] -or $now.LastWriteTimeUtc.Ticks -ne $before[$now.FullName][1]){throw "Unexpected rebuilt image: $now"}}
 foreach($id in @('compat95','mouse101','mouse31','insignia','ntvdm64')){
  $dir="build/.ninja/addon/$id";$meta=Get-Content "$dir/build.inputs.json" -Raw|ConvertFrom-Json
  if((Get-FileHash "$dir/$($meta.file)").Hash -ne (Get-Content "$dir/build.sha256" -Raw).Trim()){throw 'Receipt image mismatch'}
  foreach($source in $meta.sources){if((Get-FileHash $source.path).Hash -ne $source.sha256){throw 'Receipt source mismatch'}}
  foreach($tool in $meta.tools){if((Get-FileHash $tool.path).Hash -ne $tool.sha256){throw 'Receipt tool mismatch'}}
 }
 $commands=& ninja -C build/.ninja/addon -t commands all
 if($LASTEXITCODE -or ($commands -match '(?i)(?:^|[\s"\\/])(python(?:3)?|pwsh|powershell|node|cscript|wscript)(?:\.exe)?[\s"]')){throw 'Interpreted addon build command'}
 $result.exit=0;$result.checks=@('outside-cwd wrapper with spaced output','unknown target rejected','failed receipt rejected','compat95 source/hash checked and unverified publication rejected','no-op bytes and mtimes stable','five driver receipts match','addon graph contains no forbidden interpreter')
}catch{$result.error="$_";Write-Output $_}
$result.finished=[DateTimeOffset]::UtcNow.ToString('o')
$result|ConvertTo-Json -Depth 4|Set-Content "$PSScriptRoot/edge-result.json"
exit $result.exit
