#include <windows.h>
#include <stdio.h>
typedef WORD (WINAPI *get_word)(HWND,int);
typedef WORD (WINAPI *set_word)(HWND,int,WORD);
int main(void) {
 WNDCLASSA c={0};HWND w;int bytes;WORD old,v;DWORD se,ge;HMODULE user=GetModuleHandleA("user32.dll");
 get_word get=(get_word)GetProcAddress(user,"GetWindowWord");set_word set=(set_word)GetProcAddress(user,"SetWindowWord");
 if(!get||!set)return 2;
 for(bytes=2;bytes<=6;bytes+=2){
 c.lpfnWndProc=DefWindowProcA;c.hInstance=GetModuleHandleA(NULL);c.lpszClassName="T478_NATIVE_WORD";c.cbWndExtra=bytes;
 if(!RegisterClassA(&c))return 3;
 w=CreateWindowExA(0,c.lpszClassName,"",WS_POPUP,0,0,10,10,NULL,NULL,c.hInstance,NULL);if(!w)return 4;
 SetLastError(0);old=set(w,0,0x5a39);se=GetLastError();SetLastError(0);v=get(w,0);ge=GetLastError();
 printf("NATIVE_WORD extra=%d reported=%ld previous=%u value=%04x set_error=%lu get_error=%lu\n",bytes,GetClassLongA(w,GCL_CBWNDEXTRA),old,v,se,ge);
 if(v!=0x5a39)return 5;
 {
 HWND short_window=(HWND)(INT_PTR)(SHORT)(UINT_PTR)w;
 SetLastError(0);old=set(short_window,0,0x6b4a);se=GetLastError();SetLastError(0);v=get(short_window,0);ge=GetLastError();
 printf("SHORT_WORD hwnd=%p short=%p previous=%04x value=%04x full_value=%04x set_error=%lu get_error=%lu\n",w,short_window,old,v,get(w,0),se,ge);
 }
 if(!DestroyWindow(w)||!UnregisterClassA(c.lpszClassName,c.hInstance))return 6;
 }
 return 0;
}
