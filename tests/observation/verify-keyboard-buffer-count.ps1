param([Parameter(Mandatory)][string]$BuildRoot,[switch]$HistoryBoundary,[switch]$BiosJoin,[switch]$OriginReturn)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'BuildRoot must be below repository build'
}
New-Item -ItemType Directory -Path $build -ErrorAction Stop | Out-Null
$source=Join-Path $root 'src/mvdm/softpc.new/base/keymouse/keyba.c'
$body=[regex]::Match([IO.File]::ReadAllText($source),'(?ms)^GLOBAL int keys_in_6805_buff\(int \*part_key_transferred\)\r?\n\{.*?^\}')
if(!$body.Success){throw 'Original function extraction failed'}
[IO.File]::WriteAllText((Join-Path $build 'keyboard_buffer_count.inc'),$body.Value)
$fixture='tests/app/keyboard_buffer_count_test.c'
if(@($HistoryBoundary,$BiosJoin,$OriginReturn).Where({$_}).Count -gt 1){throw 'Choose one keyboard fixture'}
$extraObjects=@()
if($OriginReturn){
    $eventText=[IO.File]::ReadAllText((Join-Path $root 'src/mvdm/softpc.new/host/src/nt_event.c'))
    $functions=@{
        'keyboard_origin_init.inc'='void InitKeyHistory';
        'keyboard_origin_update.inc'='void update_key_history';
        'keyboard_origin_get.inc'='int GetHistoryKeyEvent';
        'keyboard_origin_calculate.inc'='int CalcNumberOfUnusedKeyEvents';
        'unused_key_return.inc'='void ReturnUnusedKeyEvents'
    }
    foreach($entry in $functions.GetEnumerator()){
        $match=[regex]::Match($eventText,('(?ms)^'+[regex]::Escape($entry.Value)+'\([^;{]*\)\r?\n\{.*?^\}'))
        if(!$match.Success){throw ('Original origin-return extraction failed: '+$entry.Value)}
        [IO.File]::WriteAllText((Join-Path $build $entry.Key),$match.Value)
    }
    $pending=[regex]::Match([IO.File]::ReadAllText($source),'(?ms)^GLOBAL unsigned PendingKeyboardHistory\(void\)\r?\n\{.*?^\}')
    if(!$pending.Success){throw 'Original pending-origin extraction failed'}
    [IO.File]::WriteAllText((Join-Path $build 'keyboard_origin_pending.inc'),$pending.Value)
    $fixture='tests/app/keyboard_origin_return_test.c'
}
if($BiosJoin){
    $eventSource=Join-Path $root 'src/mvdm/softpc.new/host/src/nt_event.c'
    $text=[IO.File]::ReadAllText($eventSource)
    $returned=[regex]::Match($text,'(?ms)^void ReturnUnusedKeyEvents\(int UnusedKeyEvents\)\r?\n\{.*?^\}')
    $bios=[regex]::Match($text,'(?ms)^VOID ReturnBiosBufferKeys\(VOID\)\r?\n\{.*?^\}')
    if(!$returned.Success -or !$bios.Success){throw 'Original BIOS return extraction failed'}
    [IO.File]::WriteAllText((Join-Path $build 'unused_key_return.inc'),$returned.Value)
    [IO.File]::WriteAllText((Join-Path $build 'keyboard_bios_return.inc'),$bios.Value)
    $fixture='tests/app/keyboard_bios_join_test.c'
}
if($HistoryBoundary){
    $up=[regex]::Match([IO.File]::ReadAllText($source),'(?ms)^GLOBAL VOID host_key_up IFN1\(int,key\).*?(?=^#ifdef NTVDM\r?\nGLOBAL VOID RaiseAllDownKeys)')
    $down=[regex]::Match([IO.File]::ReadAllText($source),'(?ms)^GLOBAL VOID host_key_down IFN1\(int,key\).*?(?=^GLOBAL VOID host_key_up IFN1)')
    $eventSource=Join-Path $root 'src/mvdm/softpc.new/host/src/nt_event.c'
    $returned=[regex]::Match([IO.File]::ReadAllText($eventSource),'(?ms)^void ReturnUnusedKeyEvents\(int UnusedKeyEvents\)\r?\n\{.*?^\}')
    $process=[regex]::Match([IO.File]::ReadAllText($eventSource),'(?ms)^VOID nt_process_keys\(PKEY_EVENT_RECORD KeyEvent\)\r?\n\{.*?^\}')
    if(!$up.Success -or !$down.Success -or !$process.Success -or !$returned.Success){throw 'Original history boundary extraction failed'}
    [IO.File]::WriteAllText((Join-Path $build 'keyboard_host_up.inc'),$up.Value)
    [IO.File]::WriteAllText((Join-Path $build 'keyboard_host_down.inc'),$down.Value)
    [IO.File]::WriteAllText((Join-Path $build 'keyboard_process.inc'),$process.Value)
    [IO.File]::WriteAllText((Join-Path $build 'unused_key_return.inc'),$returned.Value)
    $fixture='tests/app/keyboard_history_boundary_test.c'
}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$lines=& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && set')
foreach($line in $lines){$i=$line.IndexOf('=');if($i -gt 0){Set-Item -LiteralPath ('env:'+$line.Substring(0,$i)) -Value $line.Substring($i+1)}}
if($OriginReturn){
    & cl.exe /nologo /MT /W4 /WX /c (Join-Path $root 'src/ntvdm-exe/softpc/mvdm_keyboard_history.c') "/Fo:$build/origin.obj"
    if($LASTEXITCODE){throw 'Origin carrier compilation failed'}
    $extraObjects+=Join-Path $build 'origin.obj'
}
& cl.exe /nologo /MT /W4 /I $build /I (Join-Path $root 'src') (Join-Path $root $fixture) @extraObjects "/Fo:$build/test.obj" "/Fe:$build/test.exe" user32.lib
if($LASTEXITCODE){throw 'Fixture compilation failed'}
Get-FileHash -LiteralPath $source,(Join-Path $build 'keyboard_buffer_count.inc') | Format-List
& (Join-Path $build 'test.exe')
if($LASTEXITCODE){
    if($OriginReturn){throw 'Original origin-aware history return contract failed'}
    if($BiosJoin){throw 'Original BIOS/hardware return join contract failed'}
    if($HistoryBoundary){throw 'Original keyboard/history preservation contract failed'}
    throw 'Original keyboard-buffer count contract failed'
}
