param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build fixture required'}
$patch=Join-Path $root 'media/PATCH';$bin=Join-Path $root 'bin';$install=Join-Path $root 'W'
New-Item -ItemType Directory -Path $patch,$bin,(Join-Path $install 'SYSTEM')|Out-Null
Copy-Item -LiteralPath "$repo/tools/win31-setup/SETUP.CMD" -Destination "$patch/SETUP.CMD"
Copy-Item -LiteralPath "$repo/assets/release/PIF.EXE" -Destination "$patch/PIF.EXE"
@('@echo off','exit /b 0')|Set-Content "$bin/run16.cmd" -Encoding ascii
$before=@{}
foreach($name in @('WIN.COM','SYSTEM/DOSX.EXE','SYSTEM/KRNL386.EXE','SYSTEM/WIN386.EXE','SYSTEM/MOUSE.DRV','SYSTEM.INI')){
    $file=Join-Path $install $name;[IO.File]::WriteAllText($file,"fixture only: $name");$before[$file]=(Get-FileHash -LiteralPath $file).Hash
}
$oldPath=$env:PATH;$oldNoPause=$env:NTVDM_SETUP_TEST_NO_PAUSE
try{
    $env:PATH="$bin;$oldPath";$env:NTVDM_SETUP_TEST_NO_PAUSE='1'
    foreach($round in 1..2){
        $text=($install|& cmd.exe /d /c "$patch/SETUP.CMD" 2>&1|Out-String)
        if($LASTEXITCODE){throw "Profile generation $round failed: $text"}
    }
    $profiles=Join-Path $install 'PATCH'
    foreach($pair in @(@('WINSTD','/S'),@('WIN386','/3'))){
        $text=(& "$patch/PIF.EXE" show "$profiles/$($pair[0]).PIF"|Out-String)
        if($LASTEXITCODE){throw 'Generated PIF rejected'}
        foreach($value in @("PROGRAM=$install\WIN.COM","DIRECTORY=$install","ARGUMENTS=$($pair[1])",'STANDARD.CLOSE_ON_EXIT=YES','W386.MAX_XMS_KB=65535','W386.FLAGS=0x00001003','NT31.FLAGS=0x00000001')){
            if(!$text.Contains($value)){throw "Wrong generated profile: $value"}
        }
        $launch=[IO.File]::ReadAllText("$profiles/$($pair[0]).CMD")
        if(!$launch.Contains('call run16 "%~dp0'+$pair[0]+'.PIF"')){throw 'Launcher does not use its own PIF'}
    }
    $files=@('CONFIG.NT','AUTOEXEC.NT','WINSTD.PIF','WIN386.PIF','WINSTD.CMD','WIN386.CMD')
    $saved=@{};foreach($name in $files){$saved[$name]=(Get-FileHash "$profiles/$name").Hash}
    [IO.File]::AppendAllText("$profiles/CONFIG.NT","`r`nREM user configuration`r`n")
    $custom=(Get-FileHash "$profiles/CONFIG.NT").Hash
    $text=($install|& cmd.exe /d /c "$patch/SETUP.CMD" 2>&1|Out-String)
    if($LASTEXITCODE -ne 5 -or !$text.Contains('Preserve changed profile')){throw 'Changed profile was not preserved'}
    foreach($name in $files){$expected=if($name -eq 'CONFIG.NT'){$custom}else{$saved[$name]};if((Get-FileHash "$profiles/$name").Hash -ne $expected){throw "Failed preparation changed $name"}}
    if(Test-Path "$profiles/W31PROFILES"){throw 'Profile staging directory remains'}
    foreach($file in $before.Keys){if((Get-FileHash -LiteralPath $file).Hash -ne $before[$file]){throw "Profile-only finalization modified an installed input: $file"}}
    $unexpected = @(Get-ChildItem -LiteralPath $install -File -Recurse | Where-Object {
        !$_.FullName.StartsWith($profiles+'\',[StringComparison]::OrdinalIgnoreCase) -and
        !$before.ContainsKey($_.FullName)
    })
    if($unexpected.Count){throw "Profile-only finalization added an unexpected installed file: $($unexpected[0].FullName)"}
    'PASS Setup profile generation/repeat, legacy PIF fields, conflict preservation and unchanged installed inputs; Setup execution is a test shim'
}finally{$env:PATH=$oldPath;$env:NTVDM_SETUP_TEST_NO_PAUSE=$oldNoPause}
