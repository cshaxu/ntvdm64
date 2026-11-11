param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$WindowsRoot,
      [Parameter(Mandatory)][string]$ReferencePif)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory -Path $root|Out-Null
& (Get-Command nasm.exe).Source -f bin "$PSScriptRoot/win31_con_trace.asm" -o "$root/CONTRACE.COM" -l "$root/CONTRACE.lst"
if($LASTEXITCODE){throw 'Independent guest probe assembly failed'}
$tests=Join-Path $PackageRoot 'tests'
[IO.File]::WriteAllText("$root/T436.BAT","@echo off`r`n$tests\CONTRACE.COM`r`n$WindowsRoot\WIN.COM /3`r`nif errorlevel 1 echo WIN_FAILED`r`n$tests\CONTRACE.COM /D`r`necho TRACE_END`r`n",[Text.Encoding]::ASCII)
[IO.File]::WriteAllText("$root/T436BASE.BAT","@echo off`r`n$tests\CONTRACE.COM`r`nver`r`n$tests\CONTRACE.COM /D`r`necho TRACE_END`r`n",[Text.Encoding]::ASCII)
$template=[IO.File]::ReadAllBytes((Resolve-Path $ReferencePif).Path)
foreach($case in @(@('TRACE','T436.BAT'),@('BASE','T436BASE.BAT'))) {
    $pif=[byte[]]$template.Clone()
    function PutString($offset,$size,$value){$bytes=[Text.Encoding]::ASCII.GetBytes($value);if($bytes.Length -ge $size){throw 'PIF overflow'};[Array]::Clear($pif,$offset,$size);[Array]::Copy($bytes,0,$pif,$offset,$bytes.Length)}
    PutString 36 63 "$PackageRoot\system32\COMMAND.COM"
    PutString 165 64 "/c $tests\$($case[1])"
    $pos=369
    while($pos -ne 65535){$sig=[Text.Encoding]::ASCII.GetString($pif,$pos,16).Trim([char]0);if($sig -eq 'WINDOWS 386 3.0'){PutString ([BitConverter]::ToUInt16($pif,$pos+18)+40) 64 "/c $tests\$($case[1])"};$pos=[BitConverter]::ToUInt16($pif,$pos+16)}
    $sum=0;for($i=2;$i -lt 369;$i++){$sum=($sum+$pif[$i]) -band 255};$pif[1]=[byte]$sum
    [IO.File]::WriteAllBytes("$root/$($case[0]).PIF",$pif)
}
'Prepared CCPU40 DOS TSR observation; no original guest modification'
