#include <windows.h>
#include <stdio.h>
#include "lib-win32/file/identity.h"
#define CHECK(x) do { ++checks; if(!(x)){fprintf(stderr,"file identity failure line %d error=%lu\n",__LINE__,GetLastError());result=1;goto done;} } while(0)
int main(void)
{
    WCHAR directory[MAX_PATH]={0},first[MAX_PATH]={0},second[MAX_PATH]={0},alias[MAX_PATH]={0};
    BOOL same=TRUE;int checks=0,result=0;
    CHECK(GetTempPathW(MAX_PATH,directory)>0);
    CHECK(GetTempFileNameW(directory,L"f1",0,first)!=0);
    CHECK(GetTempFileNameW(directory,L"f2",0,second)!=0);
    CHECK(GetTempFileNameW(directory,L"fa",0,alias)!=0);
    CHECK(DeleteFileW(alias));
    CHECK(CreateHardLinkW(alias,first,NULL));
    CHECK(win32_file_same(first,first,&same)==ERROR_SUCCESS && same);
    CHECK(win32_file_same(first,alias,&same)==ERROR_SUCCESS && same);
    CHECK(win32_file_same(first,second,&same)==ERROR_SUCCESS && !same);
    CHECK(DeleteFileW(alias));alias[0]=0;
    CHECK(DeleteFileW(second));
    CHECK(win32_file_same(first,second,&same)!=ERROR_SUCCESS && !same);second[0]=0;
    CHECK(win32_file_same(NULL,first,&same)==ERROR_INVALID_PARAMETER && !same);
    CHECK(win32_file_same(first,first,NULL)==ERROR_INVALID_PARAMETER);
done:
    if(alias[0])DeleteFileW(alias);
    if(second[0])DeleteFileW(second);
    if(first[0])DeleteFileW(first);
    if(!result)printf("PASS Win32 file identity checks=%d\n",checks);
    return result;
}
