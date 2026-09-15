/* Shared product identity for run16.exe, basesrv.exe and ntvdm.exe.
 * Advance the application version with the admitted T task, not each S/P.
 * Protocol changes also require the matching major version in service.idl.
 * This header supplies metadata only, never application policy to adapters. */
#ifndef NTVDM_APP_VERSION_H
#define NTVDM_APP_VERSION_H
#define APP_VERSION "0.0.412"
#define APP_PROTOCOL_VERSION 3u
#define APP_VERSION_BYTES 32u
#endif
