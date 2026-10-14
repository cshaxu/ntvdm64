param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string]$CompilerWrapper,
 [Parameter(Mandatory)][string]$Game,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null
try {
 & $CompilerWrapper cl.exe /nologo /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root/probe.obj" "/Fe$root/probe.exe" "$PSScriptRoot/ntvwm_carrier_game_test.c" /link user32.lib *> "$root/compile.txt"
 if($LASTEXITCODE){throw 'Game probe build failed'}
 $stage=New-PhysicalRuntimeStage $PackageRoot
 $scope=New-IsolatedPackageScope $stage
 $bin=Get-PackageBinaryRoot $stage
 # Explicit owner-supplied native application only; never copy the user's tree.
 Copy-Item -LiteralPath $Game -Destination "$stage/mysmb64.exe"
 $scope.Paths+= (Join-Path $stage 'mysmb64.exe')
 @{source=$Game;sha256=(Get-FileHash $Game).Hash;worker=(Get-FileHash "$bin/ntvwm.exe").Hash}|ConvertTo-Json|Set-Content "$root/inputs.json"
 & "$root/probe.exe" "$bin/run16.exe" "$stage/mysmb64.exe" "$root/observer.txt"
 if($LASTEXITCODE){throw "Game probe failed $LASTEXITCODE"}
 if((Get-Content "$root/observer.txt" -Raw) -notmatch 'exit=0 hidden=1'){throw 'Esc/hidden completion not proved'}
 '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit.txt";throw}
finally {if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage}}
