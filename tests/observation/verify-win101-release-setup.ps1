param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned fixture required'
}
$null=New-Item -ItemType Directory -Path $root
$media=Join-Path $root 'M';$install=Join-Path $root 'I'
$null=New-Item -ItemType Directory -Path $media,$install
$addon=Join-Path $repo 'tools/win101-setup'
$release=Join-Path $repo 'assets/release'
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead((Join-Path $repo 'assets/win101-setup.zip'))
try {
    foreach($entry in $zip.Entries) {
        if($entry.FullName -match '^win101-setup/([^/]+)$' -and $entry.Length) {
            [IO.Compression.ZipFileExtensions]::ExtractToFile($entry,(Join-Path $media $Matches[1]))
        }
    }
} finally {$zip.Dispose()}
$before=@{}
foreach($file in Get-ChildItem -LiteralPath $media -File){$before[$file.Name]=(Get-FileHash $file.FullName).Hash}
& "$addon/apply-setup.ps1" -MediaRoot $media
$patch=Join-Path $media 'PATCH'
if((Get-FileHash "$patch/MOUSE.DRV").Hash -ne (Get-FileHash "$release/MOUSE101.DRV").Hash) {
    throw 'Prepared package did not consume release driver'
}
foreach($name in $before.Keys) {
    if((Get-FileHash (Join-Path $media $name)).Hash -ne $before[$name]){throw 'Original media changed'}
}
if(Test-Path "$patch/WORK"){throw 'Preparation must not ship WORK'}
[IO.File]::WriteAllText("$patch/USER.txt",'keep owner data')
& "$addon/apply-setup.ps1" -MediaRoot $media
if([IO.File]::ReadAllText("$patch/USER.txt") -ne 'keep owner data'){throw 'Unknown file lost'}
$script=[IO.File]::ReadAllBytes("$patch/run-setup.ps1")
[IO.File]::WriteAllText("$patch/run-setup.ps1",'owner modified script')
$failure=$null
try {& "$addon/apply-setup.ps1" -MediaRoot $media}catch{$failure=$_.Exception.Message}
if(!$failure -or !$failure.Contains('Preserve changed PATCH file')){throw 'Changed script was not refused'}
if([IO.File]::ReadAllText("$patch/run-setup.ps1") -ne 'owner modified script'){throw 'Changed script overwritten'}
[IO.File]::WriteAllBytes("$patch/run-setup.ps1",$script)

# Profile-only fixture: these sentinels are NOT installed Windows images and
# are never executed. Assert generated path ownership and copied driver only.
foreach($name in @('WIN.COM','WIN100.BIN','WIN100.OVL')) {
    [IO.File]::WriteAllBytes((Join-Path $install $name),[byte[]]@(1))
}
& "$patch/configure-launch.ps1" -Mode Installed -InstallRoot $install
foreach($name in @('WIN101.PIF','CONFIG.NT','AUTOEXEC.NT','SETVER.EXE','win.cmd','MOUSE.DRV')) {
    if(!(Test-Path (Join-Path "$install/PATCH" $name))){throw "Missing installed PATCH file: $name"}
}
if((Get-FileHash "$install/PATCH/MOUSE.DRV").Hash -ne (Get-FileHash "$release/MOUSE101.DRV").Hash) {
    throw 'Installed driver does not match release'
}
foreach($name in @('WIN.PIF','PATCH/WIN101.PIF','PATCH/CONFIG.NT','PATCH/AUTOEXEC.NT','PATCH/win.cmd')) {
    $text=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes((Join-Path $install $name)))
    if($text.Contains($media) -or $text.Contains('WORK')){throw "Installed package dependency: $name"}
}
if([IO.File]::ReadAllText("$install/PATCH/win.cmd") -notmatch '(?m)^call run16 "%~dp0WIN101\.PIF"') {
    throw 'Launcher must resolve plain run16 and adjacent installed PIF'
}

$bad=Join-Path $root 'BAD';$null=New-Item -ItemType Directory -Path $bad
Copy-Item "$release/MOUSE101.DRV","$release/addon-manifest.json" $bad
[IO.File]::WriteAllBytes("$bad/MOUSE101.DRV",[byte[]]@(0))
$prior=(Get-FileHash "$patch/addon-files.json").Hash;$failure=$null
try {& "$addon/apply-setup.ps1" -MediaRoot $media -ReleaseRoot $bad}catch{$failure=$_.Exception.Message}
if(!$failure -or !$failure.Contains('hash mismatch') -or (Get-FileHash "$patch/addon-files.json").Hash -ne $prior) {
    throw 'Bad driver was not refused before package mutation'
}
[ordered]@{releaseDriver=(Get-FileHash "$release/MOUSE101.DRV").Hash;
    originalMediaUnchanged=$true;repeatApply=$true;unknownFilesPreserved=$true;
    modifiedFilesRejected=$true;badDriverRejected=$true;installedPatchIndependent=$true;
    originalSetupExecuted=$false;guestExecutionProved=$false}|ConvertTo-Json|
    Set-Content (Join-Path $root 'result.json') -Encoding UTF8
'PASS release-driven PATCH preparation, installed profiles and safety negatives; not original Setup/runtime acceptance'
