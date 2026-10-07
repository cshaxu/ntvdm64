param([string]$InstallRoot,[switch]$ConfigureAfterFailure)
$ErrorActionPreference='Stop'
$work=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'WORK'))
if(Test-Path -LiteralPath $work){throw 'WORK already exists; preserve and move it aside before starting Setup'}
$launcher=Get-Command run16 -CommandType Application -ErrorAction Stop|Select-Object -First 1
$oldLocation=Get-Location
$oldTemp=$env:TEMP;$oldTmp=$env:TMP
$result=1
try {
    & "$PSScriptRoot/configure-launch.ps1" -Mode Setup
    New-Item -ItemType Directory -Path "$work/TEMP" -Force|Out-Null
    $env:TEMP="$work\TEMP";$env:TMP=$env:TEMP
    Set-Location -LiteralPath $work
    & $launcher.Source "$work\SETUP.PIF"
    $result=$LASTEXITCODE
    Set-Location -LiteralPath $oldLocation.Path
    [IO.File]::WriteAllText("$PSScriptRoot/setup-result.txt","run16_exit=$result`r`n")
    $configure=($result -eq 0 -or $ConfigureAfterFailure)
    if(!$InstallRoot) {
        $InstallRoot=Read-Host "Setup returned $result. If installation completed, enter its actual directory; otherwise press Enter to skip"
        $configure=!!$InstallRoot
    }
    if($configure -and $InstallRoot) {
        $destination=[IO.Path]::GetFullPath($InstallRoot).TrimEnd('\')
        if($destination -eq $work -or $destination.StartsWith($work+'\',[StringComparison]::OrdinalIgnoreCase)) {
            throw 'Installation destination cannot be inside temporary WORK'
        }
        & "$PSScriptRoot/configure-launch.ps1" -Mode Installed -InstallRoot $destination
        Write-Host "Use $destination\PATCH\win.cmd"
    } else {
        Write-Warning "Installed profiles were not generated. See setup-result.txt and the manual configuration command in README.txt."
    }
} catch {
    Write-Warning $_.Exception.Message
    $result=1
} finally {
    Set-Location -LiteralPath $oldLocation.Path
    $env:TEMP=$oldTemp;$env:TMP=$oldTmp
    if(Test-Path -LiteralPath $work) {
        # Only the fresh, exact package-owned WORK may be removed. Never follow links.
        $actual=(Get-Item -LiteralPath $work).FullName
        $links=@(Get-Item -LiteralPath $work; Get-ChildItem -LiteralPath $work -Recurse -Force) |
            Where-Object { $_.Attributes -band [IO.FileAttributes]::ReparsePoint }
        if($actual -ne (Join-Path $PSScriptRoot 'WORK') -or $links) {
            throw 'Unsafe WORK cleanup target; preserved for inspection'
        }
        Remove-Item -LiteralPath $actual -Recurse -Force
    }
}
exit $result
