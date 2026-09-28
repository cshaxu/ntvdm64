/* Admission probe against an unmodified, separately pinned libvterm source.
 * Not linked into the product. Known gaps are printed, not called features. */
#include <stdio.h>
#include <string.h>
#include "vterm.h"

static char replies[4096];
static size_t reply_bytes;
static int failures, assertions, fallback_calls;
#define CHECK(x) do { ++assertions; if(!(x)) { ++failures; printf("FAIL line=%d assertion=%s\n",__LINE__,#x); } } while(0)
static void output(const char *bytes,size_t count,void *context)
{
    (void)context;
    if(count>=sizeof(replies)-reply_bytes) { ++failures;return; }
    memcpy(replies+reply_bytes,bytes,count);reply_bytes+=count;replies[reply_bytes]=0;
}
static int unknown_csi(const char *leader,const long args[],int count,
    const char *intermediate,char command,void *context)
{
    (void)leader;(void)args;(void)count;(void)intermediate;(void)command;(void)context;
    ++fallback_calls;return 1;
}
static void feed(VTerm *terminal,const char *bytes)
{
    /* Every sequence and UTF-8 scalar is deliberately fragmented. */
    for(size_t i=0;i<strlen(bytes);++i) {
        if(vterm_input_write(terminal,bytes+i,1)!=1)++failures;
    }
}
static VTermScreenCell cell(VTermScreen *screen,int row,int col)
{
    VTermScreenCell value={0};
    CHECK(vterm_screen_get_cell(screen,(VTermPos){row,col},&value));
    return value;
}
int main(void)
{
    VTerm *terminal=vterm_new(4,12);
    VTermScreen *screen;
    VTermScreenCell value;
    VTermStateFallbacks fallbacks={0};
    int rows,columns;
    if(!terminal)return 2;
    vterm_set_utf8(terminal,1);
    vterm_output_set_callback(terminal,output,NULL);
    screen=vterm_obtain_screen(terminal);
    if(!screen){vterm_free(terminal);return 2;}
    vterm_screen_enable_altscreen(screen,1);
    vterm_screen_reset(screen,1);
    feed(terminal,"A\xe4\xb8\xad" "e\xcc\x81");
    value=cell(screen,0,0);CHECK(value.chars[0]=='A');
    value=cell(screen,0,1);CHECK(value.chars[0]==0x4e2d && value.width==2);
    value=cell(screen,0,3);CHECK(value.chars[0]=='e' && value.chars[1]==0x301);
    feed(terminal,"\x1b[2;1H\x1b[38;2;11;22;33mZ");
    value=cell(screen,1,0);vterm_screen_convert_color_to_rgb(screen,&value.fg);
    CHECK(value.chars[0]=='Z' && value.fg.rgb.red==11 && value.fg.rgb.green==22 && value.fg.rgb.blue==33);
    reply_bytes=0;replies[0]=0;
    feed(terminal,"\x1b[3;4H\x1b[6n");CHECK(!strcmp(replies,"\x1b[3;4R"));
    feed(terminal,"\x1b[?1049h\x1b[HX");
    value=cell(screen,0,0);CHECK(value.chars[0]=='X');
    feed(terminal,"\x1b[?1049l");
    value=cell(screen,0,0);CHECK(value.chars[0]=='A');
    feed(terminal,"\x1b[2J\x1b[HA\x1b[2;1HB\x1b[1S");
    value=cell(screen,0,0);CHECK(value.chars[0]=='B');
    vterm_set_size(terminal,5,20);vterm_get_size(terminal,&rows,&columns);
    CHECK(rows==5 && columns==20);
    value=cell(screen,0,0);CHECK(value.chars[0]=='B');
    feed(terminal,"\x1b[?1003h\x1b[?1006h");
    reply_bytes=0;replies[0]=0;
    vterm_mouse_move(terminal,1,2,VTERM_MOD_NONE);
    vterm_mouse_button(terminal,1,1,VTERM_MOD_NONE);
    vterm_mouse_button(terminal,1,0,VTERM_MOD_NONE);
    CHECK(strstr(replies,"\x1b[<0;3;2M") && strstr(replies,"\x1b[<0;3;2m"));
    fallbacks.csi=unknown_csi;
    vterm_screen_set_unrecognised_fallbacks(screen,&fallbacks,NULL);
    feed(terminal,"\x1b[?9001h");
    CHECK(fallback_calls==0);
    printf("LIMITATION DECSET-9001 is consumed without an unrecognised-CSI callback; Win32 key encoding is not implemented by this candidate.\n");
    vterm_free(terminal);
    printf("terminal-candidate assertions=%d failures=%d\n",assertions,failures);
    return failures ? 1 : 0;
}
