[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith("$repo\build\",[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
New-Item -ItemType Directory -Force $out | Out-Null
$source=Get-Content "$repo/src/mvdm-host/softpc.new/host/src/nt_fulsc.c" -Raw
$body=[regex]::Match($source,'(?ms)^GLOBAL VOID calcScreenParams.*?^\}').Value
if(!$body){throw 'Console restoration function not found'}
$fixture=@'
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#define GLOBAL
#define IFN2(t,a,u,b) (t a,u b)
typedef unsigned char half_word;
typedef unsigned long sys_addr;
#define VGA_HEIGHT_0 22
#define VGA_HEIGHT_1 25
#define VGA_HEIGHT_2 28
#define VGA_HEIGHT_3 43
#define VGA_HEIGHT_4 50
#define MID_VAL(x,y) (x+((y)-(x))/2)
#define EGA_CGDDOT 8
#define EGA_CGMN 14
#define EGA_HIFONT 16
#define EGA_IPSTAT1_REG 0
#define EGA_AC_INDEX_DATA 0
#define EGA_PALETTE_ENABLE 0
#define VID_LEN 0
#define VID_COLS 1
static struct {HANDLE OutputHandle;} sc;
static CONSOLE_SCREEN_BUFFER_INFO ConsBufferInfo;
static int blocked_in_gfx_mode,cell_height,font_height,loads,scanlines;
static BYTE font[32];
static BOOL WINAPI console_info(HANDLE h,PCONSOLE_SCREEN_BUFFER_INFO b){(void)h;(void)b;return TRUE;}
#define GetConsoleScreenBufferInfo console_info
#define ErrorExit() abort()
static int get_char_height(void){return cell_height;}
static void low_set_mode(int mode){(void)mode;}
static void inb(int port,half_word *v){(void)port;*v=0;}
static void outb(int port,int v){(void)port;(void)v;}
static void display_mode(void){}
static void (*choose_display_mode)(void)=display_mode;
static void set_screen_height_recal(int n){scanlines=n;}
static void recalc_text(int n){cell_height=n;}
static void sas_storew_no_check(int a,int b){(void)a;(void)b;}
void load_font(sys_addr table,int count,int first,int bank,int n){
    if(count!=256 || first || bank || table!=(sys_addr)n)abort();
    ++loads;font_height=n;
    for(int i=0;i<n;++i)font[i]=(BYTE)n;
    /* Original loader changes registers; dimensions must follow. */
    scanlines=-1;cell_height=16;
}
@CALLBACK@
#define CHECK(x) do {if(!(x)){fprintf(stderr,"failed line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
    const int rows[]={22,25,28,43,50}, heights[]={16,16,14,8,8};
    USHORT height,lines;
    for(int i=0;i<5;++i){
        ConsBufferInfo.srWindow.Bottom=(SHORT)(rows[i]-1);
        ConsBufferInfo.srWindow.Right=79;
        /* Eight new scanlines over stale taller glyphs after Windows. */
        cell_height=font_height=8;loads=0;
        for(int j=0;j<32;++j)font[j]=(BYTE)(j<8?8:99);
        calcScreenParams(&height,&lines);
        CHECK(height==heights[i] && lines==rows[i]);
        CHECK(cell_height==heights[i] && font_height==heights[i]);
        CHECK(scanlines>0);
        for(int j=0;j<height;++j)CHECK(font[j]==height);
        CHECK(loads==(height!=8));
        font[0]=123;loads=0;
        calcScreenParams(&height,&lines);
        CHECK(!loads && font[0]==123);
    }
    puts("PASS font restoration and same-height downloaded glyph preservation");
    return 0;
}
'@
$vswhere=Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Microsoft Visual Studio\Installer\vswhere.exe'
$vs=& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(!$vs){throw 'MSVC development environment required'}
$dev=Join-Path $vs 'Common7\Tools\VsDevCmd.bat'
foreach($case in @('fixed','regression-control')) {
    $callback=$body
    if($case -eq 'regression-control'){
        $callback=[regex]::Replace($body,'(?s)\s*if \(charHeight != get_char_height\(\)\)\s*\{.*?\}', '')
        if($callback -eq $body){throw 'Expected font restoration block missing: regression present'}
    }
    [IO.File]::WriteAllText("$out/$case.c",$fixture.Replace('@CALLBACK@',$callback))
    @('@echo off',('call "{0}" -arch=x86 -host_arch=x64 >nul' -f $dev),
      'if errorlevel 1 exit /b 1',
      ('cl /nologo /W3 /I"{0}/src" /Fo"{1}/{2}.obj" /Fe"{1}/{2}.exe" "{1}/{2}.c"' -f $repo,$out,$case),
      'exit /b %errorlevel%') | Set-Content "$out/$case.cmd" -Encoding ascii
    $p=Start-Process cmd.exe -ArgumentList ('/d /c "{0}/{1}.cmd"' -f $out,$case) -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$case.build.log" -RedirectStandardError "$out/$case.build.err"
    if(!$p.WaitForExit(60000)){$p.Kill();throw 'Compiler timeout'}
    if($p.ExitCode){throw "Compile failed: $case"}
    $p=Start-Process "$out/$case.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$case.log" -RedirectStandardError "$out/$case.err"
    if(!$p.WaitForExit(10000)){$p.Kill();throw 'Fixture timeout'}
    if(($case -eq 'fixed' -and $p.ExitCode -ne 0) -or ($case -ne 'fixed' -and $p.ExitCode -ne 1)){throw "Unexpected result: $case exit=$($p.ExitCode)"}
}
'PASS font restoration; old-code control fails'
