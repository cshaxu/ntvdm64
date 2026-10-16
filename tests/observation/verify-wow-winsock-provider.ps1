param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
# Invoke from an owned hidden supervisor. No external hosts or guest changes.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
try {
 $source="$PSScriptRoot/wow_winsock_provider_probe.c"
 & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX "/Fo$root/probe.obj" "/Fe$root/probe.exe" $source ws2_32.lib *> "$root/compile.txt"
 if($LASTEXITCODE){throw 'Provider probe compile failed'}
 @($source,"$root/probe.exe","$env:SystemRoot/SysWOW64/wsock32.dll","$env:SystemRoot/SysWOW64/ws2_32.dll")|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 $p=Start-Process "$root/probe.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/probe.out" -RedirectStandardError "$root/probe.err"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');deadline=$p.StartTime.AddSeconds(20).ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/process.json"
 if(!$p.WaitForExit(20000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned provider timeout'}
 $p.WaitForExit();$p.Refresh()
 if($p.ExitCode -or (Get-Content "$root/probe.err" -Raw) -notmatch 'WINSOCK_PROVIDER result=0 calls=[1-9]\d* foreign=0 outside_blocking=0 cancel=0 '){throw 'Provider callback contract failed'}
 '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
