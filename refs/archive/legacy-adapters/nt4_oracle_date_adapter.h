#ifndef NT4_ORACLE_DATE_ADAPTER_H
#define NT4_ORACLE_DATE_ADAPTER_H

#include "owned_clock_snapshot.h"
#include "owned_monitor_m1.h"

typedef struct nt4_oracle_date_adapter {
    owned_clock_snapshot snapshot;
} nt4_oracle_date_adapter;

int nt4_oracle_date_adapter_initialize(
    nt4_oracle_date_adapter *adapter,
    const owned_clock_snapshot *snapshot);
owned_monitor_m1_result nt4_oracle_date_adapter_register(
    owned_monitor_m1 *registry,
    nt4_oracle_date_adapter *adapter);

#endif
