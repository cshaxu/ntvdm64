/* Independent modern-USER negative control for the real WOW modal witness.
 * Same captions, buttons, desktop protocol and expected native return values;
 * no WOW, style DLL, guest or product execution adapter is loaded. */
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
int main(void)
{
    int box = MessageBoxA(NULL, "Original guest modal return witness",
        "T447 MessageBox scope", MB_OK);
    int about;
    printf("NATIVE_CONTROL MessageBox=%d\n", box); fflush(stdout);
    if (box != IDOK) return 20;
    about = ShellAboutA(NULL, "T447 Modal scope",
        "Original guest modal return witness", NULL);
    printf("NATIVE_CONTROL ShellAbout=%d\n", about); fflush(stdout);
    return about == 1 ? 0 : 21;
}
