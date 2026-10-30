$ErrorActionPreference='Stop'
$repo=(Resolve-Path '.').Path
$out="$repo/build/t473-s11/wow-missing-export-provider-reserved"
$provider="$repo/build/.ninja/msvc-x86-release/wow32"
if(Test-Path $out){throw 'fresh output required'}
New-Item -ItemType Directory $out | Out-Null
$def="$provider/wow32-x86.def"
$lines=@(Get-Content $def)
if(@($lines|Where-Object {$_ -match '^\s*W32Init\s*$'}).Count -ne 1){throw 'export definition changed'}
$lines|Where-Object {$_ -notmatch '^\s*W32Init\s*$'}|Set-Content "$out/wow32.def" -Encoding ascii
$old="$repo/build/t473-s11/wow-notification-provider-reserved"
$link=Get-Content "$old/link.cmd" -Raw
$link=$link.Replace("$provider/wow32-x86.def".Replace('\','/'),"$out/wow32.def".Replace('\','/'))
$link=$link.Replace("$old/diagnostic.map","$out/diagnostic.map").Replace("$old/WOW32.DLL","$out/WOW32.DLL").Replace("$old/diagnostic.lib","$out/diagnostic.lib")
if(!$link.Contains("$out/wow32.def".Replace('\','/'))){throw 'definition path not replaced'}
[IO.File]::WriteAllText("$out/link.cmd",$link)
Push-Location $provider
try {
 & cmd.exe /d /c "$out/link.cmd" *> "$out/link.log"
 if($LASTEXITCODE){throw 'link failed'}
} finally {Pop-Location}
Get-FileHash $def,"$out/wow32.def","$out/WOW32.DLL","$old/wow32.obj",$PSCommandPath|Export-Csv -NoTypeInformation "$out/inputs.csv"
