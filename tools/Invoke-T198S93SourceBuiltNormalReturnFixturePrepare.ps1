param(
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent $PSScriptRoot }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$config = Join-Path $repository 'artifacts\build\t198-s74-dem-pdb-termination-r1\native-core\config.h'
$profileRoot = Join-Path $repository 'artifacts\inputs\t194-v6-normal-return-root-r1'
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
foreach ($path in @($vsDevCmd, $config, $profileRoot,
    (Join-Path $repository 'tests\runner\t198_s93_profile_select_test.c'),
    (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_fixture.cc'))) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Required S93 input missing: $path" }
}

New-Item -ItemType Directory -Path $build | Out-Null
& (Join-Path $repository 'tools\New-T198S93SourceBuiltNormalReturnFixtureInputs.ps1') -RepositoryRoot $repository -OutputDirectory $build

$profile = @'
{
  "schema":"ntdos64-byob-profile-v8",
  "profile":"nt4-en-us-cli-stream-v8",
  "architecture":"x86","locale":"en-US","compatibility_group":"t198-s93-source-built-normal-return",
  "components":[
    {"role":"ntio","file_name":"NTIO.SYS","required":true,"bytes":33792,"sha256":"cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937","version":null},
    {"role":"ntdos","file_name":"NTDOS.SYS","required":true,"bytes":27858,"sha256":"957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84","version":null},
    {"role":"command","file_name":"COMMAND.COM","required":true,"bytes":50384,"sha256":"908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43","version":null},
    {"role":"target","file_name":"TARGET.EXE","required":true,"bytes":882,"sha256":"69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc","version":null}],
  "features":[],"owner_note":null,
  "guest_command_placement":{"path":"\\COMMAND.COM","drive_index":2},
  "guest_boot_files":{"config":{"path":"\\CONFIG.SYS","materialization":"minimal-comment-v1"},"autoexec":{"path":"\\AUTOEXEC.BAT","materialization":"empty-v1"}},
  "guest_declared_targets":[{"role":"target","placement":{"path":"\\TARGET.EXE","drive_index":2}}],
  "guest_search_metadata":{"command":{"attributes":32,"dos_time":1,"dos_date":33},"target":{"attributes":32,"dos_time":2,"dos_date":34},"config":{"attributes":32,"dos_time":3,"dos_date":35},"autoexec":{"attributes":32,"dos_time":4,"dos_date":36}},
  "guest_display_state":"stream-io-v1",
  "machine_external_initial_state_evidence_sha256":"168219f4c7d73abee06b62b1423298a2f111685e1bf36da23b2c16a642310195"
}
'@
$profilePath = Join-Path $build 'profile-v7.json'
[IO.File]::WriteAllText($profilePath, $profile, [Text.UTF8Encoding]::new($false))
$includes = @('src', 'src\bochs', 'src\bochs\instrument\stubs', 'src\bx-core', 'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\cli') | ForEach-Object { '/I "' + (Join-Path $repository $_) + '"' }
function Invoke-Cl([string]$Arguments, [string]$Log) {
    & cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo ' + $Arguments) 2>&1 | Tee-Object -FilePath $Log -Append
    if ($LASTEXITCODE -ne 0) { throw "S93 compile failed: $Arguments" }
}

$profileExe = Join-Path $build 'profile-select.exe'
$profileArgs = '/TC /std:c11 /W4 /WX /D_CRT_SECURE_NO_WARNINGS /MT ' + ($includes -join ' ') +
    ' /Fe"' + $profileExe + '" "' + (Join-Path $repository 'src\cli\byob_identity.c') + '" "' +
    (Join-Path $repository 'src\cli\byob_image.c') + '" "' + (Join-Path $repository 'src\cli\byob_profile.c') + '" "' +
    (Join-Path $repository 'tests\runner\t198_s93_profile_select_test.c') + '" /link bcrypt.lib'
Invoke-Cl $profileArgs (Join-Path $build 'profile-compile.log')
& $profileExe $profilePath $profileRoot
if ($LASTEXITCODE -ne 0) { throw 'S93 exact v8 profile selection failed.' }

$fixtureObject = Join-Path $build 'source-built-normal-return-fixture.obj'
$fixtureArgs = '/c /std:c++14 /EHsc /MT /Gy /DWIN32 /DBX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1 /DT198_S93_SOURCE_BUILT_NORMAL_RETURN /DT203_S1_PASSIVE_NORMAL_RETURN_OBSERVATION ' + ($includes -join ' ') +
    ' /FI "' + $config + '" /Fo"' + $fixtureObject + '" "' + (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_fixture.cc') + '"'
Invoke-Cl $fixtureArgs (Join-Path $build 'fixture-compile.log')
foreach ($name in @('command_bytes.cc', 'share_bytes.cc')) {
    $object = Join-Path $build ($name -replace '\.cc$', '.obj')
    Invoke-Cl ('/c /std:c++14 /EHsc /MT /Gy /Fo"' + $object + '" "' + (Join-Path $build $name) + '"') (Join-Path $build 'fixture-input-compile.log')
}

@{
    schema = 'ntdos64.t198.s93.source-built-normal-return-fixture-prepare.v1'
    guestExecution = $false
    profile = @{ manifest = $profilePath; root = $profileRoot; selection = 'accepted v7 stream + TARGET.EXE' }
    compiled = @('profile-select.exe', 'source-built-normal-return-fixture.obj', 'command_bytes.obj', 'share_bytes.obj')
    excluded = @('link', 'guest execution', 'provider change', 'Bochs/mantle edit')
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'outcome.json') -Encoding utf8
Write-Host "S93 fixture preparation completed without link or guest execution: $build"
