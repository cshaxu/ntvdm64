[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh worktree build path required'}
New-Item -ItemType Directory $root | Out-Null
$provider="$repo/build/.ninja/msvc-x86-release/wow32"
$normal=(Get-FileHash "$provider/WOW32.dll").Hash
$start=[DateTime]::UtcNow
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-window-sizing"'
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/build.log" -RedirectStandardError "$root/build.err"
try {
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$root/build.owner.json"
 if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'Sizing producer deadline'}
 $p.WaitForExit();$p.ExitCode|Set-Content "$root/build.exit"
 if($p.ExitCode){throw 'Sizing diagnostic build failed'}
 if((Get-FileHash "$provider/WOW32.dll").Hash -ne $normal){throw 'Normal provider changed'}
 Copy-Item "$provider/diagnostics/window-sizing/WOW32.dll" "$root/WOW32.DLL"
 @{kind='diagnostic-only';publishable=$false;started=$start;exit=$p.ExitCode;sha256=(Get-FileHash "$root/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$root/manifest.json"
} finally {$p.Dispose()}
