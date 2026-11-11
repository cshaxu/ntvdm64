/* Diagnostic only: native presence-query semantics used by observer flags. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv)
{
    char value[32]={0};DWORD needed,error,read;BOOL empty,pass;
    if(argc!=2 || (strcmp(argv[1],"empty") && strcmp(argv[1],"absent")))return 2;
    empty=!strcmp(argv[1],"empty");SetLastError(0);
    needed=GetEnvironmentVariableA("MVDM_TEST_EMPTY_ENV_PROBE",NULL,0);error=GetLastError();
    read=GetEnvironmentVariableA("MVDM_TEST_EMPTY_ENV_PROBE",value,sizeof(value));
    pass=!read && (empty ? needed==1 && !error : !needed && error==ERROR_ENVVAR_NOT_FOUND);
    printf("%s native environment %s needed=%lu error=%lu read=%lu value=[%s]\n",
        pass ? "PASS" : "FAIL",argv[1],needed,error,read,value);
    return pass ? 0 : 1;
}
