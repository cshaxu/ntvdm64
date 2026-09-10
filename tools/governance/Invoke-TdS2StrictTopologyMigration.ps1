[CmdletBinding()]
param([string]$RepositoryRoot = '', [switch]$Apply)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path $PSScriptRoot -Parent) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$docs = Join-Path $root 'docs'
$archive = Join-Path $root 'artifacts/documentation-archive/20260910/strict-topology'
$moves = [System.Collections.Generic.List[object]]::new()

function Add-Move([string]$Source, [string]$Destination) {
    if (Test-Path -LiteralPath $Source -PathType Leaf) {
        $item = Get-Item -LiteralPath $Source
        $moves.Add([pscustomobject]@{ Source=$Source; Destination=$Destination; Bytes=$item.Length; Sha256=(Get-FileHash -LiteralPath $Source -Algorithm SHA256).Hash.ToLowerInvariant() })
    }
}
function Get-TaskKey([string]$Name) {
    if ($Name -match '^(m\d+-t\d+)') { return $matches[1].ToLowerInvariant() }
    if ($Name -match '^(m\d+-td)' -or $Name -match '^td-') { return 'm0-td' }
    return $null
}

$allowedDesign = @('GOAL.md','ARCHITECTURE.md','CODING.md','UI.md','ROADMAP.md')
Get-ChildItem (Join-Path $docs 'design') -File | Where-Object { $_.Name -notin $allowedDesign } | ForEach-Object { Add-Move $_.FullName (Join-Path $archive ('design/' + $_.Name)) }
Get-ChildItem (Join-Path $docs 'etc/operations') -File -Recurse | Where-Object { $_.FullName -notmatch '[\\/]policy[\\/]source-policy\.md$' } | ForEach-Object { Add-Move $_.FullName (Join-Path $archive ('etc/operations/' + $_.FullName.Substring((Join-Path $docs 'etc/operations').Length + 1))) }
Get-ChildItem (Join-Path $docs 'history') -File -Filter '*.md' | ForEach-Object { Add-Move $_.FullName (Join-Path $archive ('history/raw/' + $_.Name)) }
Add-Move (Join-Path $docs 'etc/bop-list.md') (Join-Path $archive 'etc/bop-list.md')

if (-not $Apply) { [pscustomobject]@{ Files=$moves.Count; Bytes=($moves|Measure-Object Bytes -Sum).Sum; Archive=$archive } | Format-List; return }
if ((Test-Path -LiteralPath $archive) -and (Get-ChildItem -LiteralPath $archive -Recurse -File -Force | Select-Object -First 1)) { throw "Archive target already exists and is not empty: $archive" }
$map=@{}; foreach($move in $moves){$map[$move.Source]=$move.Destination}
foreach($move in $moves){New-Item -ItemType Directory -Force -Path (Split-Path $move.Destination -Parent)|Out-Null; Move-Item -LiteralPath $move.Source -Destination $move.Destination}

$linkPattern='(?<!\!)\[(?<label>[^\]]+)\]\((?<target>[^)\s]+)(?<suffix>\s+"[^"]*")?\)'
function Rewrite-Links([string]$Text,[string]$Source,[string]$Output) {
    return [regex]::Replace($Text,$linkPattern,{param($match)
        $target=$match.Groups['target'].Value; if($target -match '^(#|[a-z][a-z0-9+.-]*:|//)'){return $match.Value}
        $base=($target -split '[#?]',2)[0]; $fragment=$target.Substring($base.Length)
        $candidate=[IO.Path]::GetFullPath((Join-Path (Split-Path $Source -Parent) $base))
        if(-not $map.ContainsKey($candidate)){return $match.Value}
        $from=[Uri]((Split-Path $Output -Parent).TrimEnd('\\')+'\\'); $to=[Uri]$map[$candidate]
        $relative=[Uri]::UnescapeDataString($from.MakeRelativeUri($to).ToString())
        return "[$($match.Groups['label'].Value)]($relative$fragment$($match.Groups['suffix'].Value))"
    })
}

$groups=@{}
$moves | Where-Object { $_.Source -like (Join-Path $docs 'history/*') } | ForEach-Object { $key=Get-TaskKey ([IO.Path]::GetFileName($_.Source)); if($null -ne $key){if(-not $groups.ContainsKey($key)){$groups[$key]=@()};$groups[$key]+=$_} }
foreach($key in $groups.Keys){
    $output=Join-Path $docs ('history/'+$key+'.md'); $parts=@("# $key historical record",'', 'Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.', '')
    foreach($move in ($groups[$key]|Sort-Object Source)){$raw=[IO.File]::ReadAllText($move.Destination);$parts+="## $([IO.Path]::GetFileName($move.Source))";$parts+='';$parts+=(Rewrite-Links $raw $move.Source $output);$parts+=''}
    [IO.File]::WriteAllText($output,($parts -join "`n"),[Text.UTF8Encoding]::new($false))
}
Get-ChildItem $docs -Recurse -File -Filter '*.md' | ForEach-Object {$text=[IO.File]::ReadAllText($_.FullName);$updated=Rewrite-Links $text $_.FullName $_.FullName;if($updated -ne $text){[IO.File]::WriteAllText($_.FullName,$updated,[Text.UTF8Encoding]::new($false))}}
[IO.File]::WriteAllText((Join-Path $docs 'etc/README.md'),"# Supporting Material`n`nThis directory retains only the owner-approved source policy at [operations/policy/source-policy.md](operations/policy/source-policy.md). Historical supporting material is in the [external documentation archive](../../artifacts/documentation-archive/20260910/strict-topology/).`n",[Text.UTF8Encoding]::new($false))
$rows=@('original_path' + [char]9 + 'archive_path' + [char]9 + 'bytes' + [char]9 + 'sha256') + @($moves|Sort-Object Source|ForEach-Object{"$($_.Source.Substring($root.Length+1).Replace('\\','/'))$([char]9)$($_.Destination.Substring($root.Length+1).Replace('\\','/'))$([char]9)$($_.Bytes)$([char]9)$($_.Sha256)"})
[IO.File]::WriteAllLines((Join-Path $archive 'manifest.tsv'),[string[]]$rows,[Text.UTF8Encoding]::new($false))
Write-Output "Archived $($moves.Count) files; consolidated $($groups.Count) task records."
