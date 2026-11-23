param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][ValidateSet('window-render','worker-primitives','worker-primitives-high-resolution')][string]$Target,
 [ValidateSet('x86','x64')][string]$Architecture='x86')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $root.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh disposable measurement directory required'}
New-Item -ItemType Directory $root|Out-Null
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" '+$Architecture+' '+$Target+'"'
foreach($phase in @('build','measure')){
 $file=$env:ComSpec;$arguments=$line
 if($phase -eq 'measure'){
  $file="$root/$Target.exe";$arguments=''
  Copy-Item "$repo/build/.ninja/fixtures-$Architecture/$Target.exe" $file
 }
 $options=@{FilePath=$file;WorkingDirectory=$repo;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$root/$phase.stdout";RedirectStandardError="$root/$phase.stderr"}
 if($arguments){$options.ArgumentList=$arguments}
 $p=Start-Process @options
 try {
  $null=$p.Handle
  @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');file=$file;arguments=$arguments;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$root/$phase.owner.json"
  if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'Measurement deadline'}
  $p.WaitForExit();$p.ExitCode|Set-Content "$root/$phase.exit"
  if($p.ExitCode){throw "$phase failed"}
 } finally {$p.Dispose()}
}
Copy-Item "$root/measure.stdout" "$root/results.txt"
Get-Content "$root/results.txt"
