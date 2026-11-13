/* CCPU-profile setup for the C-VID fixture's real SAS execution checks. */
#include "insignia.h"
#include "host_def.h"
#include "sas.h"
#include "gmi.h"
#include "ntvdm-exe/session/session.h"
extern void sas_init(PHY_ADDR), sas_term(void), c_cpu_init(void);
extern int mvdm_softpc_guest_memory_begin(session *);
extern void mvdm_softpc_guest_memory_end(session *);
extern MEM_HANDLERS Glue_writes;
static session owner;
int cvidc_sas_fixture_begin(void)
{
    session_initialize(&owner, 1);
    if (!session_activate(&owner) || !session_thread_bind(&owner)) return 0;
    sas_init(0x200000);
    if (!mvdm_softpc_guest_memory_begin(&owner)) return 0;
    c_cpu_init();
    gmi_define_mem(VIDEO, &Glue_writes);
    c_sas_connect_memory(0xa0000, 0xbffff, SAS_VIDEO);
    return 1;
}
void cvidc_sas_fixture_end(void)
{
    mvdm_softpc_guest_memory_end(&owner);
    sas_term();
}
