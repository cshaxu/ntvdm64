param([string]$BuildRoot = 'build/M0-T422/S2/pm-entry-contract')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out=[IO.Path]::GetFullPath($BuildRoot, $repo)
if (!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $out)) {
    throw 'Fresh build-owned evidence root required'
}
New-Item -ItemType Directory -Path $out | Out-Null
$source=Join-Path $repo 'src/mvdm-host/dpmi32/i386/dpmi386.c'
$fixture=Join-Path $repo 'test/mvdm-host/dpmi/pm_entry_contract_fixture.c'
& "$repo/tools/build-fixtures.cmd" x86 pm-entry-contract *> "$out/build.log"
if ($LASTEXITCODE) { throw 'Native PM entry fixture build failed' }
$exe=Join-Path $repo 'build/.ninja/msvc-x86-release/worker/pm-entry-contract.exe'
& $exe | Tee-Object -FilePath "$out/result.txt"
if ($LASTEXITCODE) { throw 'PM entry contract failed' }
Get-FileHash $source,$fixture,$exe | Format-List
