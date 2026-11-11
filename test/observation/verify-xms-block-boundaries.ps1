[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,[string]$ProviderSource='src/mvdm-host/xms.486/xmsblock.c')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh worktree build output required'}
New-Item -ItemType Directory $out | Out-Null
$cache=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$wrapper=Join-Path $out 'compiler.cmd'
@('@echo off','call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul','if errorlevel 1 exit /b %errorlevel%','%*') | Set-Content $wrapper -Encoding ASCII
$graph=Get-Content "$cache/build.ninja" -Raw
$m=[regex]::Match($graph,'(?m)^build obj/xms/xmsblock.obj:.*\r?\n\s+cflags = (.+)$')
if(!$m.Success){throw 'Production XMS compile contract absent'}
$flags=$m.Groups[1].Value.Replace('$:',':')
foreach($unit in @(@('provider',$ProviderSource),@('fixture','test/mvdm-host/xms/xms_block_boundary_fixture.c'))){
 $rsp="$out/$($unit[0]).rsp"
 ($flags+' /Gy /Fo"'+$out+'/'+$unit[0]+'.obj" "'+$repo+'/'+$unit[1]+'"') | Set-Content $rsp -Encoding ASCII
 & $wrapper cl "@$rsp" 2>&1 | Out-File "$out/build.txt" -Append
 if($LASTEXITCODE){throw "Compile failed: $($unit[0])"}
}
$exe="$out/xms-boundary.exe"
# Unused move/query dependencies fail fast if accidentally called; no success seams.
$seams=@('mvdm_xms_cancel_current_operation','mvdm_softpc_guest_memory_copy_from','mvdm_softpc_guest_memory_copy_forward','getBP','setDX','getSS','mvdm_softpc_report_xms_move') | ForEach-Object {"/alternatename:_$($_)=_fixture_unexpected"}
& $wrapper link /nologo /OPT:REF @seams "/OUT:$exe" "/MAP:$exe.map" "$out/fixture.obj" "$out/provider.obj" "$cache/original-mvdm-host-suballoc.lib" kernel32.lib ntdll.lib legacy_stdio_definitions.lib 2>&1 | Out-File "$out/build.txt" -Append
if($LASTEXITCODE){throw 'Link failed'}
$map=Get-Content "$exe.map" -Raw
foreach($symbol in @('xmsAllocBlock','xmsReallocBlock','xmsFreeBlock')){if($map -notmatch "_$symbol\s+.*provider.obj"){throw "Missing original owner $symbol"}}
$started=Get-Date
$p=Start-Process $exe -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/result.txt" -RedirectStandardError "$out/error.txt"
if(!$p.WaitForExit(10000)){$p.Kill();throw 'Fixture timeout'}
$result=Get-Content "$out/result.txt" -Raw
@{pid=$p.Id;started=$started.ToString('o');ended=(Get-Date).ToString('o');exit_code=$p.ExitCode;
  provider_sha256=(Get-FileHash (Join-Path $repo $ProviderSource)).Hash;
  executable_sha256=(Get-FileHash $exe).Hash;result=$result.Trim()} | ConvertTo-Json | Set-Content "$out/result.json"
$exitCode=$p.ExitCode
$p.Dispose()
if($exitCode -ne 0 -or $result -notmatch 'assertions=\d+ failures=0'){throw 'XMS fixture failed'}
Get-Content "$out/result.txt"
