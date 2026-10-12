/* Real Console is the conversion oracle; no test VT parser or quantizer.
 * Producer builds at either width; verifier links actual capture/presentation. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static const char *sequences[]={
    "\x1b[31;44mA\x1b[92;105mB\x1b[0mC",
    "\x1b[38;5;196;48;5;21mA\x1b[38;5;123;48;5;234mB\x1b[0mC",
    "\x1b[38;2;123;45;67;48;2;23;145;210mA\x1b[38;2;10;200;90;48;2;190;30;160mB\x1b[0mC",
    "\x1b[31;44mA\x1b[39;49mB\x1b[0mC"
};
static int emit(int index,int split)
{
    const char *s=sequences[index];DWORD n,w;
    while(*s){n=split?1:(DWORD)strlen(s);if(!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),s,n,&w,NULL)||w!=n)return 10;s+=n;}
    return 0;
}
#ifndef VT_PRODUCER_ONLY
#include "ntvwm-exe/console_state.h"
#include "ntvwm-exe/text_frame.h"
#include "ntcon-exe/frontend_session.h"
#include "ntcon-exe/window_frame.h"
#include "ntcon-exe/lib/kvm-window/render.h"
#include "run16-exe/native_launch.h"
static FILE *logfile;
static unsigned checks,failures;
#define CHECK(x) do{++checks;if(!(x)){++failures;fprintf(logfile,"FAIL line=%d error=%lu %s\n",__LINE__,GetLastError(),#x);}}while(0)
static DWORD rgb(COLORREF c){return ((DWORD)GetRValue(c)<<16)|((DWORD)GetGValue(c)<<8)|GetBValue(c);}
static BOOL producer(const char *exe,HANDLE out,int index,int split)
{
    PROCESS_INFORMATION pi={0};WCHAR application[1024],cmd[2048],directory[32768];
    DWORD result=999,error,bytes=0;BYTE *payload=NULL;run16_native_start start={0};
    if(!MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS,exe,-1,application,1024))return FALSE;
    if(!GetCurrentDirectoryW(32768,directory))return FALSE;
    swprintf_s(cmd,2048,L"\"%ls\" --emit %d %d",application,index,split);
    start.application=application;start.command=cmd;start.directory=directory;
    start.environment=GetEnvironmentStringsW();if(!start.environment)return FALSE;
    start.standard[0]=GetStdHandle(STD_INPUT_HANDLE);start.standard[1]=start.standard[2]=out;
    error=run16_native_launch_pack(&start,&payload,&bytes);
    if(!error)error=run16_native_launch_start(payload,bytes,&pi);
    FreeEnvironmentStringsW((LPWCH)start.environment);
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    if(error){SetLastError(error);return FALSE;}
    if(WaitForSingleObject(pi.hProcess,10000)!=WAIT_OBJECT_0){TerminateProcess(pi.hProcess,1460);WaitForSingleObject(pi.hProcess,5000);}
    GetExitCodeProcess(pi.hProcess,&result);CloseHandle(pi.hThread);CloseHandle(pi.hProcess);
    return result==0;
}
static void inspect(frontend_session *frontend,HANDLE source,HANDLE visible,void *owner,
    const char *name,WORD *attributes)
{
    ntvwm_capture capture={0};CHAR_INFO cells[2000],oracle[2000],shown[2000];
    CONSOLE_SCREEN_BUFFER_INFOEX info={sizeof(info)},projected={sizeof(projected)};
    console_text_style font={0};console_video_description description;frontend_video video={0};
    kvm_window_frame *frame=calloc(1,sizeof(*frame));BYTE *payload=NULL;
    DWORD error,count=0,offset=0;SMALL_RECT region;COORD size={80,25},zero={0,0};
    unsigned i,x,y;lib_u32 width,height,*pixels=NULL;lib_bool valid=0;kvm_window_rect changed;
    CHECK(frame!=NULL);if(!frame)return;
    CHECK(frontend_session_bind(frontend,owner,TRUE)==0);
    CHECK(GetConsoleScreenBufferInfoEx(source,&info));
    region=(SMALL_RECT){0,0,79,24};CHECK(ReadConsoleOutputW(source,oracle,size,zero,&region));
    error=ntvwm_capture_begin_output(&capture,source);CHECK(error==0);if(error)goto done;
    while(offset<2000){error=ntvwm_capture_read(&capture,offset,cells+offset,2000-offset,&region,&count);
        CHECK(error==0 && count>0);if(error||!count)goto done;offset+=count;}
    CHECK(!memcmp(cells,oracle,sizeof(cells)));
    CHECK(!memcmp(capture.info.ColorTable,info.ColorTable,sizeof(info.ColorTable)));
    /* Deliberate two-colour glyph gives independent foreground/background
     * pixel witnesses, without relying on a host font or screenshots. */
    font.font_height=16;for(i=0;i<256;++i)memset(font.fonts[0][i],0x80,16);
    capture.cursor.bVisible=FALSE;
    error=ntvwm_text_frame_pack(&capture.info,&capture.cursor,cells,2000,&font,&description,&payload);
    CHECK(error==0);if(error)goto done;
    CHECK(frontend_video_begin(&video,1,&description)==0);
    CHECK(frontend_video_data(&video,1,0,payload,description.bytes)==0);
    CHECK(frontend_window_decode_frame(&video,frame)==0);
    CHECK(kvm_window_frame_size(frame,&width,&height));
    pixels=calloc((size_t)width*height,sizeof(*pixels));CHECK(pixels!=NULL);if(!pixels)goto done;
    CHECK(kvm_window_render_frame(frame,pixels,width,height,&valid,&changed));
    CHECK(frontend_session_video(frontend,owner,&video,TRUE)==0);
    CHECK(GetConsoleScreenBufferInfoEx(visible,&projected));
    region=(SMALL_RECT){projected.srWindow.Left,projected.srWindow.Top,
        (SHORT)(projected.srWindow.Left+79),(SHORT)(projected.srWindow.Top+24)};
    CHECK(ReadConsoleOutputW(visible,shown,size,zero,&region));
    CHECK(!memcmp(projected.ColorTable,info.ColorTable,sizeof(info.ColorTable)));
    for(i=0;i<16;++i)CHECK(description.palette[i]==rgb(info.ColorTable[i]) && frame->text.base.text_palette[i]==rgb(info.ColorTable[i]));
    for(y=0;y<25;++y)for(x=0;x<80;++x){
        WORD a=cells[y*80+x].Attributes&255;const kvm_text_cell *c=&frame->text.base.cells[y*KVM_TEXT_COLUMNS+x];
        CHECK((shown[y*80+x].Attributes&255)==a);
        CHECK(c->foreground==(a&15) && c->background==(a>>4));
        CHECK(pixels[(y*16)*width+x*8]==rgb(info.ColorTable[a&15]));
        CHECK(pixels[(y*16)*width+x*8+1]==rgb(info.ColorTable[a>>4]));
        if(cells[y*80+x].Char.UnicodeChar>=32 && cells[y*80+x].Char.UnicodeChar<127)
            CHECK(shown[y*80+x].Char.UnicodeChar==cells[y*80+x].Char.UnicodeChar);
    }
    fprintf(logfile,"CASE %s",name);
    for(i=0;i<3;++i){WORD a=cells[i].Attributes&255;attributes[i]=a;
        fprintf(logfile," cell%u=U+%04x/attr%02x/fg%06lx/bg%06lx",i,cells[i].Char.UnicodeChar,a,rgb(info.ColorTable[a&15]),rgb(info.ColorTable[a>>4]));}
    fprintf(logfile," capture=production console=production window=production-pixels\n");
done:
    /* Session retains the video pointer until acknowledged unbind. */
    CHECK(frontend_session_bind(frontend,owner,FALSE)==0);
    ntvwm_capture_end(&capture);frontend_video_dispose(&video);
    if(payload)HeapFree(GetProcessHeap(),0,payload);free(pixels);free(frame);
}
static void raw_outputs(const char *exe,const char *root)
{
    char path[2048],actual[256]={0};HANDLE file,read_pipe,write_pipe;DWORD count,mode;
    sprintf_s(path,sizeof(path),"%s.raw",root);
    file=CreateFileA(path,GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    CHECK(file!=INVALID_HANDLE_VALUE);if(file==INVALID_HANDLE_VALUE)return;
    CHECK(!GetConsoleMode(file,&mode));CHECK(producer(exe,file,2,1));
    CHECK(SetFilePointer(file,0,NULL,FILE_BEGIN)!=INVALID_SET_FILE_POINTER);
    CHECK(ReadFile(file,actual,sizeof(actual),&count,NULL));
    CHECK(count==strlen(sequences[2]) && !memcmp(actual,sequences[2],count));CloseHandle(file);
    if(!CreatePipe(&read_pipe,&write_pipe,NULL,0)){CHECK(FALSE);return;}
    CHECK(!GetConsoleMode(write_pipe,&mode));CHECK(producer(exe,write_pipe,2,1));CloseHandle(write_pipe);
    CHECK(ReadFile(read_pipe,actual,sizeof(actual),&count,NULL));
    CHECK(count==strlen(sequences[2]) && !memcmp(actual,sequences[2],count));CloseHandle(read_pipe);
}
static int verify(const char *report,const char *writer)
{
    frontend_session *frontend=NULL;HANDLE input=NULL,visible=NULL,source=INVALID_HANDLE_VALUE;
    CONSOLE_SCREEN_BUFFER_INFOEX palette={sizeof(palette)},initial={sizeof(initial)},restored={sizeof(restored)};
    DWORD count;WORD complete[4][3]={{0}},attrs[3]={0};int owner=0,index,split,pass;char name[80];
    if(fopen_s(&logfile,report,"wx"))return 2;
    CHECK(frontend_session_create(&frontend)==0);if(!frontend)goto done;
    CHECK(frontend_session_console(frontend,&input,&visible)==0);if(input)CloseHandle(input);
    CHECK(GetConsoleScreenBufferInfoEx(visible,&initial));
    source=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(source!=INVALID_HANDLE_VALUE);if(source==INVALID_HANDLE_VALUE)goto done;
    {SMALL_RECT tiny={0,0,0,0},view={0,0,79,24};
        CHECK(SetConsoleWindowInfo(source,TRUE,&tiny));CHECK(SetConsoleScreenBufferSize(source,(COORD){80,25}));
        CHECK(SetConsoleWindowInfo(source,TRUE,&view));}
    CHECK(GetConsoleScreenBufferInfoEx(source,&palette));
    for(pass=0;pass<2;++pass){
        if(pass){for(index=0;index<16;++index)palette.ColorTable[index]=RGB(index*13+5,index*7+11,index*11+3);
            ++palette.srWindow.Right;++palette.srWindow.Bottom;CHECK(SetConsoleScreenBufferInfoEx(source,&palette));
            inspect(frontend,source,visible,&owner,"palette-only-retained-api-cells",attrs);}
        for(index=0;index<5;++index)for(split=0;split<2;++split){
            BOOL vt=index!=4;
            CHECK(SetConsoleMode(source,ENABLE_PROCESSED_OUTPUT|ENABLE_WRAP_AT_EOL_OUTPUT|(vt?ENABLE_VIRTUAL_TERMINAL_PROCESSING:0)));
            CHECK(FillConsoleOutputCharacterW(source,L' ',2000,(COORD){0,0},&count)&&count==2000);
            CHECK(FillConsoleOutputAttribute(source,7,2000,(COORD){0,0},&count)&&count==2000);
            CHECK(SetConsoleTextAttribute(source,7));CHECK(SetConsoleCursorPosition(source,(COORD){0,0}));
            CHECK(producer(writer,source,index%4,split));
            if(!vt){CHAR_INFO literal[3];SMALL_RECT area={0,0,2,0};
                CHECK(ReadConsoleOutputW(source,literal,(COORD){3,1},(COORD){0,0},&area));
                CHECK(literal[1].Char.UnicodeChar==L'[' && literal[2].Char.UnicodeChar==L'3' && literal[1].Attributes==7);}
            sprintf_s(name,sizeof(name),"palette=%d sequence=%d vt=%d split=%d",pass,index,vt,split);
            inspect(frontend,source,visible,&owner,name,attrs);
            if(index<4){if(!split)memcpy(complete[index],attrs,sizeof(attrs));else CHECK(!memcmp(complete[index],attrs,sizeof(attrs)));}
            if(index==0){CHECK(attrs[0]==0x14);CHECK(attrs[1]==0xda);CHECK(attrs[2]==7);}
        }
        /* Explicit API indexed writer, including every foreground/background. */
        {CHAR_INFO cells[2000];SMALL_RECT area={0,0,79,24};
            for(index=0;index<2000;++index){cells[index].Char.UnicodeChar=L'X';cells[index].Attributes=(WORD)(index%256);}
            CHECK(WriteConsoleOutputW(source,cells,(COORD){80,25},(COORD){0,0},&area));
            inspect(frontend,source,visible,&owner,pass?"api-custom-palette":"api-default-palette",attrs);}
    }
    raw_outputs(writer,report);
done:
    if(source!=INVALID_HANDLE_VALUE)CloseHandle(source);
    if(frontend)frontend_session_destroy(frontend);
    if(visible){CHECK(GetConsoleScreenBufferInfoEx(visible,&restored));CHECK(!memcmp(initial.ColorTable,restored.ColorTable,sizeof(initial.ColorTable)));CloseHandle(visible);}
    fprintf(logfile,"RESULT checks=%u failures=%u\n",checks,failures);fclose(logfile);return failures?1:0;
}
#endif
int main(int argc,char **argv)
{
    if(argc==4 && !strcmp(argv[1],"--emit")){int i=atoi(argv[2]);return i>=0&&i<4?emit(i,atoi(argv[3])):2;}
#ifndef VT_PRODUCER_ONLY
    if(argc==3)return verify(argv[1],argv[2]);
#endif
    return 2;
}
