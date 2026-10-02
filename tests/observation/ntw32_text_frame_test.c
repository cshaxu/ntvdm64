#include "ntw32-exe/text_frame.h"
#include "ntkvm-exe/console_video.h"
#include <stdio.h>
#include <string.h>

static unsigned checks,failures;
#define CHECK(test) do {++checks;if(!(test)){++failures;fprintf(log,"FAIL line=%d %s\n",__LINE__,#test);}} while(0)
int wmain(int argc,WCHAR **argv)
{
    CONSOLE_SCREEN_BUFFER_INFOEX screen={sizeof(screen)};
    CONSOLE_CURSOR_INFO cursor={20,TRUE};CHAR_INFO cells[12]={0};
    console_text_style font={0},*style;console_video_description description;
    run16_console_video video={0};BYTE *payload=NULL;FILE *log=NULL;
    unsigned i,height;DWORD error;
    if(argc!=2 || _wfopen_s(&log,argv[1],L"wx"))return 2;
    screen.dwSize=(COORD){6,2};screen.srWindow=(SMALL_RECT){1,0,4,1};
    screen.dwCursorPosition=(COORD){3,1};screen.ColorTable[1]=RGB(12,34,56);
    for(i=0;i<12;++i){cells[i].Char.UnicodeChar=L'A';cells[i].Attributes=0x17;}
    cells[1].Char.UnicodeChar=0x263a;
    cells[2].Char.UnicodeChar=0x2502;
    cells[3].Char.UnicodeChar=0x2302;
    cells[4].Char.UnicodeChar=L'Z';cells[4].Attributes=0x1e|COMMON_LVB_REVERSE_VIDEO;
    cells[7].Char.UnicodeChar=0xd83d;cells[8].Char.UnicodeChar=0xde00;
    cells[9].Char.UnicodeChar=0x4e00;cells[9].Attributes|=COMMON_LVB_LEADING_BYTE;
    cells[10].Char.UnicodeChar=0x4e00;cells[10].Attributes|=COMMON_LVB_TRAILING_BYTE;
    for(i=0;i<sizeof(font.fonts);++i)((BYTE *)font.fonts)[i]=(BYTE)(i*37+(i>>8));
    for(height=14;height<=32;height+=18) {
        font.font_height=height;font.attribute_font_select=height==32;
        error=ntw32_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload);
        CHECK(error==0);if(error)break;
        style=(console_text_style *)payload;
        CHECK(description.kind==CONSOLE_VIDEO_TEXT_FRAME && description.depth==0);
        CHECK(description.width==4 && description.height==2 && description.stride==8);
        CHECK(description.bytes==sizeof(font)+16);
        CHECK(description.palette[1]==0x0c2238 && description.palette[16]==0);
        CHECK(memcmp(style->fonts,font.fonts,sizeof(font.fonts))==0);
        CHECK(style->attribute_font_select==font.attribute_font_select);
        CHECK(style->cursor_column==2 && style->cursor_row==1 && style->cursor_visible==1);
        CHECK(style->cursor_start+style->cursor_height==(int)height);
        CHECK(style->cursor_height1==0);
        CHECK(payload[sizeof(font)]==1 && payload[sizeof(font)+2]==0xb3);
        CHECK(payload[sizeof(font)+4]==127 && payload[sizeof(font)+6]=='Z');
        CHECK(payload[sizeof(font)+7]==0xe1);
        CHECK(payload[sizeof(font)+8]=='?' && payload[sizeof(font)+10]==' ');
        CHECK(payload[sizeof(font)+12]=='?' && payload[sizeof(font)+14]==' ');
        /* Use the production NTVDM receiver unchanged. Incomplete frames must
         * not replace the last complete one. No special native decoder. */
        CHECK(run16_console_video_begin(&video,height,&description)==0);
        CHECK(run16_console_video_data(&video,height,0,payload,100)==0);
        CHECK(video.published_serial==(height==14 ? 0u : 14u));
        CHECK(run16_console_video_data(&video,height,100,payload+100,description.bytes-100)==0);
        CHECK(video.published_serial==height && !memcmp(video.pixels,payload,description.bytes));
        HeapFree(GetProcessHeap(),0,payload);payload=NULL;
    }
    /* A horizontally scrolled viewport can begin at the second UTF-16 cell.
     * Its predecessor remains in the captured buffer, outside the viewport. */
    cells[0].Char.UnicodeChar=0xd83d;cells[1].Char.UnicodeChar=0xde00;
    CHECK(ntw32_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==0);
    if(payload) {
        CHECK(payload[sizeof(font)]==' ');
        HeapFree(GetProcessHeap(),0,payload);payload=NULL;
    }
    cells[0].Char.UnicodeChar=L'A';cells[1].Char.UnicodeChar=0xde00;
    CHECK(ntw32_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==0);
    if(payload) {
        CHECK(payload[sizeof(font)]=='?');
        HeapFree(GetProcessHeap(),0,payload);payload=NULL;
    }
    cells[1].Attributes|=COMMON_LVB_UNDERSCORE;
    CHECK(ntw32_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==0);
    if(payload) {
        CHECK(description.stride==12 && description.bytes==sizeof(font)+24);
        CHECK(payload[sizeof(font)+2]==CONSOLE_TEXT_UNDERLINE);
        CHECK(run16_console_video_begin(&video,33,&description)==0);
        CHECK(run16_console_video_data(&video,33,0,payload,description.bytes)==0);
        CHECK(video.published_serial==33);
        payload[sizeof(font)+2]=0x80;
        CHECK(run16_console_video_begin(&video,34,&description)==0);
        CHECK(run16_console_video_data(&video,34,0,payload,description.bytes)==ERROR_INVALID_DATA);
        CHECK(video.published_serial==33);
        HeapFree(GetProcessHeap(),0,payload);payload=NULL;
    }
    cells[1].Attributes=0x17;
    CHECK(ntw32_text_frame_pack(&screen,&cursor,cells,11,&font,&description,&payload)==ERROR_INVALID_DATA);
    screen.srWindow.Left=-1;
    CHECK(ntw32_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==ERROR_INVALID_DATA);
    screen.srWindow.Left=1;font.font_height=33;
    CHECK(ntw32_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==ERROR_INVALID_DATA);
    CHECK(video.published_serial==33);
    run16_console_video_dispose(&video);
    {
        CHAR_INFO *page=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,4000*sizeof(*page));
        CHECK(page!=NULL);
        if(page) {
            for(i=0;i<4000;++i){page[i].Char.UnicodeChar=L'A';page[i].Attributes=7;}
            screen.dwSize=(COORD){80,50};screen.srWindow=(SMALL_RECT){0,0,79,49};
            screen.dwCursorPosition=(COORD){79,49};font.font_height=16;
            CHECK(!ntw32_text_frame_pack(&screen,&cursor,page,4000,&font,&description,&payload));
            if(payload) {
                ntw32_mouse mouse={0},saved;
                console_pointer_input motion={0};INPUT_RECORD events[2];DWORD event_count=0;
                BYTE *text=payload+sizeof(console_text_style);
                CHECK(!ntw32_mouse_geometry(&mouse,screen.srWindow,16));
                ntw32_mouse_compose(&mouse,&description,payload);
                CHECK(text[(25*80+40)*2+1]==7);
                motion.action=CONSOLE_MOUSE_ENTER;
                CHECK(!ntw32_mouse_input(&mouse,&motion,events,&event_count) && !event_count);
                ntw32_mouse_compose(&mouse,&description,payload);
                CHECK(text[(25*80+40)*2+1]==(7^0x77));
                CHECK(page[25*80+40].Attributes==7);
                CHECK(((console_text_style *)payload)->cursor_column==79);
                CHECK(((console_text_style *)payload)->cursor_row==49);
                HeapFree(GetProcessHeap(),0,payload);payload=NULL;
                CHECK(!ntw32_text_frame_pack(&screen,&cursor,page,4000,&font,&description,&payload));
                motion.action=CONSOLE_MOUSE_MOVE;motion.dx=INT_MAX;motion.dy=INT_MAX;
                motion.control=SHIFT_PRESSED;motion.buttons=1;
                CHECK(!ntw32_mouse_input(&mouse,&motion,events,&event_count) && event_count==2);
                CHECK(events[0].Event.MouseEvent.dwMousePosition.X==79 &&
                    events[0].Event.MouseEvent.dwMousePosition.Y==49);
                CHECK(events[0].Event.MouseEvent.dwEventFlags==MOUSE_MOVED &&
                    !events[0].Event.MouseEvent.dwButtonState);
                CHECK(events[1].Event.MouseEvent.dwButtonState==FROM_LEFT_1ST_BUTTON_PRESSED &&
                    events[1].Event.MouseEvent.dwControlKeyState==SHIFT_PRESSED);
                ntw32_mouse_compose(&mouse,&description,payload);
                text=payload+sizeof(console_text_style);
                CHECK(text[(25*80+40)*2+1]==7 && text[3999*2+1]==(7^0x77));
                CHECK(page[3999].Attributes==7 && description.kind==CONSOLE_VIDEO_TEXT_FRAME);
                saved=mouse;motion.buttons=4;
                CHECK(ntw32_mouse_input(&mouse,&motion,events,&event_count)==ERROR_INVALID_DATA);
                CHECK(!memcmp(&mouse,&saved,sizeof(mouse)));
                motion=(console_pointer_input){0};motion.action=CONSOLE_MOUSE_LEAVE;
                CHECK(!ntw32_mouse_input(&mouse,&motion,events,&event_count) && event_count==1);
                CHECK(!events[0].Event.MouseEvent.dwButtonState && !mouse.visible);
                CHECK(!ntw32_mouse_input(&mouse,&motion,events,&event_count) && !event_count);
                CHECK(!ntw32_mouse_geometry(&mouse,(SMALL_RECT){20,100,59,124},8));
                CHECK(mouse.x==319 && mouse.y==199);
                motion.action=CONSOLE_MOUSE_MOVE;motion.dx=-1;motion.dy=-1;
                CHECK(!ntw32_mouse_input(&mouse,&motion,events,&event_count) && event_count==1);
                CHECK(events[0].Event.MouseEvent.dwMousePosition.X==59 &&
                    events[0].Event.MouseEvent.dwMousePosition.Y==124);
                CHECK(description.width==80 && description.height==50 && description.stride==160);
                CHECK(!run16_console_video_begin(&video,1,&description));
                CHECK(!run16_console_video_data(&video,1,0,payload,description.bytes));
                CHECK(video.published_serial==1);
                HeapFree(GetProcessHeap(),0,payload);payload=NULL;
            }
            HeapFree(GetProcessHeap(),0,page);run16_console_video_dispose(&video);
        }
    }
    {
        static const unsigned rows[]={22,25,28,43,50};
        ntw32_mouse mouse={0},other={0},saved;
        INPUT_RECORD events[2];DWORD count;
        console_pointer_input motion={0};
        for(i=0;i<ARRAYSIZE(rows);++i) {
            unsigned font_height=rows[i]>=43 ? 8 : rows[i]==28 ? 14 : 16;
            CHECK(!ntw32_mouse_geometry(&mouse,(SMALL_RECT){0,100,79,(SHORT)(99+rows[i])},font_height));
            motion=(console_pointer_input){INT_MAX,INT_MAX,LEFT_ALT_PRESSED,0,CONSOLE_MOUSE_MOVE};
            CHECK(!ntw32_mouse_input(&mouse,&motion,events,&count));
            CHECK(mouse.x==639 && mouse.y==(LONG)(rows[i]*font_height-1) && !mouse.buttons);
            CHECK(count==1 && events[0].Event.MouseEvent.dwMousePosition.X==79 &&
                events[0].Event.MouseEvent.dwMousePosition.Y==99+rows[i]);
            CHECK(events[0].Event.MouseEvent.dwControlKeyState==LEFT_ALT_PRESSED);
            motion.dx=INT_MIN;motion.dy=INT_MIN;
            CHECK(!ntw32_mouse_input(&mouse,&motion,events,&count) && count==1);
            CHECK(!mouse.x && !mouse.y && events[0].Event.MouseEvent.dwMousePosition.Y==100);
        }
        CHECK(!ntw32_mouse_geometry(&other,(SMALL_RECT){0,0,79,24},16));
        motion=(console_pointer_input){0,0,0,0,CONSOLE_MOUSE_POSITION};
        CHECK(!ntw32_mouse_input(&other,&motion,events,&count) &&
            !other.x && !other.y && count==1);
        motion.dx=639;motion.dy=399;motion.buttons=1;
        CHECK(!ntw32_mouse_input(&other,&motion,events,&count) &&
            other.x==639 && other.y==399 && count==2 &&
            events[1].Event.MouseEvent.dwMousePosition.X==79 &&
            events[1].Event.MouseEvent.dwMousePosition.Y==24);
        motion.buttons=0;motion.dx=0;motion.dy=0;
        CHECK(!ntw32_mouse_input(&other,&motion,events,&count) &&
            !other.x && !other.y && count==2);
        saved=other;motion=(console_pointer_input){0,0,0,3,CONSOLE_MOUSE_MOVE};
        CHECK(!ntw32_mouse_input(&mouse,&motion,events,&count) && count==1);
        CHECK(events[0].Event.MouseEvent.dwButtonState==
            (FROM_LEFT_1ST_BUTTON_PRESSED|RIGHTMOST_BUTTON_PRESSED));
        CHECK(!memcmp(&other,&saved,sizeof(other)));
        saved=mouse;motion.action=CONSOLE_MOUSE_LEAVE;
        CHECK(ntw32_mouse_input(&mouse,&motion,events,&count)==ERROR_INVALID_DATA && !count);
        CHECK(!memcmp(&mouse,&saved,sizeof(mouse)));
        motion.buttons=0;
        CHECK(!ntw32_mouse_input(&mouse,&motion,events,&count) && count==1 && !mouse.visible);
        CHECK(!events[0].Event.MouseEvent.dwButtonState);
        CHECK(!ntw32_mouse_input(&mouse,&motion,events,&count) && !count);
    }
    fprintf(log,"NTW32-TEXT-FRAME checks=%u failures=%u production-receiver=yes production-channel=no\n",checks,failures);
    fclose(log);return failures ? 1 : 0;
}
