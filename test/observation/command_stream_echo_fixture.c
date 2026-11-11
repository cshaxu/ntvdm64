/* Test-only S34 native child.  Original COMMAND must propagate its guest
 * standard input to this Win32 child and preserve its stdout through the
 * same product stream path. */
#include <windows.h>
#include <string.h>

int main(void)
{
    char input[80];
    char output[128];
    HANDLE standard_input=GetStdHandle(STD_INPUT_HANDLE);
    HANDLE standard_output=GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD read=0,written=0;
    int length;

    if (standard_input==NULL || standard_input==INVALID_HANDLE_VALUE ||
        standard_output==NULL || standard_output==INVALID_HANDLE_VALUE ||
        !ReadFile(standard_input,input,sizeof(input)-1u,&read,NULL) || !read)
        return 1;
    input[read]='\0';
    length=wsprintfA(output,"S34_NATIVE_CHILD_STDIN=%s",input);
    return length>0 && WriteFile(standard_output,output,(DWORD)length,&written,NULL) &&
        written==(DWORD)length ? 0 : 2;
}
