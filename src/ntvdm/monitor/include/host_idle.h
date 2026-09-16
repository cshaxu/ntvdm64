#ifndef ADAPTER_MVDM_MONITOR_HOST_IDLE_H
#define ADAPTER_MVDM_MONITOR_HOST_IDLE_H

/* Source-shaped historical idle notification. It conveys only that a host
 * activity occurred; it owns neither a CPU loop nor a BOP decision. */
void HostIdleNoActivity(void);

#endif
