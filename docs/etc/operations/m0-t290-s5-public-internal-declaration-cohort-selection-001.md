# M0 T290 S5 — public-internal declaration cohort selection

## Decision

Select two complete, declaration-only OpenNT public-internal packages for the
original `vdmredir/vrnetapi.c` compilation closure:

- `public/internal/base/inc` →
  `mvdm-platform-abi/source/public/internal/base/inc` (59 files);
- `public/internal/net/inc` →
  `mvdm-platform-abi/source/public/internal/net/inc` (12 files).

The first reached dependency is `smbgtpt.h` from the base package and the
second is `tstring.h` from the network package. The source is selected at
package scope so that `rxp.h` retains its historical declaration cohort rather
than accumulating arbitrary same-name headers.

## Evidence and selection

- `smbgtpt.h` is byte-identical to the corresponding
  OpenNT-4.5 `nt/private/inc/smbgtpt.h`.
- `tstring.h` is byte-identical to the corresponding OpenNT-4.5
  `nt/private/net/inc/tstring.h`.
- The full OpenNT public packages have 59 and 12 files respectively. Their
  older private counterparts are wider product trees (97 and 102 files), so
  they are not interchangeable whole-package supersets.
- The OpenNT public-internal lineage is selected because it is the direct
  include cohort of the already selected original DS package. This does not
  admit a private base/network service, RPC implementation, server, or host
  runtime route.

## Boundary

All files remain exact declaration carriers in `mvdm-platform-abi`. The
compilation probe may reveal further historical declaration packages; each
must receive the same package-scope selection before it is added. No
Redirector selector, NetAPI provider route, private transaction broker, or
new compatibility implementation is enabled by this import.

The accompanying manifest records every selected source path and SHA-256.
