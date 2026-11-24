[CmdletBinding()]
param([string]$NinjaExecutable='ninja')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$native=Join-Path $repo 'build/.ninja'
$products=@(
    @('msvc-x86-release/worker','ntvdm.exe','VCDEX.DLL','VDMREDIR.dll','nthook32.dll'),
    @('msvc-x86-release/wow32','wow32.dll'),
    @('msvc-x64-release/launcher','run16.exe'),
    @('msvc-x64-release/frontend','ntcon.exe'),
    @('msvc-x64-release/worker','ntvwm.exe'),
    @('msvc-x64-release/monitor','ntmon.exe'),
    @('msvc-x64-release/service','ntsrv.exe'),
    @('msvc-x64-release/hook','nthook64.dll'))
$files=@('msvc-x86-release/worker/generated/softpc-embedded-roms.rc',
    'msvc-x86-release/worker/generated/ntvdm-wow32-provider.def')
foreach($product in $products){foreach($target in $product[1..($product.Count-1)]){
    $files+=($product[0]+'/'+$target)
}}
# Keep one cache audit for all production graphs, including intermediate files.
$intermediates=@(foreach($architecture in @('msvc-x86-release','msvc-x64-release')) {
    Get-ChildItem (Join-Path $native $architecture) -Recurse -File |
        Where-Object Extension -in @('.obj','.lib','.res')
})
if(!$intermediates.Count){throw 'Missing production intermediates'}
foreach($file in $intermediates){$files+=$file.FullName.Substring($native.Length+1)}
foreach($product in $products){foreach($target in $product[1..($product.Count-1)]){
    $bytes=[IO.File]::ReadAllBytes((Join-Path $native ($product[0]+'/'+$target)))
    $pe=[BitConverter]::ToInt32($bytes,0x3c)
    $machine=if($product[0].StartsWith('msvc-x86-')){0x14c}else{0x8664}
    if([BitConverter]::ToUInt32($bytes,$pe) -ne 0x4550 -or
        [BitConverter]::ToUInt16($bytes,$pe+4) -ne $machine){throw "Wrong PE architecture: $target"}
}}
$before=@($files | ForEach-Object {
    $path=Join-Path $native $_
    [pscustomobject]@{Path=$path;Hash=(Get-FileHash $path).Hash;Ticks=(Get-Item $path).LastWriteTimeUtc.Ticks}
})
for($iteration=0;$iteration -lt 2;$iteration++){
    # The test drives the supported producer; it owns no compiler/configure recipe.
    & (Join-Path $repo 'tools/build.cmd')
    if($LASTEXITCODE){throw 'CMD production build failed'}
    foreach($entry in $before){
        if((Get-FileHash $entry.Path).Hash -ne $entry.Hash -or
            (Get-Item $entry.Path).LastWriteTimeUtc.Ticks -ne $entry.Ticks){
            throw ('Unchanged input/image was rewritten: '+$entry.Path)
        }
    }
    foreach($product in $products){
        $targets=$product[1..($product.Count-1)]
        $plan=(& $NinjaExecutable -C (Join-Path $native $product[0]) -n @targets 2>&1 | Out-String)
        if($LASTEXITCODE -or $plan -notmatch 'ninja: no work to do\.'){
            throw ('Prebuilt production graph is not a no-op: '+$plan)
        }
    }
}
'PASS 11 PE architectures; two CMD builds preserve ROM/DEF, images and intermediate bytes/timestamps; all eight graphs are no-op'
