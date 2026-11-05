param([Parameter(Mandatory)][string]$Patcher,[Parameter(Mandatory)][string]$Vmm,[Parameter(Mandatory)][string]$Kernel,[Parameter(Mandatory)][string]$Setver,[Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
if(Test-Path -LiteralPath $OutputRoot){throw 'Output root must be fresh'}
New-Item -ItemType Directory -Path $OutputRoot | Out-Null
$inputs=@($Vmm,$Kernel,$Setver);$hashes=@{};foreach($p in $inputs){$hashes[$p]=(Get-FileHash -LiteralPath $p).Hash}
$cases=@(@('prepare',$Vmm,'vmm'),@('prepare-kernel',$Kernel,'kernel'),@('prepare-setver',$Setver,'setver'))
foreach($c in $cases){
 $out=Join-Path $OutputRoot ($c[2]+'.out');$repeat=Join-Path $OutputRoot ($c[2]+'.repeat')
 & $Patcher $c[0] $c[1] $out;if($LASTEXITCODE -ne 0){throw "prepare failed: $($c[2])"}
 & $Patcher $c[0] $out $repeat;if($LASTEXITCODE -ne 0){throw "repeat failed: $($c[2])"}
 if((Get-FileHash $out).Hash -ne (Get-FileHash $repeat).Hash){throw "repeat changed: $($c[2])"}
 $bad=Join-Path $OutputRoot ($c[2]+'.bad');$ErrorActionPreference='Continue';& $Patcher $c[0] $Patcher $bad 2>$null;$ErrorActionPreference='Stop'
 if($LASTEXITCODE -eq 0 -or (Test-Path $bad)){throw "unknown input accepted: $($c[2])"}
}
foreach($p in $inputs){if((Get-FileHash -LiteralPath $p).Hash -ne $hashes[$p]){throw "input modified: $p"}}
Write-Output 'PATCH95_IDENTITY_IDEMPOTENCE_OK'
