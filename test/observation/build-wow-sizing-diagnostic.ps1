[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh worktree build path required'}
New-Item -ItemType Directory $root | Out-Null
$names=@('wow_window_words_binding.c','wow_window_creation_binding.c')
$saved=@{};$patched=@{}
try {
 foreach($name in $names){
  $path=Join-Path $repo "src/wow32-dll/source/$name"
  $saved[$path]=[IO.File]::ReadAllBytes($path)
  [IO.File]::WriteAllBytes((Join-Path $root $name),$saved[$path])
  $s=[Text.Encoding]::UTF8.GetString($saved[$path])
  $include='#include "'+($PSScriptRoot.Replace('\','/')+'/wow_window_sizing_diagnostic.h')+'"'
  if(!$s.Contains('#include <stdio.h>')){throw 'stdio insertion unavailable'}
  $s=$s.Replace('#include <stdio.h>',"#include <stdio.h>`n$include")
  if($name -eq 'wow_window_words_binding.c'){
   $needle='*result = DispatchClientMessage(&view, message, wp, lp, procedure);'
   if(([regex]::Matches($s,[regex]::Escape($needle))).Count -ne 2){throw 'Unexpected dispatch sites'}
   $s=$s.Replace($needle,'sizing_witness("before",window,message,lp); '+$needle+' sizing_witness("after",window,message,lp);')
  }else{
   $needle='trace_creation(&scope, result, error);'
   if(!$s.Contains($needle)){throw 'Missing creation trace'}
   $s=$s.Replace($needle,'sizing_witness("created",result,0,(LPARAM)scope.request); '+$needle)
  }
  $patched[$path]=[Text.Encoding]::UTF8.GetBytes($s)
  [IO.File]::WriteAllBytes($path,$patched[$path])
 }
 $start=[DateTime]::UtcNow
 $p=Start-Process cmake.exe -ArgumentList '--build build/.ninja/msvc-x86-release --target wow32' -WorkingDirectory $repo -WindowStyle Hidden -RedirectStandardOutput "$root/build.log" -RedirectStandardError "$root/build.err" -PassThru
 $p.Id | Set-Content "$root/build.pid"
 if(!$p.WaitForExit(900000)){$p.Kill($true);throw 'Build timeout'}
 if($p.ExitCode){throw "Diagnostic build failed $($p.ExitCode)"}
 Copy-Item "$repo/build/.ninja/msvc-x86-release/wow32/wow32.dll" "$root/WOW32.DLL"
 @{kind='diagnostic-only';publishable=$false;started=$start;exit=$p.ExitCode;sha256=(Get-FileHash "$root/WOW32.DLL").Hash}|ConvertTo-Json|Set-Content "$root/manifest.json"
} finally {
 foreach($path in $saved.Keys){
  if($patched.ContainsKey($path) -and ![Linq.Enumerable]::SequenceEqual([byte[]][IO.File]::ReadAllBytes($path),[byte[]]$patched[$path])){throw "Concurrent source change, do not overwrite: $path"}
  [IO.File]::WriteAllBytes($path,$saved[$path])
 }
}
