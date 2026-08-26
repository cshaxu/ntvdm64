#include <stdio.h>
#include "app/bop/route.h"
#include <string.h>
#include "app/bop/route.h"

#include "app/bop/dem_session.h"
#include "opennt-bop/dem/opennt_dem_dispatch_composition.h"
void MS_bop_0(void);
#include "app/bop/route.h"
#include "app/bop/dem_host_session.h"
#include "app/bop/route.h"
#include "app/bop/dem_entry.h"
#include "app/bop/route.h"

typedef struct fixture_state { uint8_t bytes[0x2500]; } fixture_state;

static int publish(void *s,HANDLE h,uint32_t *t,DWORD *e) { (void)s;(void)h;if(t)*t=0u;if(e)*e=ERROR_INVALID_HANDLE;return 0; }
static int lookup(void *s,uint32_t t,HANDLE *h) { (void)s;(void)t;if(h)*h=INVALID_HANDLE_VALUE;return 0; }
static int release(void *s,uint32_t t,DWORD *e) { (void)s;(void)t;if(e)*e=ERROR_INVALID_HANDLE;return 0; }
static int query(void *s,uint8_t d,const wchar_t *p,DWORD *a,DWORD *e) { (void)s;(void)d;(void)p;if(a)*a=0u;if(e)*e=ERROR_FILE_NOT_FOUND;return 0; }
static int set(void *s,uint8_t d,const wchar_t *p,DWORD a,DWORD *e) { (void)s;(void)d;(void)p;(void)a;if(e)*e=ERROR_FILE_NOT_FOUND;return 0; }
static int read_guest(void *s,uint32_t a,uint8_t *b,uint32_t n) { fixture_state *x=(fixture_state *)s;if(!x||a>sizeof(x->bytes)||n>sizeof(x->bytes)-a)return 0;memcpy(b,x->bytes+a,n);return 1; }
static int write_guest(void *s,uint32_t a,const uint8_t *b,uint32_t n) { fixture_state *x=(fixture_state *)s;if(!x||a>sizeof(x->bytes)||n>sizeof(x->bytes)-a)return 0;memcpy(x->bytes+a,b,n);return 1; }

static void event_initialize(struct runtime_generic_ud_event *event,
    uint8_t selector, uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic=RUNTIME_GENERIC_UD_EVENT_MAGIC;
    event->abi_version=RUNTIME_GENERIC_UD_EVENT_VERSION;
    event->struct_bytes=sizeof(*event); event->vector=6u;
    event->execution_mode=RUNTIME_CPU_EXECUTION_REAL;
    event->fault_rip=0x2400u; event->eip=0x2400u;
    event->window_bytes=4u; event->window[0]=0xc4u; event->window[1]=0xc4u;
    event->window[2]=selector; event->window[3]=service;
}

int main(void)
{
    static const uint8_t dem_bop[] = { 0xc4u, 0xc4u, 0x50u, 0x1fu };
    if (!runtime_bop_ingress_bind(runtime_opennt_bop_route_dispatch, 0)) return 90;
    fixture_state state={{0}}; runtime_dem_direct_host_session host;
    runtime_dem_native_session session; struct runtime_generic_ud_event event;
    struct runtime_generic_ud_outcome outcome;
    (void)state; (void)publish; (void)lookup; (void)release; (void)query; (void)set;
    memcpy(state.bytes + 0x2400u, dem_bop, sizeof(dem_bop));
    if(!runtime_dem_direct_host_session_initialize(&host) ||
       !runtime_dem_native_session_initialize(&session,
           runtime_dem_direct_host_session_context(&host), &state,
           read_guest, write_guest) ||
       !runtime_dem_native_session_bind(&session)) return 1;
    event_initialize(&event,0x50u,0x1fu); memset(&outcome,0,sizeof(outcome));
    event.eip = 0x2403u;
    if (!runtime_dem_native_session_invoke_scoped_body(&event, &outcome,
            MS_bop_0, 4u) || CurrentISVC != 0x1fu) return 11;
    event.eip = 0x2400u;
    memset(&outcome, 0, sizeof(outcome));
    if (!runtime_dem_generic_ud_recognizes(&event)) return 6;
    if (!runtime_dem_native_session_dispatch(&event,&outcome)) return 7;
    if (outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x2404u ||
        (outcome.eflags_values & RUNTIME_CPU_RESULT_EFLAGS_CF) != 0u) {
        return 8;
    }
    memset(&outcome, 0, sizeof(outcome));
    if(!runtime_machine_generic_ud_bridge(&event,&outcome) ||
       outcome.disposition!=RUNTIME_GENERIC_UD_RESUME || outcome.resume_rip!=0x2404u ||
       (outcome.eflags_values&RUNTIME_CPU_RESULT_EFLAGS_CF)!=0u) return 9;
    event_initialize(&event,0x54u,0x1fu);
    if(runtime_machine_generic_ud_bridge(&event,&outcome)) return 3;
    runtime_dem_native_session_unbind(&session);
    runtime_dem_direct_host_session_reset(&host);
    event_initialize(&event,0x50u,0x1fu);
    if(runtime_machine_generic_ud_bridge(&event,&outcome)) return 4;
    puts("T271 S2 DEM: copied #UD enters original MS_bop_0 then DemDispatch");
    return 0;
}
