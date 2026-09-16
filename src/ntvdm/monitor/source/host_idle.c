#include "host_idle.h"

/*
 * DIVERGENCE(ADAPTER-MONITOR-006): OpenNT's nt_unix.c implementation cleared
 * IdleNoActivity and pulsed the CCPU IdleEvent only while a historical CCPU
 * waiter was active. The earlier recovery product deliberately had no CCPU
 * executor or that idle event. Keep the original public notification spelling
 * as a harmless current-session activity boundary; no scheduler, BOP or
 * machine state is created here.
 */
void HostIdleNoActivity(void)
{
}
