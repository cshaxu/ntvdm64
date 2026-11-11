param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
# Run only through an owned background supervisor. Explicit PDF queue/output.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
try {
 $source="$PSScriptRoot/wow_print_provider_probe.c"
 & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX "/Fo$root/probe.obj" "/Fe$root/probe.exe" $source gdi32.lib *> "$root/compile.txt"
 if($LASTEXITCODE){throw 'Print provider probe compile failed'}
 @($source,"$root/probe.exe","$env:SystemRoot/SysWOW64/gdi32.dll","$env:SystemRoot/SysWOW64/gdi32full.dll")|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 $arguments='"'+$root+'/normal.pdf" "'+$root+'/cancel.pdf"'
 $p=Start-Process "$root/probe.exe" -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/probe.out" -RedirectStandardError "$root/probe.err"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;deadline=$p.StartTime.AddSeconds(30).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/process.json"
 if(!$p.WaitForExit(30000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned print provider timeout; inspect owned PDF job before retry'}
 $p.WaitForExit();$p.Refresh()
 if($p.ExitCode -or (Get-Content "$root/probe.out" -Raw) -notmatch 'PRINT_PROVIDER result=0 calls=[1-9]\d* foreign=0 outside=0 cancellations=[1-9]\d*'){throw 'Print provider callback contract failed'}
 $deadline=[DateTime]::UtcNow.AddSeconds(5)
 do {
  try {$bytes=[IO.File]::ReadAllBytes("$root/normal.pdf");if($bytes.Length -ge 5){break}}catch{}
  Start-Sleep -Milliseconds 50
 }while([DateTime]::UtcNow -lt $deadline)
 if(!$bytes -or [Text.Encoding]::ASCII.GetString($bytes,0,5) -ne '%PDF-'){throw 'Normal document did not produce PDF'}
 if(Test-Path "$root/cancel.pdf"){throw 'Cancelled document unexpectedly produced a PDF'}
 Get-FileHash "$root/normal.pdf"|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/output.json"
 '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
