/* Compile the exact original helper body selected by the fixture generator. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd-temp-body.inc"
#define CHECK(x) do{if(!(x)){printf("CMD_TEMP_FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
int main(void)
{
 char temp[MAX_PATH],directory[MAX_PATH],*path=NULL,buffer[4]={0};
 HANDLE file=INVALID_HANDLE_VALUE,second;DWORD flags,bytes;
 CHECK(GetTempPathA(MAX_PATH,temp)&&GetTempFileNameA(temp,"ctf",0,directory));
 CHECK(DeleteFileA(directory)&&CreateDirectoryA(directory,NULL));
 CHECK(SetEnvironmentVariableA("TMP",directory)&&SetEnvironmentVariableA("TEMP",directory));
 CHECK(cmdCreateTempFile(&file,&path)&&file!=INVALID_HANDLE_VALUE&&path);
 CHECK(!_strnicmp(path,directory,strlen(directory))&&path[strlen(directory)]=='\\');
 CHECK(GetHandleInformation(file,&flags)&&(flags&HANDLE_FLAG_INHERIT));
 CHECK(WriteFile(file,"abc",3,&bytes,NULL)&&bytes==3);
 second=CreateFileA(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
 CHECK(second!=INVALID_HANDLE_VALUE);
 CHECK(ReadFile(second,buffer,3,&bytes,NULL)&&bytes==3&&!memcmp(buffer,"abc",3));
 CHECK(CloseHandle(second)&&CloseHandle(file));CHECK(DeleteFileA(path));free(path);
 CHECK(RemoveDirectoryA(directory));puts("CMD_ORIGINAL_TEMP_PATH_INHERIT_SHARED_RW_CLEANUP_OK");return 0;
}
