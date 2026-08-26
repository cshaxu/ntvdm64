/* Staging-only compilation preamble. The OpenNT DEM headers assume the
 * historical build environment has already established Win32 base types. */
#ifndef STAGE_PREAMBLE_H
#define STAGE_PREAMBLE_H

#include <windows.h>
#include <winioctl.h>

/* DEM owns an internal helper named GetDiskSpaceInformation.  The current SDK
 * redirects that spelling to an unrelated Win32 API macro. */
#ifdef GetDiskSpaceInformation
#undef GetDiskSpaceInformation
#endif

/* OpenNT's release ntrtl.h makes ASSERT a no-op. Some DEM sources rely on
 * that historical precompiled-header ordering, so reproduce only that PROD
 * behavior in the isolated staging compile. */
#if defined(PROD) && !defined(ASSERT)
#define ASSERT(exp) ((void)0)
#endif

#endif
