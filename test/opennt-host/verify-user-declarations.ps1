param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'A fresh repository build directory is required'
}
New-Item -ItemType Directory $out | Out-Null
$fixture=@'
#include "wow_user_client_view_layout.h"
#define WOW_ORIGINAL_DIALOG_GETSET_LAYOUT
#include "compat/include/opennt_user_multipass.h"
#include "compat/include/opennt_user_multipass.h"
void verify_type(CLS *cls, CALLPROCDATA *cpd) { cls->spcpdFirst = cpd; }
typedef char table_capacity[(FNID_END-FNID_START+1 <= FNID_ARRAY_SIZE) ? 1 : -1];
'@
[IO.File]::WriteAllText("$out/layout.c",$fixture)
& $CompilerDriver cl.exe /nologo /c /we4133 /I "$repo/src" /I "$repo/src/wow32-dll/include" "/Fo$out/layout.obj" "$out/layout.c"
if ($LASTEXITCODE) {throw 'Original layouts or multipass pointer identity failed'}
$source=Join-Path $repo 'src/non-mvdm/windows/core/ntuser/inc/user.h'
$text=[IO.File]::ReadAllText($source)
$start=$text.IndexOf('#define FNID_END ')
$end=$text.IndexOf('#endif',$start)+6
if ($start -lt 0 -or $end -le $start) {throw 'Capacity contract not found'}
# The real header passed above. Exercise its exact capacity cohort with an
# intentionally excessive span; this must reject, not silently compile.
[IO.File]::WriteAllText("$out/overflow.c","#define FNID_START 0`n"+$text.Substring($start,$end-$start))
& $CompilerDriver cl.exe /nologo /P "/Fi$out/overflow.i" "$out/overflow.c" *> "$out/overflow.log"
if (!$LASTEXITCODE -or !(Select-String -LiteralPath "$out/overflow.log" -SimpleMatch -Pattern 'The size of the function array is greater than the allocated storage')) {
    throw 'Original capacity overflow was not rejected'
}
Get-FileHash $source,"$out/layout.c","$out/overflow.c" | Export-Csv -NoTypeInformation "$out/inputs.csv"
'PASS original USER layouts, multipass pointer identity and overflow rejection'
