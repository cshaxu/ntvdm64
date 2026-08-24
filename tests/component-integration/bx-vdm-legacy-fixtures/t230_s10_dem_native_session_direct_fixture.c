#include <stdio.h>
#include "opennt-bop/ingress/opennt_bop_route.h"
#include <string.h>
#include "opennt-bop/ingress/opennt_bop_route.h"

#include "opennt-bop/ingress/dem_native_session.h"
#include "opennt-bop/ingress/opennt_bop_route.h"
#include "opennt-bop/ingress/dem_direct_session.h"
#include "opennt-bop/ingress/opennt_bop_route.h"
#include "opennt-bop/ingress/dem_v2_generic_ud_bridge.h"
#include "opennt-bop/ingress/opennt_bop_route.h"

typedef struct fixture_state { uint8_t bytes[32]; } fixture_state;

static int publish(void *s,HANDLE h,uint32_t *t,DWORD *e) { (void)s;(void)h;if(t)*t=0u;if(e)*e=ERROR_INVALID_HANDLE;return 0; }
static int lookup(void *s,uint32_t t,HANDLE *h) { (void)s;(void)t;if(h)*h=INVALID_HANDLE_VALUE;return 0; }
static int release(void *s,uint32_t t,DWORD *e) { (void)s;(void)t;if(e)*e=ERROR_INVALID_HANDLE;return 0; }
static int query(void *s,uint8_t d,const wchar_t *p,DWORD *a,DWORD *e) { (void)s;(void)d;(void)p;if(a)*a=0u;if(e)*e=ERROR_FILE_NOT_FOUND;return 0; }
static int set(void *s,uint8_t d,const wchar_t *p,DWORD a,DWORD *e) { (void)s;(void)d;(void)p;(void)a;if(e)*e=ERROR_FILE_NOT_FOUND;return 0; }
static int read_guest(void *s,uint32_t a,uint8_t *b,uint32_t n) { fixture_state *x=(fixture_state *)s;if(!x||a>sizeof(x->bytes)||n>sizeof(x->bytes)-a)return 0;memcpy(b,x->bytes+a,n);return 1; }
static int write_guest(void *s,uint32_t a,const uint8_t *b,uint32_t n) { fixture_state *x=(fixture_state *)s;if(!x||a>sizeof(x->bytes)||n>sizeof(x->bytes)-a)return 0;memcpy(x->bytes+a,b,n);return 1; }

static void event_initialize(struct runtime_generic_ud_event_v1 *event,
    uint8_t selector, uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic=RUNTIME_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version=RUNTIME_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes=sizeof(*event); event->vector=6u;
    event->execution_mode=RUNTIME_CPU_EXECUTION_REAL;
    event->fault_rip=0x2400u; event->eip=0x2400u;
    event->window_bytes=4u; event->window[0]=0xc4u; event->window[1]=0xc4u;
    event->window[2]=selector; event->window[3]=service;
}

int main(void)
{
    if (!runtime_bop_ingress_v1_bind(runtime_opennt_bop_route_dispatch_v1, 0)) return 90;
    fixture_state state={{0}}; runtime_dem_direct_host_session host;
    runtime_dem_native_session session; struct runtime_generic_ud_event_v1 event;
    struct runtime_generic_ud_outcome_v1 outcome;
    (void)state; (void)publish; (void)lookup; (void)release; (void)query; (void)set;
    if(!runtime_dem_direct_host_session_initialize(&host) ||
       !runtime_dem_native_session_initialize(&session,
           runtime_dem_direct_host_session_context(&host), &host,
           runtime_dem_direct_host_session_guest_read,
           runtime_dem_direct_host_session_guest_write) ||
       !runtime_dem_native_session_bind(&session)) return 1;
    event_initialize(&event,0x50u,0x1fu); memset(&outcome,0,sizeof(outcome));
    if(!runtime_mantle_generic_ud_bridge_v1(&event,&outcome) ||
       outcome.disposition!=RUNTIME_GENERIC_UD_RESUME || outcome.resume_rip!=0x2404u ||
       (outcome.eflags_values&RUNTIME_CPU_RESULT_V2_EFLAGS_CF)!=0u) return 2;
    event_initialize(&event,0x54u,0x1fu);
    if(runtime_mantle_generic_ud_bridge_v1(&event,&outcome)) return 3;
    runtime_dem_native_session_unbind(&session);
    runtime_dem_direct_host_session_reset(&host);
    event_initialize(&event,0x50u,0x1fu);
    if(runtime_mantle_generic_ud_bridge_v1(&event,&outcome)) return 4;
    puts("T230 Direct DEM v2 composition: copied #UD reaches original dispatcher without v1 fallback");
    return 0;
}
