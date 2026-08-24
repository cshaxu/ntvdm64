/* Target-local historical include redirect.  OpenNT's winioctl.h contains the
 * NT4 disk declarations used by nt_fdisk/nt_rflop; this is selected only after
 * their source has included the fixed historical nt.h declaration set. */
#ifndef OPENNT_DISK_BACKEND_NTDDDISK_H
#define OPENNT_DISK_BACKEND_NTDDDISK_H

/* nt_fdisk.c includes windows.h immediately after this historical header.
 * The redirect reaches winioctl.h directly, so retain that required Win32
 * scalar-type preamble at the same target-local include boundary. */
#include <windows.h>
#include <winioctl.h>

#endif
