param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$driver=(Resolve-Path -LiteralPath $CompilerDriver).Path
$null=New-Item -ItemType Directory $root
$source="$repo/src/ntvdm-exe/softpc/mvdm_softpc_termination.c"
$probe="$PSScriptRoot/softpc_trace_no_guest_reads.c"
Push-Location $root
try {
 & $driver cl.exe /nologo /O2 /MT /Gy /W3 /FIwindows.h "/I$repo/src" "/I$repo/src/ntvdm-exe/softpc/include" "/Fo$root/" "/Fe$root/probe.exe" $probe $source /link /OPT:REF *> "$root/compile.txt"
 if($LASTEXITCODE){throw 'Trace fixture compile failed'}
}finally{Pop-Location}
& "$root/probe.exe" "$root/trace.txt" | Tee-Object -FilePath "$root/result.txt"
if($LASTEXITCODE -or !(Select-String -LiteralPath "$root/result.txt" -SimpleMatch 'SOFTPC_TRACE_NO_GUEST_READS_OK')){throw 'Trace fixture failed'}
$trace=Get-Content "$root/trace.txt" -Raw
foreach($marker in @('far-call selector=0008','dpmi-fault vector=0d','source-descriptor-store selector=0008')){
 if(!$trace.Contains($marker)){throw "Missing enabled trace: $marker"}
}
if($trace.Contains('code=')){throw 'Guest instruction bytes unexpectedly sampled'}
@($source,$probe,"$root/probe.exe") | Get-FileHash -Algorithm SHA256 | Export-Csv -NoTypeInformation "$root/inputs.csv"
