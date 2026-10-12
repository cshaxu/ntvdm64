/* The runner extracts the actual production functions on every build.
 * These stubs expose side effects; they do not replace the guarded logic. */
#include <windows.h>
#include <stdio.h>
#define CCPU 1
#define EGG 1
#undef TEXT
#define TEXT 0
#define STREAM_IO 2
#define VGA 1
#define EGA 2
#define EGA_TICK_DELAY 3
#define MOUSE_DELAY 3
#define TICKS_PER_FLUSH 2
#define CNTRL_VDMBLOCKED 0x20000
#define EHS_FUNC_FAILED 1001
static ULONG CntrlHandlerState;
static struct { int ModeType,ScreenState;HPALETTE ColPalette; } sc;
static BOOL presentation_updated,ConsoleInitialised=TRUE,ConsoleNoUpdates;
static int flush_count,video_adapter=VGA,mode_change_now,update_vlt;
static int ega_tick_delay,CursorResizeNeeded,changing;
static unsigned routes,paints,frames,configs,streams,shapes,errors,modes;
static DWORD send_error;
static int mvdm_softpc_text_video_sync_route(void){++routes;return 1;}
static BOOL NtvdmConsoleUpdateText(HPALETTE p)
{(void)p;++frames;SetLastError(send_error);return !send_error;}
static BOOL NtvdmConsoleUpdateTextConfiguration(HPALETTE p)
{(void)p;++configs;SetLastError(send_error);return !send_error;}
static void DisplayErrorTerm(int n,DWORD e,const char *f,int l)
{(void)n;(void)e;(void)f;(void)l;++errors;}
static void stream_io_update(void){++streams;}
static int get_mode_change_required(void){return changing;}
static void set_mode_change_required(int value){changing=value;}
static int get_palette_change_required(void){return 0;}
static void set_the_vlt(void){}
static void DelayMouseEvents(int value){(void)value;}
static void make_cursor_change(void){++shapes;CursorResizeNeeded=0;}
static void paint(void){++paints;presentation_updated=TRUE;}
static struct {void (*calc_update)(void);} update_alg={paint};
static void choose_mode(void){++modes;}
static void (*choose_display_mode)(void)=choose_mode;
#include "blocked_video_tick.inc"
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void)
{
    presentation_updated=TRUE;CursorResizeNeeded=1;flush_count=TICKS_PER_FLUSH-1;
    CntrlHandlerState=CNTRL_VDMBLOCKED;
    nt_graphics_tick();
    CHECK(!routes && !paints && !frames && !shapes && !errors);
    CHECK(presentation_updated && CursorResizeNeeded && flush_count==TICKS_PER_FLUSH-1);
    changing=1;mode_change_now=1;nt_graphics_tick();
    CHECK(changing && mode_change_now==1 && !modes && !routes);
    changing=mode_change_now=0;
    CntrlHandlerState=0;nt_graphics_tick();
    CHECK(routes==1 && paints==1 && frames==1 && shapes==1 && !presentation_updated && !errors);
    sc.ScreenState=STREAM_IO;CntrlHandlerState=CNTRL_VDMBLOCKED;flush_count=1;
    nt_graphics_tick();CHECK(!configs && !streams && flush_count==1 && routes==1);
    CntrlHandlerState=0;nt_graphics_tick();CHECK(configs==1 && streams==1 && flush_count==0);
    /* Independent shell-count bits do not suspend rendering. */
    sc.ScreenState=0;CntrlHandlerState=3;flush_count=1;nt_graphics_tick();CHECK(frames==2);
    /* A real failure on an owned channel is still reported, never hidden. */
    send_error=ERROR_NOT_READY;flush_count=1;nt_graphics_tick();CHECK(errors==1 && frames==3);
    send_error=ERROR_BROKEN_PIPE;flush_count=1;nt_graphics_tick();CHECK(errors==2 && frames==4);
    /* Original handoff's final paint precedes setting the blocked bit. */
    send_error=0;CntrlHandlerState=0;presentation_updated=TRUE;publish_text_update();
    CHECK(frames==5 && !presentation_updated && errors==2);
    puts("PASS blocked video tick: no transport, retained dirt/cursor/mode/stream state, resume, shell-count independence, final paint and real error propagation");
    return 0;
}
