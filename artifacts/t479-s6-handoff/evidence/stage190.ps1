$ErrorActionPreference='Stop'
$r='O:/repos.hobby/ntvdm64';Set-Location $r;$d="$r/build/t479-s6-livecmd";$pkg="$d/package190"
if((Get-Content "$d/input188.exit" -Raw).Trim() -ne '0'){throw 'Candidate build not passed'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -like 'O:\tmp\n40e\*'}).Count){throw 'Prior stage active'}
if(Test-Path $pkg){throw 'Fresh diagnostic package required'}
Copy-Item "$r/build/t479-s6-integration/r091-package" $pkg -Recurse
Copy-Item "$r/build/.ninja/msvc-x86-release/worker/ntvdm.exe" "$pkg/system32/ntvdm.exe"
$rows=@(Get-Content "$pkg/product-system32-manifest.json" -Raw|ConvertFrom-Json)
foreach($row in $rows){$row.Sha256=(Get-FileHash (Join-Path $pkg $row.RelativePath)).Hash;if($row.Name -eq 'ntvdm.exe'){$row.InputSha256=(Get-FileHash "$r/src/ntvdm-exe/softpc/mvdm_vm_executor.c").Hash}}
$rows|ConvertTo-Json|Set-Content "$pkg/product-system32-manifest.json"
. "$r/tests/observation/isolated_package_cleanup.ps1"
$runtime=New-PhysicalRuntimeStage $pkg -FreshPackageIdentity
[ordered]@{runtime=$runtime;scope='Unpublished paired-character diagnostic';utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$d/stage190.json"
Copy-Item O:/tmp/w479m/PATCH/NTVDM64.386 "$d/input-driver188/NTVDM64.before"
Copy-Item "$d/input-driver188/ntvdm64.386" O:/tmp/w479m/PATCH/NTVDM64.386
$leaf=Split-Path $runtime -Leaf
$code=[IO.File]::ReadAllText("$d/key121.c").Replace('ncf8',$leaf)
[IO.File]::WriteAllText("$d/key190.c",$code)
$cmd=[IO.File]::ReadAllText("$d/key115.cmd").Replace('key115','key190')
[IO.File]::WriteAllText("$d/key190.cmd",$cmd)
& cmd.exe /d /c "build\t479-s6-livecmd\key190.cmd"
if($LASTEXITCODE){throw 'Owned probe build failed'}
$root=[IO.File]::ReadAllText("$d/control-root094.ps1").Replace('094','190').Replace('O:\tmp\nb55',$runtime).Replace('O:/tmp/nb55',$runtime.Replace('\','/'))
[IO.Directory]::CreateDirectory("$d/inspect190")|Out-Null
[IO.File]::WriteAllText("$d/control-root190.ps1",$root)
& "$d/control-root190.ps1"