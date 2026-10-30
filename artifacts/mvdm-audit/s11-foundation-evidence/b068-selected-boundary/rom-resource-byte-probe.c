#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 HMODULE m; HRSRC r; DWORD n,read; const BYTE *data; BYTE *file; HANDLE h;
 if(argc!=3)return 2;
 m=LoadLibraryExA(argv[1],NULL,LOAD_LIBRARY_AS_DATAFILE); if(!m)return 3;
 r=FindResourceA(m,"SOFTPC_BIOS1",RT_RCDATA); if(!r)return 4;
 n=SizeofResource(m,r); data=(const BYTE *)LockResource(LoadResource(m,r));
 h=CreateFileA(argv[2],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
 if(!data || n<0x30d4 || h==INVALID_HANDLE_VALUE || GetFileSize(h,NULL)!=n || !(file=malloc(n)))return 5;
 if(!ReadFile(h,file,n,&read,NULL) || read!=n || memcmp(file,data,n))return 6;
 printf("BIOS1_EMBEDDED_IDENTICAL bytes=%lu first=%02x%02x b=%02x%02x%02x w=%02x%02x%02x d=%02x%02x%02x%02x\n",n,
 data[0x3030],data[0x3031],data[0x30b0],data[0x30b1],data[0x30b2],
 data[0x30c0],data[0x30c1],data[0x30c2],data[0x30d0],data[0x30d1],data[0x30d2],data[0x30d3]);
 free(file);CloseHandle(h);FreeLibrary(m);return 0;
}
