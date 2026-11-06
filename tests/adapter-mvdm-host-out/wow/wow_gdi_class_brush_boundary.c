#include <windows.h>
#include <stdio.h>
#include "ntvdm-exe/wow/include/wow_gdi_alias.h"
int main(void) {
 HBRUSH native=CreateSolidBrush(RGB(128,128,128));
 WORD alias; HBRUSH decoded; int collision;
 if(!native) return 2;
 alias=mvdm_wow_gdi_handle_from_native(native);
 /* Same threshold/decision as current GETWNDCLASS16; no product patch. */
 decoded=alias>COLOR_INFOBK ? (HBRUSH)mvdm_wow_gdi_handle_to_native(alias) : (HBRUSH)(ULONG_PTR)alias;
 collision=alias!=0 && decoded!=native;
 printf("alias=%u color_end=%u native=%p class_brush=%p collision=%d\n",alias,COLOR_INFOBK,(void*)native,(void*)decoded,collision);
 mvdm_wow_gdi_handle_retire(alias);DeleteObject(native);
 return collision ? 1 : 0;
}
