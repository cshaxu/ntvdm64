param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'BuildRoot must be below repository build'
}
New-Item -ItemType Directory -Path $build -ErrorAction Stop | Out-Null
$source=Join-Path $root 'src/mvdm/softpc.new/base/keymouse/keyba.c'
$text=[IO.File]::ReadAllText($source)
$patterns=@{
 'keyboard_device_add.inc'='^LOCAL VOID add_to_6805_buff IFN2.*?^\s*\} /\* end of add_to_6805_buff \*/';
 'keyboard_device_remove.inc'='^static half_word remove_from_6805_buff IFN0.*?^\} /\* end of remove_from_6805_buff \*/';
 'keyboard_device_clear.inc'='^LOCAL VOID clear_buff_6805 IFN0\(\)\r?\n\{.*?^\}';
 'keyboard_device_reset.inc'='^void Reset6805and8042\(void\)\r?\n\{.*?^\}';
 'keyboard_device_mark.inc'='^LOCAL VOID mark_key_codes_6805_buff IFN1.*?^\}';
 'keyboard_host_down.inc'='^GLOBAL VOID host_key_down IFN1\(int,key\).*?(?=^GLOBAL VOID host_key_up IFN1)';
 'keyboard_host_up.inc'='^GLOBAL VOID host_key_up IFN1\(int,key\).*?(?=^#ifdef NTVDM\r?\nGLOBAL VOID RaiseAllDownKeys)';
 'keyboard_device_immediate.inc'='^LOCAL VOID AddTo6805BuffImm IFN1.*?^\}';
 'keyboard_device_translate.inc'='^LOCAL half_word translate_6805_8042 IFN0.*?^\} /\* end of translate_6805_8042 \*/';
 'keyboard_device_output.inc'='^LOCAL VOID do_q_int\(char scancode\)\r?\n\{.*?^\}';
 'keyboard_device_continue.inc'='^GLOBAL VOID continue_output IFN0.*?^\} /\* end of continuous_output \*/';
 'keyboard_device_eoi.inc'='^void KbdEOIHook\(int IrqLine, int CallCount\)\r?\n\{.*?^\}';
 'keyboard_origin_pending.inc'='^GLOBAL unsigned PendingKeyboardHistory\(void\)\r?\n\{.*?^\}';
 'keyboard_device_replay.inc'='^if \(scanning_discontinued && !waiting_for_next_code\).*?^\s*scanning_discontinued=FALSE;\r?\n\s*\}'
}
foreach($entry in $patterns.GetEnumerator()){
    $match=[regex]::Match($text,'(?ms)'+$entry.Value)
    if(!$match.Success){throw ('Original device extraction failed: '+$entry.Key)}
    [IO.File]::WriteAllText((Join-Path $build $entry.Key),$match.Value)
}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$lines=& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && set')
$pathSet=$false
foreach($line in $lines){
    $i=$line.IndexOf('=')
    if($i -le 0){continue}
    $name=$line.Substring(0,$i)
    if($name -ieq 'Path'){
        if($pathSet){continue}
        $pathSet=$true
    }
    Set-Item -LiteralPath ('env:'+$name) -Value $line.Substring($i+1)
}
& cl.exe /nologo /MT /W4 /WX /c (Join-Path $root 'src/ntvdm-exe/softpc/mvdm_keyboard_history.c') "/Fo:$build/origin.obj"
if($LASTEXITCODE){throw 'Origin carrier compilation failed'}
& cl.exe /nologo /MT /W4 /I $build /I (Join-Path $root 'src') (Join-Path $root 'tests/app/keyboard_origin_device_test.c') (Join-Path $build 'origin.obj') "/Fo:$build/test.obj" "/Fe:$build/test.exe"
if($LASTEXITCODE){throw 'Original device fixture compilation failed'}
Get-FileHash -LiteralPath $source,(Join-Path $build 'test.exe') | Format-List
& (Join-Path $build 'test.exe')
if($LASTEXITCODE){throw 'Original device provenance contract failed'}
