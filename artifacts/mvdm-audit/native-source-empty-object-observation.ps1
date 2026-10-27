$ErrorActionPreference="Stop"
$driver="O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\wow32\msvc-x86.cmd"
foreach($name in @("c_getset","ccpupig")){
 $object="O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\worker\obj\ccpu\$name.obj"
 & $driver dumpbin.exe /symbols $object *> "O:\repos.hobby\ntvdm64\build\t473-s10\$name-symbols.txt"
 if($LASTEXITCODE){throw "Object observation failed: $name"}
 Get-FileHash -LiteralPath $object|Select-Object Path,Hash|ConvertTo-Json|Set-Content "O:\repos.hobby\ntvdm64\build\t473-s10\$name-object.json"
}
