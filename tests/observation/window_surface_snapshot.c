/* Read-only AMD64 frontend probe. Compile against the exact current frame ABI.
 * Snapshot is non-atomic, never acceptance by dimensions alone. No input sent. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/kvm-window/window.h"
typedef struct context_prefix {
    kvm_window *component;
    kvm_window_frame frame;
    void *dc,*bitmap,*previous;
    uint32_t *pixels,width,height;
} context_prefix;
static DWORD wanted;
static const char *output;
static int result=1;
static BOOL CALLBACK visit(HWND window,LPARAM unused) {
    DWORD pid;char kind[128];SIZE_T got;context_prefix *context;
    HANDLE process,file;void *pixels;DWORD size,written;
    (void)unused;
    GetWindowThreadProcessId(window,&pid);GetClassNameA(window,kind,sizeof(kind));
    if(pid!=wanted || strcmp(kind,"LibKvmWindow"))return TRUE;
    process=OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    if(!process)return TRUE;
    context=malloc(sizeof(*context));if(!context){CloseHandle(process);return FALSE;}
    ULONG_PTR address=(ULONG_PTR)GetWindowLongPtrA(window,GWLP_USERDATA);
    if(!address || !ReadProcessMemory(process,(void*)address,context,sizeof(*context),&got) || got!=sizeof(*context))goto done;
    printf("pid=%lu non-atomic=1 graphics=%u valid=%u width=%u height=%u context=%p pixels=%p frame-size=%zu\n",
        pid,context->frame.graphics,context->frame.valid,context->width,context->height,(void*)address,(void*)context->pixels,sizeof(context->frame));
    if(!context->pixels || !context->width || !context->height || context->width>4096 || context->height>4096)goto done;
    size=context->width*context->height*4;pixels=malloc(size);if(!pixels)goto done;
    if(ReadProcessMemory(process,context->pixels,pixels,size,&got)&&got==size) {
        BITMAPFILEHEADER header={0};BITMAPINFOHEADER info={0};
        header.bfType=0x4d42;header.bfOffBits=sizeof(header)+sizeof(info);header.bfSize=header.bfOffBits+size;
        info.biSize=sizeof(info);info.biWidth=context->width;info.biHeight=-(LONG)context->height;
        info.biPlanes=1;info.biBitCount=32;info.biSizeImage=size;
        file=CreateFileA(output,GENERIC_WRITE,0,NULL,CREATE_NEW,0,NULL);
        if(file!=INVALID_HANDLE_VALUE) {
            BOOL ok=WriteFile(file,&header,sizeof(header),&written,NULL)&&written==sizeof(header);
            ok=ok&&WriteFile(file,&info,sizeof(info),&written,NULL)&&written==sizeof(info);
            ok=ok&&WriteFile(file,pixels,size,&written,NULL)&&written==size;
            CloseHandle(file);if(ok)result=0;
        }
    }
    free(pixels);
done:free(context);CloseHandle(process);return TRUE;
}
int main(int argc,char **argv) {
    if(argc!=4 || !(wanted=strtoul(argv[1],NULL,10)))return 64;
    output=argv[3];
    HDESK desktop=OpenDesktopA(argv[2],0,FALSE,DESKTOP_READOBJECTS|DESKTOP_ENUMERATE);
    if(!desktop)return 65;
    EnumDesktopWindows(desktop,visit,0);CloseDesktop(desktop);return result;
}
