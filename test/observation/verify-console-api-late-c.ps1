param([string]$BuildRoot='build/t475-s5/p9-fixture',[switch]$ExpectFailure)
$ErrorActionPreference='Stop';$repo=(Resolve-Path "$PSScriptRoot/../..").Path;$root=[IO.Path]::GetFullPath($BuildRoot,$repo);if(Test-Path $root){throw 'Fresh output required'};New-Item -ItemType Directory -Path $root|Out-Null
& "$repo/tools/build-fixtures.cmd" x64 console-api-late-contract *> "$root/build.stdout"
if($LASTEXITCODE){throw 'Late Console fixture build failed'}
$variant=if($ExpectFailure){'fault'}else{'normal'}
$dll="$repo/build/.ninja/fixtures-x64/console-api/$variant/mouse-hook.dll"
for($i=0;$i -lt 4;$i++) {
 $dir=Join-Path $root "run$i/build/M0-T436/S2";New-Item -ItemType Directory -Force $dir|Out-Null;$log=Join-Path $dir 'trace.txt'
 $p=Start-Process "$repo/build/.ninja/fixtures-x64/console-api/ntcon.exe" -ArgumentList @('"'+$dll+'"','"'+$log+'"',$(if($ExpectFailure){'fail'}else{'pass'})) -WorkingDirectory $root -RedirectStandardOutput "$root/run$i.stdout" -RedirectStandardError "$root/run$i.stderr" -WindowStyle Hidden -PassThru;"pid=$($p.Id)"|Set-Content "$root/run$i.process";if(!$p.WaitForExit(60000)){taskkill /PID $p.Id /T /F;throw 'fixture timeout'};$p.Refresh();"exit=$($p.ExitCode)"|Add-Content "$root/run$i.process";if($p.ExitCode){throw "fixture $i failed"}
 $text=Get-Content -Raw $log;if($ExpectFailure){if($text -notmatch 'ATTACH_ERROR .*installed=0' -or $text -match 'api=ATTACHED|api=SetConsole'){throw "run $i did not roll back"}}else{foreach($name in @('api=ATTACHED','SetConsoleWindowInfo','SetConsoleScreenBufferSize','SetConsoleCursorPosition','SetConsoleCursorInfo','WriteConsoleOutputW')){if(!$text.Contains($name)){throw "run $i missing $name"}};if($text.Contains('ATTACH_ERROR')){throw "run $i attach failed"}}
}
if($ExpectFailure){'PASS x64 late observer fault rollback, suppressed input and thread progress in four processes'}else{'PASS x64 late observer actual API errors, logs, thread progress and pinned lifetime in four processes'}
