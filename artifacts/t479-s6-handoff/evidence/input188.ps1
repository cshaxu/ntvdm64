$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$d='O:/repos.hobby/ntvdm64/build/t479-s6-livecmd'
try {
 & cmd.exe /d /c "build\t479-s6-io\input079.cmd"
 if($LASTEXITCODE){throw 'x86 input failed'}
 Copy-Item build/.ninja/msvc-x86-release/Testing/Temporary/LastTest.log "$d/input188-x86.test.log"
 & cmd.exe /d /c "build\t479-s6-io\input080.cmd"
 if($LASTEXITCODE){throw 'x64 input failed'}
 Copy-Item build/.ninja/msvc-x64-release/Testing/Temporary/LastTest.log "$d/input188-x64.test.log"
 & cmd.exe /d /c "build\.ninja\msvc-x86-release\worker\run-ninja-parallel.cmd" ntvdm.exe
 if($LASTEXITCODE){throw 'Worker build failed'}
 & ./src/addon/ntvdm64-386/build.ps1 -OutputRoot "$d/input-driver188" -Wlink O:/repos.hobby/ntvdm64/build/t479-s4-driver/watcom/binnt64/wlink.exe
 [IO.File]::WriteAllText("$d/input188.exit",'0')
} catch {$_ | Out-String | Write-Error -ErrorAction Continue; [IO.File]::WriteAllText("$d/input188.exit",'1');exit 1}