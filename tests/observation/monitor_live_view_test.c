/* Live RPC + the actual product renderer/key dispatch. Captures its real
 * Console cells to a bitmap without touching the user's desktop. */
#define wmain monitor_product_main
#include "../../src/ntmon-exe/main.c"
#undef wmain
#include <assert.h>

static void capture_bitmap(HANDLE output,const WCHAR *prefix,const WCHAR *suffix)
{
    CHAR_INFO cells[MONITOR_COLUMNS*MONITOR_ROWS];
    SMALL_RECT area={0,0,MONITOR_COLUMNS-1,MONITOR_ROWS-1};
    CONSOLE_SCREEN_BUFFER_INFOEX console={sizeof(console)};
    BITMAPINFO bitmap={0};BITMAPFILEHEADER file={0};void *pixels=NULL;
    HDC dc=CreateCompatibleDC(NULL);HBITMAP image;HFONT font;HANDLE target;
    WCHAR path[1024];DWORD bytes,written;const int width=800,height=500;
    assert(dc && ReadConsoleOutputW(output,cells,(COORD){80,25},(COORD){0,0},&area));
    assert(GetConsoleScreenBufferInfoEx(output,&console));
    bitmap.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bitmap.bmiHeader.biWidth=width;
    bitmap.bmiHeader.biHeight=-height;bitmap.bmiHeader.biPlanes=1;
    bitmap.bmiHeader.biBitCount=32;bitmap.bmiHeader.biCompression=BI_RGB;
    image=CreateDIBSection(dc,&bitmap,DIB_RGB_COLORS,&pixels,NULL,0);assert(image && pixels);
    HGDIOBJ old_image=SelectObject(dc,image);
    font=CreateFontW(-19,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,FIXED_PITCH,L"Consolas");assert(font);
    HGDIOBJ old_font=SelectObject(dc,font);
    for(unsigned y=0;y<MONITOR_ROWS;++y)for(unsigned x=0;x<MONITOR_COLUMNS;++x) {
        const CHAR_INFO *cell=&cells[y*MONITOR_COLUMNS+x];
        RECT rectangle={(LONG)x*10,(LONG)y*20,(LONG)(x+1)*10,(LONG)(y+1)*20};
        WCHAR glyph=cell->Char.UnicodeChar ? cell->Char.UnicodeChar : L' ';
        SetTextColor(dc,console.ColorTable[cell->Attributes&15]);
        SetBkColor(dc,console.ColorTable[(cell->Attributes>>4)&15]);
        assert(ExtTextOutW(dc,rectangle.left,rectangle.top,ETO_OPAQUE|ETO_CLIPPED,&rectangle,&glyph,1,NULL));
    }
    swprintf_s(path,ARRAYSIZE(path),L"%s-%s.bmp",prefix,suffix);
    target=CreateFileW(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);assert(target!=INVALID_HANDLE_VALUE);
    bytes=(DWORD)width*height*4;file.bfType=0x4d42;file.bfOffBits=sizeof(file)+sizeof(bitmap.bmiHeader);
    file.bfSize=file.bfOffBits+bytes;
    assert(WriteFile(target,&file,sizeof(file),&written,NULL) && written==sizeof(file));
    assert(WriteFile(target,&bitmap.bmiHeader,sizeof(bitmap.bmiHeader),&written,NULL) && written==sizeof(bitmap.bmiHeader));
    assert(WriteFile(target,pixels,bytes,&written,NULL) && written==bytes);
    CloseHandle(target);SelectObject(dc,old_font);SelectObject(dc,old_image);
    DeleteObject(font);DeleteObject(image);DeleteDC(dc);
}
int wmain(int argc,WCHAR **argv)
{
    MONITOR_STATE state={0};DTASKMGR_WORKER *items=NULL;ULONG count=0,index;
    HANDLE output;DWORD worker_pid;common_task_trace_node *nodes;
    WORKER_TRACE_NODE *reply=NULL;ULONG coverage=0,total=0;
    monitor_trace_row order[TASK_TRACE_MAX_NODES];uint32_t live=0;
    if(argc!=3)return 2;worker_pid=wcstoul(argv[1],NULL,10);
    assert(!refresh(&state,&items,&count));
    for(index=0;index<count;++index)if(items[index].key.category==MANAGEMENT_WORKER &&
        items[index].process_id==worker_pid)break;
    assert(index<count && items[index].parent.category==MANAGEMENT_FRONTEND);
    state.selected_key=items[index].key;state.selected_row=index;state.status=ERROR_SUCCESS;
    common_rpc_management client={state.binding,state.process};
    assert(!common_rpc_worker_task_trace(&client,&state.selected_key,&coverage,&total,&reply));
    nodes=(common_task_trace_node *)reply;
    assert(monitor_trace_order(nodes,total,order,&live));
    for(uint32_t row=0;row<live;++row) {
        assert(nodes[order[row].index].state!=TASK_TRACE_EXITED);
        if(row && nodes[order[row].index].entered_order && nodes[order[row-1].index].entered_order)
            assert(nodes[order[row].index].entered_order>=nodes[order[row-1].index].entered_order);
    }
    output=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,CONSOLE_TEXTMODE_BUFFER,NULL);assert(output!=INVALID_HANDLE_VALUE);
    assert(SetConsoleWindowInfo(output,TRUE,&(SMALL_RECT){0,0,0,0}));
    assert(SetConsoleScreenBufferSize(output,(COORD){80,25}));
    render(output,&state,items,count);capture_bitmap(output,argv[2],L"main");
    assert(!handle_key(&state,items,count,VK_RETURN,0) && state.trace_key.category);
    render(output,&state,items,count);render_trace(output,&state);assert(state.trace_count==live);
    {
        WCHAR header[81]={0};DWORD copied=0;
        ULONG shown=live ? (live>MONITOR_MODAL_ROWS ? MONITOR_MODAL_ROWS : live) : 1;
        SHORT top=(SHORT)((MONITOR_ROWS-(shown+7))/2);
        assert(ReadConsoleOutputCharacterW(output,header,80,(COORD){0,top+1},&copied) && copied==80);
        assert(wcsstr(header,L"WORKER TASKS") && !wcsstr(header,L"Worker tasks"));
        assert(ReadConsoleOutputCharacterW(output,header,72,(COORD){4,(SHORT)(top+2)},&copied) && copied==72);
        for(unsigned column=0;column<72;++column)assert(header[column]==L' ');
        assert(ReadConsoleOutputCharacterW(output,header,80,(COORD){0,top+3},&copied) && copied==80);
        assert(wcsstr(header,L"ID") && wcsstr(header,L"METHOD") && wcsstr(header,L"TYPE") &&
            wcsstr(header,L"ELAPSED") && wcsstr(header,L"TASK") && !wcsstr(header,L"FILE"));
        assert(ReadConsoleOutputCharacterW(output,header,72,(COORD){4,(SHORT)(top+4+shown)},&copied) && copied==72);
        for(unsigned column=0;column<72;++column)assert(header[column]==L' ');
    }
    capture_bitmap(output,argv[2],L"modal");
    assert(!handle_key(&state,items,count,VK_DELETE,0) && !state.confirm_key.category);
    assert(!handle_key(&state,items,count,VK_ESCAPE,0) && !state.trace_key.category);
    CloseHandle(output);MIDL_user_free(reply);MIDL_user_free(items);
    RpcBindingFree(&state.binding);CloseHandle(state.process);
    printf("PASS live NTSRV snapshot, actual NTMON page/modal render, entry order and read-only keys; tasks=%lu\n",live);
    return 0;
}
