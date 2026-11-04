param(
 [Parameter(Mandatory)][string]$BuildRoot,
 [string]$FormalCache='build/.ninja/t475-handle-compare-p/worker-x86',
 [string]$Baseline='build/.ninja/t475-handle-compare-p/worker-x86/ntvdm.exe'
)
$ErrorActionPreference='Stop'
& "$PSScriptRoot/build-win101-install-guard.ps1" -BuildRoot $BuildRoot -FormalCache $FormalCache -Baseline $Baseline -ProbeSource "$PSScriptRoot/win101_palette_trace.c" -EntrySymbol 'win101_palette_trace_entry'
if($LASTEXITCODE){throw "Private palette trace relink failed: $LASTEXITCODE"}
'PASS private C palette trace relink; no production publication'
