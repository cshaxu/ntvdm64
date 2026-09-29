#include "ntcon-exe/text_frame.h"
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
        error=ntcon_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload);
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
    CHECK(ntcon_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==0);
    if(payload) {
        CHECK(payload[sizeof(font)]==' ');
        HeapFree(GetProcessHeap(),0,payload);payload=NULL;
    }
    cells[0].Char.UnicodeChar=L'A';cells[1].Char.UnicodeChar=0xde00;
    CHECK(ntcon_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==0);
    if(payload) {
        CHECK(payload[sizeof(font)]=='?');
        HeapFree(GetProcessHeap(),0,payload);payload=NULL;
    }
    cells[1].Attributes|=COMMON_LVB_UNDERSCORE;
    CHECK(ntcon_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==0);
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
    CHECK(ntcon_text_frame_pack(&screen,&cursor,cells,11,&font,&description,&payload)==ERROR_INVALID_DATA);
    screen.srWindow.Left=-1;
    CHECK(ntcon_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==ERROR_INVALID_DATA);
    screen.srWindow.Left=1;font.font_height=33;
    CHECK(ntcon_text_frame_pack(&screen,&cursor,cells,12,&font,&description,&payload)==ERROR_INVALID_DATA);
    CHECK(video.published_serial==33);
    run16_console_video_dispose(&video);
    fprintf(log,"NTCON-TEXT-FRAME checks=%u failures=%u production-receiver=yes production-channel=no\n",checks,failures);
    fclose(log);return failures ? 1 : 0;
}
