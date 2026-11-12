param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'A fresh repository build directory is required'
}
New-Item -ItemType Directory $out | Out-Null
$source=Join-Path $repo 'src/mvdm-host/wow32/wow32.c'
$text=[IO.File]::ReadAllText($source)
$start=$text.IndexOf('    cb = sizeof(fThunkStrRtns);')
$end=$text.IndexOf('#ifdef DEBUG',$start)
if ($start -lt 0 -or $end -le $start) {throw 'Original initialization range not found'}
$body=$text.Substring($start,$end-$start)
# Compile production statements; control only the registry/locale boundaries.
# Real WOW startup remains a separate integration gate.
$before=@'
#include <windows.h>
#include <stdio.h>
static LONG query_status;
static DWORD query_type,query_value,calls;
static LCID locale;
static LONG query(HKEY key,LPCSTR name,LPDWORD reserved,LPDWORD type,LPBYTE value,LPDWORD size)
{
    (void)key; (void)name; (void)reserved; ++calls;
    if (*size != sizeof(DWORD)) return ERROR_INVALID_DATA;
    if (query_status) return query_status;
    *type=query_type; *(DWORD *)value=query_value; *size=sizeof(DWORD);
    return ERROR_SUCCESS;
}
#define RegQueryValueEx query
#define GetSystemDefaultLCID() locale
static DWORD initialize(void)
{
    DWORD cb,dwType,fThunkStrRtns=99;
    HKEY WowKey=NULL;
'@
$after=@'
    return fThunkStrRtns;
}
int main(void)
{
    DWORD i;
    static const struct { LONG status; DWORD type,value; LCID lcid; DWORD expected; } cases[]={
        {0,REG_DWORD,0,0x411,0}, {0,REG_DWORD,1,0x409,1},
        {ERROR_FILE_NOT_FOUND,0,0,0x409,0}, {ERROR_FILE_NOT_FOUND,0,0,0x411,1},
        {0,REG_SZ,1,0x409,0}, {ERROR_ACCESS_DENIED,0,0,0x411,1}
    };
    for(i=0;i<sizeof(cases)/sizeof(cases[0]);++i) {
        query_status=cases[i].status; query_type=cases[i].type;
        query_value=cases[i].value; locale=cases[i].lcid; calls=0;
        if(initialize()!=cases[i].expected || calls!=1) return (int)i+1;
    }
    puts("PASS WOW ThunkNLS: six original-source controls"); return 0;
}
'@
[IO.File]::WriteAllText("$out/fixture.c",$before+"`n"+$body+"`n"+$after)
& $CompilerDriver cl.exe /nologo /W4 /MT "/Fo$out/fixture.obj" "$out/fixture.c" /link "/out:$out/fixture.exe"
if ($LASTEXITCODE) {throw 'Compilation failed'}
& "$out/fixture.exe" | Tee-Object "$out/result.log"
if ($LASTEXITCODE) {throw 'Original ThunkNLS selection failed'}
Get-FileHash $source,"$out/fixture.c","$out/fixture.exe" | Export-Csv -NoTypeInformation "$out/inputs.csv"
