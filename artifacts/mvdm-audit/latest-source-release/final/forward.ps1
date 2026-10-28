$ErrorActionPreference='Stop'
$root='O:/repos.hobby/ntvdm64/build/t473-s10/latest-source-release-r3'
foreach($name in @('build','stage-publish','run-full')){
 & "$root/$name.ps1" *> "$root/$name.log"
 if($LASTEXITCODE){throw "$name returned $LASTEXITCODE"}
}
'PASS forward package build, publication and Full suite'
