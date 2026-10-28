$ErrorActionPreference='Stop'
$repo='O:/repos.hobby/ntvdm64';Set-Location $repo
$root="$repo/build/t473-s10/addon-renames-r7";New-Item -ItemType Directory $root|Out-Null
$pwsh='C:/Users/neko/.cache/codex-runtimes/codex-primary-runtime/dependencies/native/powershell/pwsh.exe'
$python=(Get-Command python.exe).Source
$receipt=[ordered]@{result='running';started=[DateTime]::UtcNow.ToString('o');source_commit=(&git rev-parse HEAD);runs=@();core_before=@();drivers_before=@();sources=@()}
foreach($r in (Get-Content assets/release/release-manifest.json -Raw|ConvertFrom-Json)){$receipt.core_before+=@{name=$r.Name;sha256=(Get-FileHash "assets/release/$($r.Name)").Hash}}
$prior=Get-Content assets/release/addon-manifest.json -Raw|ConvertFrom-Json
$receipt.drivers_before=@($prior.drivers)
foreach($p in (Get-ChildItem src/addon -File -Recurse|Where-Object Extension -in '.c','.h','.asm','.inc','.CMD','.ps1')){$receipt.sources+=@{path=$p.FullName;sha256=(Get-FileHash $p.FullName).Hash}}
function Run($name,$image,$arguments,$wd=$repo){
 $p=Start-Process $image -ArgumentList $arguments -WorkingDirectory $wd -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 $receipt.runs+=@{name=$name;pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments=$arguments}
 $receipt|ConvertTo-Json -Depth 9|Set-Content "$root/result.json"
 $p.WaitForExit();$p.WaitForExit();$c=$p.ExitCode;$p.Dispose();$receipt.runs[-1].exit=$c
 $receipt|ConvertTo-Json -Depth 9|Set-Content "$root/result.json"
 if($c){throw "$name returned $c"}
}
try{
 foreach($r in @(@{id='pif';file='PIF.EXE'},@{id='hash';file='HASH.EXE'},@{id='grp';file='GRP.EXE'},@{id='lnk';file='LNK.EXE'},@{id='szdd';file='SZDD.EXE'},@{id='patch31';file='patch31.exe'},@{id='path31';file='path31.exe'},@{id='patch95';file='patch95.exe'},@{id='path95';file='path95.exe'})){
  $a='/d /s /c ""'+$repo+'/src/addon/'+$r.id+'/BUILD.CMD" "'+$root+'/'+$r.file+'""'
  Run ('build-'+$r.id) $env:ComSpec $a $root
 }
 Run mouse101 $pwsh ('-NoProfile -File tests/observation/build-win101-mouse-driver.ps1 -BuildRoot "'+$root+'/mouse101"')
 Run mouse31 $pwsh ('-NoProfile -File tests/observation/build-win31-mouse-driver.ps1 -BuildRoot "'+$root+'/mouse31" -Python "'+$python+'"')
 Run compat95 $pwsh ('-NoProfile -File src/addon/compat95/build.ps1 -OutputRoot "'+$root+'/compat95"')
 foreach($pair in @(@('win101','mouse101','MOUSE101.DRV'),@('win31','mouse31','MOUSE31.DRV'),@('win95-compat','compat95','compat95.sys'))){
  $old=@($prior.drivers|Where-Object id -eq $pair[0])[0]
  if((Get-FileHash "$root/$($pair[1])/$($pair[2])").Hash -ne $old.sha256){throw "Driver bytes changed for $($pair[0]); inherited runtime acceptance cannot be reused"}
 }
 & tools/build/Stage-MouseDriverRelease.ps1 -Win101Build "$root/mouse101" -Win31Build "$root/mouse31" -Win31Acceptance verified
 & tools/build/Stage-Compat95Release.ps1 -BuildRoot "$root/compat95" -Acceptance candidate
 # Normalize the newly requested physical lower-case patch95 file name.
 # Its prior published bytes are preserved as evidence before replacement.
 foreach($target in @('assets/release','O:/winnt/system32')){
  $existing=Join-Path $target 'PATCH95.EXE'
  if(Test-Path -LiteralPath $existing){$old=@($prior.utilities|Where-Object id -eq 'patch95')[0];if((Get-FileHash $existing).Hash -ne $old.sha256){throw 'Preserve unknown prior patch95 artifact'};Copy-Item $existing "$root/previous-patch95-$([IO.Path]::GetFileName($target)).exe";Remove-Item -LiteralPath $existing}
 }
 & tools/build/Stage-AddonUtilityRelease.ps1 -PifBuild "$root/PIF.EXE" -HashBuild "$root/HASH.EXE" -GrpBuild "$root/GRP.EXE" -LnkBuild "$root/LNK.EXE" -SzddBuild "$root/SZDD.EXE" -Patch31Build "$root/patch31.exe" -Path31Build "$root/path31.exe" -Patch95Build "$root/patch95.exe" -Path95Build "$root/path95.exe"
 $meta=Get-Content assets/release/addon-manifest.json -Raw|ConvertFrom-Json
 foreach($r in (@($meta.drivers)+@($meta.utilities))){Copy-Item -LiteralPath "assets/release/$($r.file)" -Destination "O:/winnt/system32/$($r.file)" -Force;if((Get-FileHash "O:/winnt/system32/$($r.file)").Hash -ne $r.sha256){throw 'Add-on deployment mismatch'}}
 foreach($name in @('W31PATCH.EXE','W31PATH.EXE','WIN95PATH.EXE')){
  $old=@($prior.utilities|Where-Object file -eq $name);if($old.Count -ne 1){throw 'Old-name ownership record missing'}
  foreach($target in @('assets/release','O:/winnt/system32')){$file=Join-Path $target $name;if(Test-Path -LiteralPath $file){if((Get-FileHash $file).Hash -ne $old[0].sha256){throw 'Preserve unknown retired-name artifact'};Remove-Item -LiteralPath $file}}
 }
 $install="$root/installed";New-Item -ItemType Directory $install|Out-Null
 [IO.File]::WriteAllText("$install/WIN.COM",'fixture, never executed');[IO.File]::WriteAllText("$install/GROUP.DAT",'relative-path marker');[IO.File]::WriteAllText("$install/PROGMAN.INI","[Groups]`r`nGroup1=C:\OLD\GROUP.DAT`r`n")
 Run path-roundtrip $pwsh ('-NoProfile -File tests/component-integration/win31_path_recovery_test.ps1 -InstallRoot "'+$install+'" -Apply tools/win31-path/APPLY.CMD -Unapply tools/win31-path/UNAPPLY.CMD -BuildRoot "'+$root+'/path-runs"')
 Run win101-setup $pwsh '-NoProfile -File tests/component-integration/win101_setup_cmd_test.ps1 -ToolRoot tools/win101-setup -BuildRoot build/w101ar7'
 Run win31-setup $pwsh '-NoProfile -File tests/component-integration/win31_setup_cmd_roundtrip_test.ps1 -OriginalMedia O:/assets/mswindos/ewin31 -BuildRoot build/t473-s10/renamed-w31-media-r7'
 Run win95-command $pwsh ('-NoProfile -File tests/component-integration/win95_launch_command_safety_test.ps1 -OutputRoot "'+$root+'/win95-command"')
 Run win95-install $pwsh ('-NoProfile -File tests/component-integration/win95_launch_install_test.ps1 -OutputRoot "'+$root+'/win95-install"')
 Run win95-menu $pwsh ('-NoProfile -File tests/component-integration/win95_start_menu_template_test.ps1 -OutputRoot "'+$repo+'/build/w95ar7"')
 Run pif-hash $pwsh ('-NoProfile -File tests/component-integration/pif_hash_tool_test.ps1 -Pif "'+$repo+'/assets/release/PIF.EXE" -Hash "'+$repo+'/assets/release/HASH.EXE" -BuildRoot "'+$root+'/pif-hash" -ReleaseRoot "'+$repo+'/assets/release"')
 Run pif-root $pwsh ('-NoProfile -File tests/component-integration/pif_root_update_test.ps1 -Pif "'+$repo+'/assets/release/PIF.EXE" -BuildRoot "'+$root+'/pif-root"')
 Run lnk $python ('-B tests/component-integration/lnk_tool_test.py --lnk "'+$repo+'/assets/release/LNK.EXE" --output "'+$root+'/lnk"')
 if(Test-Path -LiteralPath O:/w31/.git){throw 'Do not copy a Git checkout as a GRP fixture'}
 Run grp $pwsh ('-NoProfile -File tests/component-integration/grp_tool_test.ps1 -Grp "'+$repo+'/assets/release/GRP.EXE" -FixtureRoot O:/w31 -BuildRoot "'+$root+'/grp"')
 Run szdd $pwsh '-NoProfile -File tests/component-integration/szdd_roundtrip_test.ps1 -BuildRoot build/t473-s10/renamed-szdd-r7'
 foreach($r in $receipt.sources){if((Get-FileHash $r.path).Hash -ne $r.sha256){throw 'Source changed during build/test'}}
 foreach($r in $receipt.core_before){if((Get-FileHash "assets/release/$($r.name)").Hash -ne $r.sha256){throw 'Unrelated ten-image package changed'}}
 $receipt.result='pass'
}catch{$receipt.result='fail';$receipt.error=$_.Exception.Message}
$receipt.finished=[DateTime]::UtcNow.ToString('o');$receipt|ConvertTo-Json -Depth 9|Set-Content "$root/result.json"
if($receipt.result -ne 'pass'){throw $receipt.error}
'PASS all 12 add-on component builds, 12 outputs, unchanged three driver bytes and eleven functional tool groups'
