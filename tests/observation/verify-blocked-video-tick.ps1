param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerWrapper)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Require a fresh build-only evidence directory'}
$wrapper=(Resolve-Path $CompilerWrapper).Path
New-Item -ItemType Directory $root | Out-Null
$source=Get-Content "$repo/src/mvdm/softpc.new/host/src/nt_graph.c" -Raw
function Extract-Function([string]$Signature) {
    $at=$source.IndexOf($Signature,[StringComparison]::Ordinal)
    if($at -lt 0){throw "Missing production function: $Signature"}
    $open=$source.IndexOf('{',$at);$depth=1;$end=$open+1
    # These two source bodies have no quoted/comment braces. Fail rather than
    # accepting a changed function spelling or silently testing a stale copy.
    while($end -lt $source.Length -and $depth){if($source[$end] -eq '{'){$depth++};if($source[$end] -eq '}'){$depth--};$end++}
    if($depth){throw 'Unbalanced production function'}
    $source.Substring($at,$end-$at)
}
$body=(Extract-Function 'static void publish_text_update(void)')+"`r`n"+(Extract-Function 'void nt_graphics_tick(void)')
[IO.File]::WriteAllText("$root/blocked_video_tick.inc",$body,[Text.Encoding]::ASCII)
@("$repo/src/mvdm/softpc.new/host/src/nt_graph.c","$repo/tests/component-integration/blocked_video_tick_test.c") | ForEach-Object {
    [pscustomobject]@{Path=$_;Sha256=Get-IsolatedFileSha256 $_}
} | ConvertTo-Json | Set-Content "$root/inputs.json"
& $wrapper cl.exe /nologo /MT /O2 /W4 /WX /TC "/I$root" "/Fo$root/test.obj" "/Fe$root/test.exe" "$repo/tests/component-integration/blocked_video_tick_test.c" *> "$root/build.log"
if($LASTEXITCODE){throw 'Blocked tick fixture build failed'}
& "$root/test.exe" *> "$root/result.txt"
if($LASTEXITCODE){throw 'Blocked tick fixture failed'}
Get-Content "$root/result.txt"
