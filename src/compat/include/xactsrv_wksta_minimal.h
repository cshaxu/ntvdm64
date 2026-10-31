/*
 * Finite standalone declaration boundary for the selected original XACTSRV
 * local-NetAPI handlers.
 *
 * This deliberately replaces XactSrvP.h, whose aggregate server includes
 * pull NT4 authentication and srv.sys FSCTL definitions into a function that
 * handlers which translate DOS/RAP records through public NetAPI calls.
 */
#ifndef _XACTSRV_LOCAL_MINIMAL_H_
#define _XACTSRV_LOCAL_MINIMAL_H_

#include <nt.h>
#include <windows.h>
#include <tstring.h>
#include <lm.h>
#include <winsvc.h>
#include <nettypes.h>
#include <smbtypes.h>
#include <smbmacro.h>
#include <smbgtpt.h>
#include <apiparam.h>
#include <remdef.h>
#include <remtypes.h>
#include <netlib.h>
#include <netdebug.h>
#include <rap.h>
#include <xstypes.h>
#include <xsprocs.h>

#include "xsconst.h"
#include "xsdata.h"
#include "xsdef16.h"
#include "xsparm16.h"
#include "xsprocsp.h"
#include "xsunicod.h"
#include "wkstadef.h"

#ifndef STATIC
#define STATIC static
#endif

#ifndef IF_DEBUG
#define IF_DEBUG(_x) if (0)
#endif

#ifndef API_HANDLER_PARAMETERS_REFERENCE
#define API_HANDLER_PARAMETERS_REFERENCE \
    UNREFERENCED_PARAMETER(Header); \
    UNREFERENCED_PARAMETER(AuxStructureDesc)
#endif

#endif
