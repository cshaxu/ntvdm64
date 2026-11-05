#include "t198_s121_dem_lifecycle_ledger.h"
#include <string.h>
void t198_s121_dem_lifecycle_ledger_reset(struct t198_s121_dem_lifecycle_ledger *ledger) { if (ledger) memset(ledger,0,sizeof(*ledger)); }
void t198_s121_dem_lifecycle_ledger_record(struct t198_s121_dem_lifecycle_ledger *ledger,
 const struct runtime_generic_ud_event *event,const struct runtime_generic_ud_outcome *outcome,int accepted,
 const runtime_boot_namespace_diagnostic *d) {
 struct t198_s121_dem_lifecycle_entry *e; uint8_t s;
 if(!ledger||!event||!d||ledger->count>=T198_S121_DEM_LIFECYCLE_LEDGER_CAPACITY||event->window_bytes<4u||event->window[0]!=0xc4u||event->window[1]!=0xc4u||event->window[2]!=0x50u) return;
 s=event->window[3]; if(s!=0x12u&&s!=0x02u&&s!=0x42u)return;
 e=&ledger->entries[ledger->count++]; e->service=s;e->accepted=accepted?1u:0u;e->cs=event->cs;e->eip=event->eip;e->token=((event->eax&0xffffu)<<16)|(event->ebp&0xffffu);e->namespace_open=d->open;e->namespace_file_index=d->open_file_index;e->namespace_generation=d->generation;e->namespace_offset=d->offset;
 if(accepted&&outcome){e->disposition=outcome->disposition;e->ax=outcome->gpr16_values[0];e->bp=outcome->gpr16_values[5];}
 }
int t198_s121_dem_lifecycle_ledger_get(const struct t198_s121_dem_lifecycle_ledger *ledger,uint32_t index,struct t198_s121_dem_lifecycle_entry *entry){if(!ledger||!entry||index>=ledger->count)return 0;*entry=ledger->entries[index];return 1;}
