/* Direct source mirror of softpc.new/host/src/nt_bop.c:MS_bop_E.
 * The historical product dispatcher is intentionally not linked; bx-vdm
 * recognizes the selector and scopes this source body through a copied CPU
 * call.  UMBNotify remains the original no-op. */
#include "opennt-bop/dem/opennt_dem_ccpu_sas_facade.h"

void demDasdInit(void);
void UMBNotify(unsigned char code);

void MS_bop_E(void)
{
   unsigned char code;

   code = (unsigned char)getAL();
   if (code == 0) {
       UMBNotify(0);
       demDasdInit();
       }
}
