$ErrorActionPreference='Stop'
$repo='O:/repos.hobby/ntvdm64';$root="$repo/build/t473-s10/latest-source-release-r3"
& "$repo/tests/component-integration/szdd_roundtrip_test.ps1" -BuildRoot build/t473-s10/szdd-roundtrip-r4
if($LASTEXITCODE){throw 'SZDD test failed'}
foreach($name in @('SZDD.EXE','W31PATCH.EXE')){
 Copy-Item -LiteralPath "$repo/assets/release/$name" -Destination "O:/winnt/system32/$name" -Force
 if((Get-FileHash "$repo/assets/release/$name").Hash -ne (Get-FileHash "O:/winnt/system32/$name").Hash){throw 'Utility deployment hash mismatch'}
}
'PASS utility installation hashes'
