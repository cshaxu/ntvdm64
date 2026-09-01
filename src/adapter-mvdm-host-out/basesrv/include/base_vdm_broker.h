#ifndef BASE_VDM_BROKER_H
#define BASE_VDM_BROKER_H

#include <stdint.h>

#include "base_vdm_local.h"
#include "broker/base_vdm_record.h"

typedef struct session session;

#define BASE_VDM_BROKER_VERSION UINT32_C(1)

enum base_vdm_broker_delivery {
    BASE_VDM_BROKER_DELIVERY_ERROR = -1,
    BASE_VDM_BROKER_DELIVERY_PENDING = 0,
    BASE_VDM_BROKER_DELIVERY_COMPLETE = 1
};

/* Adapter-owned binding between a selected original BaseVDM command producer
 * and the transport-neutral broker record. It has no MVDM pointer, HANDLE,
 * guest address or mapping-manager field. */
typedef struct base_vdm_broker {
    uint32_t version;
    uint32_t struct_bytes;
    session *owner;
    uint32_t broker_id;
    uint32_t session_id;
    uint32_t open;
    broker_base_vdm_state records;
} base_vdm_broker;

void base_vdm_broker_initialize(base_vdm_broker *binding);
int base_vdm_broker_bind(base_vdm_broker *binding, session *owner,
    uint32_t broker_id);
int base_vdm_broker_publish(base_vdm_broker *binding,
    const base_vdm_command *command);
int base_vdm_broker_deliver(base_vdm_broker *binding,
    base_vdm_local *destination);
int base_vdm_broker_unbind(base_vdm_broker *binding);

#endif
