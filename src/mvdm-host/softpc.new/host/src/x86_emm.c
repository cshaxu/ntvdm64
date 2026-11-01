/*                      INSIGNIA MODULE SPECIFICATION
                        -----------------------------

MODULE NAME     : 'Lower layer' of Expanded Memory Manager

        THIS PROGRAM SOURCE FILE  IS  SUPPLIED IN CONFIDENCE TO THE
        CUSTOMER, THE CONTENTS  OR  DETAILS  OF  ITS OPERATION MUST
        NOT BE DISCLOSED TO ANY  OTHER PARTIES  WITHOUT THE EXPRESS
        AUTHORISATION FROM THE DIRECTORS OF INSIGNIA SOLUTIONS INC.

DESIGNER        : Simon Frost
DATE            : March '92

PURPOSE         : NT specific code for EMS LIM rev 4.0
                implementation.

The Following Routines are defined:
                1. host_initialise_EM()
                2. host_deinitialise_EM()
                3. host_allocate_storage()
                4. host_free_storage()
                5. host_reallocate_storage()
                6. host_map_page()
                7. host_unmap_page()
                8. host_alloc_page()
                9. host_free_page()
                10. host_copy_con_to_con()
                11. host_copy_con_to_EM()
                12. host_copy_EM_to_con()
                13. host_copy_EM_to_EM()
                14. host_exchg_con_to_con()
                15. host_exchg_con_to_EM()
                16. host_exchg_EM_to_EM()
                17. host_get_access_key()

=========================================================================
*/
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>

#include "insignia.h"
#include "host_def.h"
#include "string.h"
#include "stdlib.h"

#ifdef LIM
#ifdef MONITOR  //x86 specific LIM functions
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#endif /* MONITOR */
#endif /* LIM */
