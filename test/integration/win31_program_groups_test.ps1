param([Parameter(Mandatory)][string]$InstallRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path -LiteralPath $InstallRoot).Path
$path=Join-Path $root 'PROGMAN.INI'
if(!(Test-Path -LiteralPath $path -PathType Leaf)){throw 'PROGMAN.INI missing'}
$text=[IO.File]::ReadAllText($path,[Text.Encoding]::GetEncoding(1252))
$groups=[regex]::Match($text,'(?ims)^\s*\[groups\]\s*\r?\n(?<body>.*?)(?=^\s*\[|\z)').Groups['body'].Value
$entries=@([regex]::Matches($groups,'(?im)^\s*group\d+\s*=\s*([^\r\n]+)') | ForEach-Object {$_.Groups[1].Value.Trim()})
if(!$entries.Count){throw 'PROGMAN.INI has no program group entries'}
foreach($entry in $entries){
 $name=$entry.Trim('"');$file=if([IO.Path]::IsPathRooted($name)){$name}else{Join-Path $root $name}
 if(!(Test-Path -LiteralPath $file -PathType Leaf)){throw "Missing group file $name"}
}
"PASS $($entries.Count) configured program groups resolve to files"
