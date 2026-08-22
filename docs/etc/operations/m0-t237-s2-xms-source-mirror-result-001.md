# M0 T237 S2 — XMS source-mirror recovery result

## Result

The complete OpenNT `52:00..0B` package is now mirrored under
`src/bx-vdm/bop/opennt/xms/`: `xms.c`, `xmsa20.c`, `xmsblock.c`,
`xmsdisp.c`, `xmsmisc.c`, `xmsumb.c`, and the original `suballoc` policy.
The sole adaptation surface is `bop/shim/xms_shim.*`, with a typed native
session and selector recognizer in `bop/xms_v2_*`.  No Bochs/core/mantle file
recognizes an XMS BOP.

## Disposition

| Services | Result |
| --- | --- |
| `52:01..05`, `52:07..08`, `52:0A..0B` | Direct mirrored OpenNT dispatcher/service body, with checked guest RAM and source suballocator callbacks. |
| `52:00` | Direct mirrored `xmsa20.c` when the mantle A20 lifecycle is active; otherwise explicit decline, because the source assumes SAS availability and supplies no failure-register alternative. |
| `52:06` | Explicit decline pending a mantle-owned UMB-reservation result and a checked durable guest byte for the source's `AX:BX` HIMEM-A20-state publication. |
| `52:09` | Explicit decline pending the keyboard/BIOS owner's `UpdateKbdInt15` mechanism; the former silent no-op is removed. |

OpenNT `config.c` proves that XMS capacity requires a whole-machine size of
XMS + EMS + conventional RAM.  The current 1 MiB mantle stage therefore is
not bound as a false XMS session.  This package result is not a claim of
native guest continuity or a completed extended-RAM machine aperture.

## Verification

Fresh formal graph `build/M0-T237-S2/004` compiled all mirrored sources and
the fixed-width bridge under MSVC x64 `/MT`.  The fixture initially exposed a
missing declaration of the public `user32.lib` dependency in the shared
source-mirror library; that import is now recorded in the formal manifest.
Linking the resulting formal objects with that declared library and running
`t237-s2-xms-source-mirror-fixture.exe` returned zero.  The fixture covers
every service: source results for `01..05`, `07..08`, `0A..0B`, and explicit
unavailability for `00`, `06`, `09`.

The regenerated S3 graph additionally proves one bounded product route:
copied `C4 C4 52 05` generic-UD ingress reaches `XMSDispatch`, returns the
source `AX=8192` value and advances exactly four bytes.  It has no dependence
on a fabricated extended-memory aperture.

The fresh `build/M0-T237-S3/002` graph additionally builds and runs
`t237-s3-xms-a20-source-mirror-fixture.exe`.  That fixture creates a real
16 MiB minimal mantle machine, binds the 8192 KiB source XMS session, and
drives `C4 C4 52 00` through the common opaque bridge.  The original
`xmsa20.c` bodies turn wrapping on and off, and their source query result
matches the mantle A20 capability after each transition.  This proves the
admitted success path; it does not claim UMB/INT15 or guest-startup closure.

S3 regenerates the graph after the manifest correction and removes all XMS v1
inputs before making the final verification claim.
