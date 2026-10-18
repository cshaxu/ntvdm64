[CmdletBinding()]
param([Parameter(Mandatory)][string]$LogonHelper,
 [Parameter(Mandatory)][string]$BuildRoot)
# Invoke as an owned hidden background run. Tests only this process's token.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
 throw 'Fresh build-owned evidence required'
}
$null=New-Item -ItemType Directory $root
try{
 $helper=(Resolve-Path $LogonHelper).Path
 Get-FileHash $PSCommandPath,$helper|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Add-Type -Path $helper
 $rows=@()
 for($round=0;$round -lt 16;$round++){
  $identity=[Security.Principal.WindowsIdentity]::GetCurrent()
  try{
   $token=$identity.Token
   [GC]::Collect();[GC]::WaitForPendingFinalizers();[GC]::Collect()
   $entries=[WowMessageLogonEnvironment]::Read($token)
   if(!$entries.Count -or @($entries|Where-Object {$_.IndexOf('=',1) -lt 1}).Count){
    throw 'Incomplete unfiltered current-token environment'
   }
   $rows+=[pscustomobject]@{round=$round;held=$true;entries=$entries.Count;gc=$true}
  }finally{$identity.Dispose()}
 }
 # Deterministic negative: the identical raw handle is not an ownership
 # transfer. Once its owner is disposed, userenv must reject it, not return
 # an apparently successful default environment. No project runtime is used.
 $identity=[Security.Principal.WindowsIdentity]::GetCurrent()
 $stale=$identity.Token
 $identity.Dispose()
 $rejected=$false
 try{$null=[WowMessageLogonEnvironment]::Read($stale)}catch{
  $cause=$_.Exception
  while($cause.InnerException){$cause=$cause.InnerException}
  if($cause -is [ComponentModel.Win32Exception] -and $cause.NativeErrorCode -eq 6){
   $rejected=$true
  }else{throw}
 }
 if(!$rejected){throw 'Closed-token negative control was not rejected'}
 $rows|ConvertTo-Json|Set-Content "$root/results.json"
 @{heldRounds=16;forcedGc=$true;closedTokenRejected=$true;originalGuestChanged=$false;
   limits='Proves token ownership contract; does not recover the exact GC schedule of a historical failed process'}|
  ConvertTo-Json|Set-Content "$root/result.json"
 foreach($input in Import-Csv "$root/inputs.csv"){
  if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Test input changed'}
 }
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
