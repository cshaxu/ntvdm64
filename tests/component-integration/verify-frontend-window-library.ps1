param([string]$BuildRoot = 'build/M0-T423/S6/library')
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
$buildPrefix = (Join-Path $repo 'build') + [IO.Path]::DirectorySeparatorChar
if (!$output.StartsWith($buildPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Build output must remain below repository build/'
}
if ($env:VSCMD_ARG_TGT_ARCH -ne 'x86') { throw 'Run from the MSVC x86 developer environment' }
$owner = Join-Path $repo 'src/ntkvm-exe'
$manifest = Get-Content (Join-Path $owner 'nxvm-import.json') -Raw | ConvertFrom-Json
if ((Get-FileHash (Join-Path $owner 'lib/LICENSE.nxvm') -Algorithm SHA256).Hash -ne $manifest.licenseSha256) {
    throw 'Imported license differs from pinned nxvm'
}
$sources = @()
foreach ($row in $manifest.files) {
    if ($row.path -notmatch '^lib/(types|base|kvm-base|kvm-window)/[A-Za-z0-9_./-]+$' -or
        $row.path.Contains('..')) { throw "Invalid imported path: $($row.path)" }
    $file = Join-Path $owner $row.path
    if ((Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash -ne $row.sha256) {
        throw "Imported file differs from pinned nxvm: $($row.path)"
    }
    if ($row.compile) { $sources += $file }
}
New-Item -ItemType Directory -Force -Path $output | Out-Null
& (Join-Path $repo 'tools/build/Generate-FrontendFont.ps1') -OutputFile (Join-Path $output 'native_pc_font.h')
$objects = @()
$sources += Join-Path $owner 'window_frame.c'
$sources += Join-Path $owner 'text_frame.c'
$sources += Join-Path $owner 'window_controller.c'
$sources += Join-Path $owner 'window_input_queue.c'
$sources += Join-Path $owner 'window_keyboard.c'
$sources += Join-Path $repo 'src/ntw32-exe/text_frame.c'
$sources += Join-Path $owner 'console_video.c'
$sources += Join-Path $PSScriptRoot 'frontend_window_library_test.c'
for ($index = 0; $index -lt $sources.Count; ++$index) {
    $object = Join-Path $output "$index.obj"
    & cl.exe /nologo /c /MT /std:c11 /W4 /DWIN32_LEAN_AND_MEAN /D_WIN32_WINNT=0x0601 "/I$owner" "/I$repo/src" "/I$output" "/Fo$object" $sources[$index]
    if ($LASTEXITCODE) { throw "Compilation failed: $($sources[$index])" }
    $objects += $object
}
$exe = Join-Path $output 'frontend-window-library-test.exe'
& link.exe /nologo /machine:x86 "/out:$exe" @objects user32.lib gdi32.lib
if ($LASTEXITCODE) { throw 'Window library link failed' }
& $exe
if ($LASTEXITCODE) { throw 'Window library assertions failed' }
Write-Output 'Retired ConPTY controller fixture is not built here; actual Window input/return is gated by Verify-CommandExitStatus Window17.'
$keyboardObject = Join-Path $output 'window-keyboard.obj'
$keyboardTestObject = Join-Path $output 'keyboard-test.obj'
& cl.exe /nologo /c /MT /std:c11 /W4 /D_WIN32_WINNT=0x0601 "/I$owner" "/Fo$keyboardObject" (Join-Path $owner 'window_keyboard.c')
if ($LASTEXITCODE) { throw 'Window keyboard compilation failed' }
& cl.exe /nologo /c /MT /std:c11 /W4 /D_WIN32_WINNT=0x0601 "/I$owner" "/Fo$keyboardTestObject" (Join-Path $PSScriptRoot 'frontend_window_keyboard_test.c')
if ($LASTEXITCODE) { throw 'Window keyboard fixture compilation failed' }
$keyboardExe = Join-Path $output 'frontend-window-keyboard-test.exe'
& link.exe /nologo /machine:x86 "/out:$keyboardExe" $keyboardObject $keyboardTestObject user32.lib
if ($LASTEXITCODE) { throw 'Window keyboard fixture link failed' }
& $keyboardExe
if ($LASTEXITCODE) { throw 'Window keyboard assertions failed' }
Write-Output "PASS byte-exact imported files: $($manifest.files.Count); selected library x86 units: $(@($manifest.files | Where-Object compile).Count)"
