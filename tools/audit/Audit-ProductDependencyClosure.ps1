[CmdletBinding()]
param(
    [string]$BuildRoot='build/M0-T423/S1/restart-formal-x86',
    [string]$WowBuildRoot='build/M0-T423/S1/restart-wow-x86',
    [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$output=[IO.Path]::GetFullPath((Join-Path $repository $OutputRoot))
$prefix=(Join-Path $repository 'build')+[IO.Path]::DirectorySeparatorChar
if(!$output.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){throw 'Output must be under repository build'}
if(Test-Path -LiteralPath $output){throw 'Use a fresh audit output root'}
$null=New-Item -ItemType Directory -Path $output
function Read-Graph([string]$Root) {
    $graph=Join-Path $repository (Join-Path $Root 'build.ninja')
    $edges=@{}
    foreach($line in Get-Content -LiteralPath $graph){
        if($line -notmatch '^build (.+?): (\S+)\s*(.*)$'){continue}
        $outputs=($Matches[1] -split '\s+\|\s+')[0] -split '\s+'
        $inputs=($Matches[3] -split '\s+\|\|?\s+')[0] -split '\s+'
        foreach($name in $outputs){$edges[$name]=$inputs}
    }
    return $edges
}
function Get-Closure($Edges,[string]$Target) {
    if(!$Edges.ContainsKey($Target)){throw "Target missing: $Target"}
    $pending=[Collections.Generic.Stack[string]]::new()
    $seen=[Collections.Generic.HashSet[string]]::new()
    $sources=[Collections.Generic.HashSet[string]]::new()
    $imports=[Collections.Generic.HashSet[string]]::new()
    $archives=[Collections.Generic.HashSet[string]]::new()
    $pending.Push($Target)
    while($pending.Count){
        $node=$pending.Pop()
        if(!$seen.Add($node)){continue}
        foreach($input in $Edges[$node]){
            $normalized=$input.Replace('$:',':')
            $sourcePrefix=$repository.Replace('\','/')+'/src/'
            if($normalized.StartsWith($sourcePrefix,[StringComparison]::OrdinalIgnoreCase) -and
               $normalized -match '\.(c|cpp|asm|rc)$'){
                $null=$sources.Add('src/'+$normalized.Substring($sourcePrefix.Length))
            } elseif($normalized -match '/libvterm-[^/]+/src/[^/]+\.c$'){
                $null=$imports.Add($normalized)
            } elseif($input -match '\.(obj|lib)$' -and $Edges.ContainsKey($input)){
                if($input.EndsWith('.lib')){$null=$archives.Add($input)}
                $pending.Push($input)
            }
        }
    }
    [pscustomobject]@{Target=$Target;Sources=@($sources|Sort-Object);ImportedSources=@($imports|Sort-Object);Archives=@($archives|Sort-Object)}
}
$formal=Read-Graph $BuildRoot
$wow=Read-Graph $WowBuildRoot
$closures=@(foreach($target in 'run16.exe','ntsrv.exe','ntkvm.exe','ntmon.exe','ntvdm.exe','VDMREDIR.dll'){
    Get-Closure $formal $target
})+@(Get-Closure $wow 'wow32.dll')
$closures | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $output 'explicit-closure.json')
$rows=foreach($closure in $closures){
    foreach($group in $closure.Sources | Group-Object {($_ -split '/')[1]}){
        [pscustomobject]@{Target=$closure.Target;Owner=$group.Name;TranslationUnits=$group.Count}
    }
}
$rows | Export-Csv -NoTypeInformation -LiteralPath (Join-Path $output 'owner-counts.csv')
$rows | Format-Table -AutoSize
foreach($closure in $closures){
    $root=if($closure.Target -eq 'wow32.dll'){$WowBuildRoot}else{$BuildRoot}
    $map=Join-Path $repository (Join-Path $root ($closure.Target+'.map'))
    if(!(Test-Path -LiteralPath $map)){continue}
    $objects=@(Get-Content -LiteralPath $map | ForEach-Object {
        if($_ -match '\s+([^\s]+\.obj)\s*$'){$Matches[1]}
    } | Sort-Object -Unique)
    $objects | Set-Content -LiteralPath (Join-Path $output ($closure.Target+'.map-objects.txt'))
}
Write-Output 'Explicit static archive closure only; not proof every archive member survives the linker.'
Write-Output 'Implicit/runtime/order-only prerequisites intentionally excluded. Inspect PE imports/maps separately.'
