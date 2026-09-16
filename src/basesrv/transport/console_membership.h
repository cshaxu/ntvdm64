#ifndef BROKER_CONSOLE_MEMBERSHIP_H
#define BROKER_CONSOLE_MEMBERSHIP_H
#include <windows.h>
/* Only an isolated, initially detached helper may call this function.
 * IDs come from an authenticated owner's pinned live process records; this
 * function supplies membership observation, NOT identity or reuse authority.
 * Owner revalidates lifetimes/generations after return. Output is unchanged
 * on failure. No unrelated PID is opened, controlled or returned. */
DWORD broker_console_membership(DWORD caller,const DWORD *candidates,DWORD count,BYTE *members);
#endif
