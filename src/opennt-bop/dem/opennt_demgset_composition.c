/* Project-owned BOP composition and scoped state seam for the direct OpenNT demgset.c
 * import.  Original source: src/opennt/base/mvdm/dos/dem/demgset.c.
 *
 * The direct wrappers deliberately call the corresponding user-mode Win32
 * APIs.  They do not create a virtual drive model or carry legacy-v1 policy.
 */

#include "opennt_demgset_composition.h"

void demSetDefaultDrive(void);
void demGetBootDrive(void);
void demGetDriveFreeSpace(void);
void demGetDrives(void);
void demQueryDate(void);
void demQueryTime(void);
void demSetDate(void);
void demSetTime(void);
void demSetDTALocation(void);
void demGSetMediaID(void);
void demGetDPB(void);
void demGetComputerName(void);
void demGetDPBList(void);

/* Direct composition uses the real Win32 clock API.  A scoped test writer is
 * intentionally opt-in and exists only to avoid mutating the host clock in a
 * local fixture; it is not a mutation-profile backend. */
static __declspec(thread) runtime_demgset_clock_writer g_clock_writer;
BOOL runtime_demgset_set_local_time(const SYSTEMTIME *time)
{ return g_clock_writer != NULL ? g_clock_writer(time) : SetLocalTime(time); }
void runtime_demgset_set_clock_writer(runtime_demgset_clock_writer writer)
{ g_clock_writer = writer; }

/* demGetBDS/demGetBPB are now owned by the directly imported OpenNT
 * demdasd.c mirror (src/opennt/base/mvdm/dos/dem/demdasd.c).  Keeping the
 * former S6 synthetic fallback here would replace the original owner and
 * collide at link time, so it is intentionally absent. */

/* Persistent, typed replacement for the original CCPU/SAS pointer aliases
 * installed by demSetDTALocation.  The imported search owner consumes the
 * preserved DOS far-pointer word pair and current PDB value on later calls. */
static __declspec(thread) ULONG g_dta_location;
static __declspec(thread) USHORT g_current_pdb;
static __declspec(thread) runtime_demhndl_extended_error g_extended_error;

int runtime_demgset_register_dta(USHORT ds, USHORT dta_offset,
    USHORT pdb_offset, USHORT error_offset, USHORT wow_offset)
{
    ULONG dta_location;
    USHORT current_pdb;
    (void)error_offset;
    (void)wow_offset;
    if (!runtime_demhndl_copy_guest(ds, dta_offset, &dta_location,
            sizeof(dta_location)) ||
        !runtime_demhndl_copy_guest(ds, pdb_offset, &current_pdb,
            sizeof(current_pdb))) {
        SetLastError(ERROR_INVALID_ADDRESS);
        return 0;
    }
    g_dta_location = dta_location;
    g_current_pdb = current_pdb;
    memset(&g_extended_error, 0, sizeof(g_extended_error));
    pulDTALocation = &g_dta_location;
    pusCurrentPDB = (PUSHORT)&g_current_pdb;
    pExtendedError = &g_extended_error;
    return 1;
}

int runtime_demgset_invoke(runtime_demhndl_call *call)
{
    void (*body)(void) = NULL;
    if (!runtime_demhndl_call_valid(call)) return 0;
    switch (call->service) {
    case 0x0du: body = demGetBootDrive; break;
    case 0x0eu: body = demGetDriveFreeSpace; break;
    case 0x0fu: body = demGetDrives; break;
    case 0x10u: body = demGSetMediaID; break;
    case 0x14u: body = demQueryDate; break;
    case 0x15u: body = demQueryTime; break;
    case 0x19u: body = demSetDate; break;
    case 0x1au: body = demSetDefaultDrive; break;
    case 0x1bu: body = demSetDTALocation; break;
    case 0x1cu: body = demSetTime; break;
    case 0x25u: body = demGetDPB; break;
    case 0x41u: body = demGetComputerName; break;
    case 0x46u: body = demGetDPBList; break;
    default: return 0;
    }
    return runtime_demhndl_invoke_body(call, body);
}
