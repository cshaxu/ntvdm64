param([Parameter(Mandatory)][string]$Fixture,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path "$PSScriptRoot/../..").Path
$fixturePath=(Resolve-Path $Fixture).Path
New-Item -ItemType Directory -Force $BuildRoot | Out-Null
$case=(Resolve-Path $BuildRoot).Path
$image=Join-Path $case 'original-classifier.exe'
Copy-Item -LiteralPath $fixturePath -Destination $image
foreach($suffix in @('COM','pif','exe','bin')) {
 [IO.File]::WriteAllBytes((Join-Path $case "suffix.$suffix"),[byte[]](1..64 | ForEach-Object {65}))
}
$media=@('dos/v86/cmd/mem/mem.exe','dos/v86/cmd/command/COMMAND.COM','dos/v86/cmd/qbasic/edit.com','wow16/write/WRITE.EXE') |
 ForEach-Object { (Resolve-Path "$root/src/mvdm-guest/$_").Path }
$p=Start-Process $image -ArgumentList ($media | ForEach-Object {'"'+$_+'"'}) -WorkingDirectory $case -WindowStyle Hidden -PassThru -RedirectStandardOutput "$case/stdout.txt" -RedirectStandardError "$case/stderr.txt"
if(-not $p.WaitForExit(15000)) { $p.Kill(); throw 'Original classifier timed out' }
[ordered]@{fixture=$fixturePath;sha256=(Get-FileHash $image).Hash;pid=$p.Id;exit=$p.ExitCode;media=$media} | ConvertTo-Json | Set-Content "$case/result.json"
Get-Content "$case/stdout.txt"
if($p.ExitCode){throw "Original classifier failed: $($p.ExitCode)"}
