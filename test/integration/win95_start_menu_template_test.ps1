[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$OutputRoot,
    [string]$Pif='',
    [string]$Lnk=''
)

$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $out)){
    throw 'Fresh build-owned evidence root required'
}
if(!$Pif){$Pif=Join-Path $repo 'assets/release/PIF.EXE'}
if(!$Lnk){$Lnk=Join-Path $repo 'assets/release/LNK.EXE'}
$pif=(Resolve-Path -LiteralPath $Pif).Path
$lnk=(Resolve-Path -LiteralPath $Lnk).Path
function Require([bool]$condition,[string]$message){if(!$condition){throw $message}}
function Refuses([scriptblock]$operation,[string]$message){
    $failed=$false
    try{& $operation|Out-Null}catch{$failed=$true;Require ($_.ToString().Contains($message)) "Unexpected refusal: $_"}
    Require $failed "Expected refusal: $message"
}
function Show-Pif([string]$path){
    $lines=@(& $pif show $path)
    if($LASTEXITCODE){throw "PIF show failed: $path"}
    return $lines
}
function Set-Word([byte[]]$bytes,[int]$offset,[uint16]$value){
    [BitConverter]::GetBytes($value).CopyTo($bytes,$offset)
}

New-Item -ItemType Directory -Path $out|Out-Null
$windows=Join-Path $out 'WINDOWS'
$programs=Join-Path $out 'Program Files'
$menu=Join-Path $windows 'STARTM~1'
New-Item -ItemType Directory -Path $menu,$programs|Out-Null
[IO.File]::WriteAllText((Join-Path $windows 'NOTEPAD.EXE'),'fixture target',[Text.Encoding]::ASCII)

# Build a test-only extended PIF through the released writer, then give it the
# known Win95 prompt profile values at the public PIF record offsets.  It is a
# synthetic fixture, not an installed or guest PIF.
$template=Join-Path $windows 'DOSPRMPT.PIF'
& $pif create $template --title Template --program COMMAND.COM --directory '' --arguments '' --config '' --autoexec ''
if($LASTEXITCODE){throw 'PIF template creation failed'}
$bytes=[IO.File]::ReadAllBytes($template)
Set-Word $bytes 0x20 640; Set-Word $bytes 0x22 128
$w386Offset=[BitConverter]::ToUInt16($bytes,0x199)
Require ($w386Offset -gt 0 -and $w386Offset+4 -le $bytes.Length) 'Synthetic PIF lacks a valid Windows 386 extension'
Set-Word $bytes $w386Offset 65535; Set-Word $bytes ($w386Offset+2) 65535
# The independently authored fixture uses the documented legacy no-checksum
# form after setting record fields directly. PIF.EXE accepts and preserves it.
$bytes[1]=0
[IO.File]::WriteAllBytes($template,$bytes)
$templateBefore=Show-Pif $template
foreach($line in @('STANDARD.MAXMEM_KB=640','STANDARD.MINMEM_KB=128','W386.MAXMEM_KB=65535','W386.MINMEM_KB=65535')){
    Require ($templateBefore -contains $line) "Fixture lacks template field: $line"
}
$templateHash=(Get-FileHash -LiteralPath $template).Hash

$inventory=@()
for($i=1;$i -le 25;$i++){
    $name=('APP{0:D2}.LNK' -f $i);$path=Join-Path $menu $name
    # The PIF-provenance fixture does not exercise Link target growth.  Keep a
    # root-only source target so the independently tested LNK writer is not a
    # second variable in this alternate-WindowsRoot plan test.
    & $lnk create $path --target 'C:\' --directory 'C:\' --codepage 936
    if($LASTEXITCODE){throw "LNK fixture creation failed: $name"}
    $inventory+=@{path='/WINDOWS/STARTM~1/'+$name;long_path='/WINDOWS/Start Menu/'+$name}
}
$menuPif=Join-Path $menu 'MSDOSP~1.PIF'
& $pif create $menuPif --title Source --program COMMAND.COM --directory '' --arguments '' --config '' --autoexec ''
if($LASTEXITCODE){throw 'Source PIF fixture creation failed'}
$menuPifHash=(Get-FileHash -LiteralPath $menuPif).Hash
$inventory+=@{path='/WINDOWS/STARTM~1/MSDOSP~1.PIF';long_path='/WINDOWS/Start Menu/MS-DOS Prompt.PIF'}
$inventoryPath=Join-Path $out 'inventory.json'
$inventory|ConvertTo-Json -Depth 3|Set-Content $inventoryPath -Encoding UTF8

$prepared=Join-Path $out 'prepared'
& "$repo/tools/win95-launch/Prepare-StartMenu.ps1" -OutputRoot $prepared -Inventory $inventoryPath -WindowsRoot $windows -ProgramFilesRoot $programs -Lnk $lnk -Pif $pif
if($LASTEXITCODE){throw 'Start Menu preparation failed'}
$plan=Get-Content "$prepared/plan.json" -Raw|ConvertFrom-Json
$pifRow=@($plan.files|Where-Object {$_.role -eq 'menu' -and $_.relative -match '\.PIF$'})
Require ($pifRow.Count -eq 1) 'Expected exactly one prepared MS-DOS Prompt PIF'
$row=$pifRow[0]
Require ($row.source -eq $menuPif -and $row.sourceSha256 -eq $menuPifHash) 'Lost original menu-source provenance'
Require ($row.templateSource -eq $template -and $row.templateSha256 -eq $templateHash) 'Missing actual template provenance'
Require ((Get-FileHash -LiteralPath $menuPif).Hash -eq $menuPifHash) 'Preparation changed original menu PIF'
Require ((Get-FileHash -LiteralPath $template).Hash -eq $templateHash) 'Preparation changed template PIF'
$candidate=Show-Pif $row.candidate
Require ($candidate -contains 'PROGRAM=O:\WINNT\system32\COMMAND.COM') 'Candidate did not receive dispatch target'
Require ($candidate -contains "DIRECTORY=$windows") 'Candidate did not receive dispatch directory'
$templateStable=@($templateBefore|Where-Object {$_ -notmatch '^(TITLE|PROGRAM|DIRECTORY)='})
$candidateStable=@($candidate|Where-Object {$_ -notmatch '^(TITLE|PROGRAM|DIRECTORY)='})
Require (@(Compare-Object -ReferenceObject $templateStable -DifferenceObject $candidateStable -SyncWindow 0).Count -eq 0) 'Template profile changed outside dispatch fields'
$longWindows=Join-Path $out ('W'*64)
New-Item -ItemType Directory -Path $longWindows,(Join-Path $out 'long-programs')|Out-Null
Copy-Item -LiteralPath $template -Destination (Join-Path $longWindows 'DOSPRMPT.PIF')
Refuses {
    & "$repo/tools/win95-launch/Prepare-StartMenu.ps1" -OutputRoot (Join-Path $out 'long-prepared') -Inventory $inventoryPath -WindowsRoot $longWindows -ProgramFilesRoot (Join-Path $out 'long-programs') -Lnk $lnk -Pif $pif
} '62-byte OEM limit'
'WIN95_START_MENU_TEMPLATE_OK menu_source=1 template_provenance=1 profile_preserved=1 pif_directory_limit=1 installation=0 fixture_only=1'
