[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$BuildRoot)

$ErrorActionPreference='Stop'
$build=[IO.Path]::GetFullPath($BuildRoot)
$cpu=Join-Path $build 'cpu'
$base=Join-Path $cpu 'libcpu.t130.a'
$replacement=Join-Path $cpu 'cpu.o'
$result=Join-Path $cpu 'libcpu.a'
foreach($path in @($base,$replacement)){if(-not(Test-Path -LiteralPath $path -PathType Leaf)){throw "Missing: $path"}}
$lib=(Get-ChildItem 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC' -Recurse -Filter lib.exe | Where-Object {$_.FullName -match 'HostX64\\x86\\lib\.exe$'} | Select-Object -First 1 -ExpandProperty FullName)
if([string]::IsNullOrWhiteSpace($lib)){throw 'MSVC x86 LIB.exe is unavailable.'}
$members=& $lib /nologo /list $base | Where-Object {$_ -match '\.o$'}
if($members.Count -ne 93 -or ($members | Where-Object {$_ -eq 'cpu.o'}).Count -ne 1){throw 'Unexpected T130 CPU archive membership.'}
$work=Join-Path $cpu 't148-members'; New-Item -ItemType Directory -Path $work -Force | Out-Null
foreach($member in $members){if($member -eq 'cpu.o'){continue}; & $lib /nologo ("/extract:$member") ("/out:"+(Join-Path $work $member)) $base; if($LASTEXITCODE -ne 0){throw "Extract failed: $member"}}
$stagedReplacement = Join-Path $work 'cpu.o'
Copy-Item -LiteralPath $replacement -Destination $stagedReplacement -Force
$reverseMembers = @($members)
[array]::Reverse($reverseMembers)
$response=Join-Path $work 't148-members.rsp'; [IO.File]::WriteAllLines($response,$reverseMembers,[Text.UTF8Encoding]::new($false))
Push-Location $work
try { & $lib /nologo ("/out:"+(Join-Path $cpu 'libcpu.a')) '@t148-members.rsp'; if($LASTEXITCODE -ne 0){throw 'Archive repack failed.'} }
finally { Pop-Location }
$actual=& $lib /nologo /list $result | Where-Object {$_ -match '\.o$'}
if((Compare-Object $members $actual).Count -ne 0){throw 'Repacked archive member order or membership differs.'}
Write-Host 'T148 CPU archive packing retained the original member order with one cpu.o replacement.'
