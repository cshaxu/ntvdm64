param([Parameter(Mandatory)][string]$InstallRoot,
      [Parameter(Mandatory)][string]$OutputDirectory,
      [ValidateSet('Standard','Enhanced')][string]$Mode='Standard',
      [switch]$BootLog,[switch]$DisableIdleDetection)
$ErrorActionPreference='Stop'
$root=(Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
$profile=Join-Path $root 'PATCH'
if(($profile+'\AUTOEXEC.NT').Length -ge 64 -or $root -match '[^\x21-\x7e]|["%&|<>^!]') {
    throw 'Use a short physical DOS-compatible installation path; no drive mapping'
}
foreach($name in @('WIN.COM','SYSTEM\DOSX.EXE','SYSTEM\KRNL386.EXE','WIN31.PIF')) {
    if(!(Test-Path -LiteralPath (Join-Path $root $name))){throw "Missing existing launch input: $name"}
}
$pif=[IO.File]::ReadAllBytes((Join-Path $root 'WIN31.PIF'))
function PutString([int]$offset,[int]$size,[string]$value) {
    $text=[Text.Encoding]::ASCII.GetBytes($value)
    if($text.Length -ge $size -or $offset+$size -gt $pif.Length){throw 'PIF field overflow'}
    [Array]::Clear($pif,$offset,$size);[Array]::Copy($text,0,$pif,$offset,$text.Length)
}
$arguments=if($Mode -eq 'Enhanced'){'/3'}else{'/S'}
if($BootLog){$arguments+=' /B'}
PutString 2 30 "Windows 3.1 $Mode"
PutString 0x24 63 "$root\WIN.COM"
PutString 0x65 64 $root
PutString 0xa5 64 $arguments
$position=0x171;$seen=@{};$nt=$false;$extended=$false
while($position -ne 0xffff) {
    if($seen.ContainsKey($position) -or $position+22 -gt $pif.Length){throw 'Invalid PIF extension chain'}
    $seen[$position]=$true
    $signature=[Text.Encoding]::ASCII.GetString($pif,$position,16).Trim([char]0)
    $data=[BitConverter]::ToUInt16($pif,$position+18)
    $length=[BitConverter]::ToUInt16($pif,$position+20)
    if($data+$length -gt $pif.Length){throw 'Truncated extension'}
    if($signature -eq 'WINDOWS 386 3.0') {
        if($length -lt 104){throw 'Short 386 extension'}
        PutString ($data+40) 64 $arguments;$extended=$true
        if($DisableIdleDetection){
            $flags=[BitConverter]::ToUInt32($pif,$data+16)
            $flags=[uint32]($flags-($flags -band 0x1000)) # original fPollingDetect
            [Array]::Copy([BitConverter]::GetBytes($flags),0,$pif,$data+16,4)
        }
    }
    if($signature -eq 'WINDOWS NT  3.1') {
        if($length -lt 140){throw 'Short NT extension'}
        PutString ($data+12) 64 "$profile\CONFIG.NT"
        PutString ($data+76) 64 "$profile\AUTOEXEC.NT";$nt=$true
    }
    $position=[BitConverter]::ToUInt16($pif,$position+16)
}
if(!$nt -or !$extended){throw 'Missing required reference PIF extensions'}
$sum=0;for($i=2;$i -lt 0x171;$i++){$sum=($sum+$pif[$i]) -band 255};$pif[1]=[byte]$sum
New-Item -ItemType Directory -Path $OutputDirectory -Force|Out-Null
[IO.File]::WriteAllBytes((Join-Path $OutputDirectory 'WIN31.PIF'),$pif)
$config="REM Windows 3.1 standard mode: isolated DOS profile.`r`ndos=high, umb`r`ndevice=%SystemRoot%\system32\himem.sys`r`nfiles=128`r`ndosonly`r`n"
$auto="@echo off`r`nREM Use Windows 3.1 SYSTEM\DOSX.EXE; do not preload NT/WOW DOSX.`r`nlh %SystemRoot%\system32\mscdexnt.exe`r`nlh %SystemRoot%\system32\redir`r`nSET TEMP=$profile\TEMP`r`nSET TMP=$profile\TEMP`r`nSET PATH=$root;$root\SYSTEM;%SystemRoot%\system32`r`n"
[IO.File]::WriteAllText((Join-Path $OutputDirectory 'CONFIG.NT'),$config,[Text.Encoding]::ASCII)
[IO.File]::WriteAllText((Join-Path $OutputDirectory 'AUTOEXEC.NT'),$auto,[Text.Encoding]::ASCII)
Copy-Item -LiteralPath "$PSScriptRoot/win.cmd.template" -Destination (Join-Path $OutputDirectory 'win.cmd') -Force
"Generated isolated $Mode profile; existing guest files unchanged"
