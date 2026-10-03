param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$LogPath)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {throw 'BuildRoot must be below repository build'}
$log=[IO.Path]::GetFullPath($LogPath)
if(!$log.StartsWith('O:\winnt\Logs2\',[StringComparison]::OrdinalIgnoreCase) -and
   !$log.StartsWith('O:\winnt\logs\',[StringComparison]::OrdinalIgnoreCase)) {throw 'Runtime log must use approved observation directory'}
New-Item -ItemType Directory -Force $build | Out-Null
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$envLines=& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && set')
foreach($line in $envLines){$i=$line.IndexOf('=');if($i -gt 0){Set-Item -Path ('env:'+$line.Substring(0,$i)) -Value $line.Substring($i+1)}}
$sources=@('tests/app/console_input_batch_test.c',
 'src/common/console/client.c',
 'src/common/transport/pipe_transfer.c',
 'src/ntvdm-exe/win32/console_client.c','src/ntvdm-exe/win32/console_graphics.c',
 'src/ntvdm-exe/win32/console_bitmap.c','src/opennt-abi/host-compat/console_grid.c',
 'src/ntvdm-exe/session/session.c','src/ntvdm-exe/session/guest_memory_lease.c') |
 ForEach-Object {Join-Path $root $_}
$exe=Join-Path $build 'console-input-batch.exe'
$objects=@()
foreach($source in $sources){
    # Two owners legitimately have console_client.c; never overwrite objects
    # by basename when compiling this standalone fixture outside Ninja.
    $object=Join-Path $build ('unit-'+$objects.Count+'.obj')
    & cl.exe /nologo /c /TC /MT /W4 /Gy ('/I'+(Join-Path $root 'src')) ('/I'+(Join-Path $root 'src/opennt-abi/host-compat/include')) ('/Fo'+$object) $source *> (Join-Path $build ('unit-'+$objects.Count+'.log'))
    if($LASTEXITCODE){throw ('Fixture compile failed: '+$source)}
    $objects+= $object
}
& link.exe /nologo ('/OUT:'+$exe) /OPT:REF @objects user32.lib gdi32.lib *> (Join-Path $build 'build.log')
if($LASTEXITCODE){throw 'Batch fixture build failed; see build.log'}
& $exe *> $log
if($LASTEXITCODE){throw ('Batch fixture failed; see '+$log)}
Get-Content $log
