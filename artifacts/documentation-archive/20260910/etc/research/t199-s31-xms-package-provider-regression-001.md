# M0 T199 S31: XMS Package Provider Regression

## Result

`src/bx-vdm/bx_ntvdm_xms_package_session_v1.c` is the single XMS package
router. It accepts only the common ingress record for `C4 C4 52 service`, a
real-mode CPU state, and the original XMS provider selection. It does not
enter Bochs directly or recognize any selector outside `bx-vdm`.

OpenNT's ordered owner is `base/mvdm/xms.486/xmsdisp.c`; the source-backed
handlers and present disposition are:

| `52:xx` | OpenNT handler | S31 disposition |
| --- | --- | --- |
| `00` | `xmsA20` | Resume through the typed mantle A20 capability. |
| `01` | `xmsMoveBlock` | Package-deferred: copied guest move structure and conventional-memory bridge are not admitted. |
| `02` | `xmsAllocBlock` | Resume through C2 allocation. The provider reserves the historical first 64 KiB before the first client allocation. |
| `03` | `xmsFreeBlock` | Resume through C2 release after exact source-shaped base/size lookup. |
| `04` | `xmsSysPageSize` | Package-deferred: the historical result is host page-size policy, not a Bochs device value. |
| `05` | `xmsQueryExtMem` | Resume through C2 capacity query. |
| `06..08` | UMB handlers | Package-deferred under C3; no UMB map is fabricated. |
| `09` | `xmsNotifyHookI15` | Package-deferred under C5; no keyboard/PIC/firmware callback is fabricated. |
| `0A` | `xmsQueryFreeExtMem` | Package-deferred; C2 intentionally exports no allocator-global free-space contract. |
| `0B` | `xmsReallocBlock` | Resume through C2 resize and update the opaque handle/base map; return the source-shaped `CX` base. |

`52:0C` remains non-callable and is rejected by the pre-existing classifier.
Deferred rows are explicit `PASS_THROUGH` dispositions from the one package
router, not success results or detached service handlers.

## A20 and allocation evidence

OpenNT `xmsa20.c` changes guest wrapping state, while `xms.c` constructs its
suballocator at `1 MiB + 64 KiB`. Its allocator is host/SAS-owned; it does not
become unable to allocate merely because a client requests 20-bit wrapping.

Bochs' `ordinary_ram_*` APIs rightly apply A20 for guest memory accesses, but
that made C2 reject the original XMS lifecycle. Registered exception
`BX-MANTLE-079` therefore adds only selector-blind checked physical-backing
RAM validation/copy primitives in `bx-core/memory`. They retain bounds,
memory-handler, and monitor rejection, expose no host pointer, and are used
only by the mantle-private C2 aperture. No BOP/XMS terminology enters that
core code.

## Reproducible evidence

1. The source-built `/W4 /WX` x64 unit at
   `artifacts/build/t199-s31-xms-session-r2/t199-s31-xms-session.exe` routes
   all twelve callable identities through one session: admitted identities
   return checked resumes and all seven unadmitted identities return the one
   deferred disposition.
2. The native CPU5/Pentium-MMX x64 `/MT` closure at
   `artifacts/build/t199-s31-xms-native-r5-backing/t197-s6-minimal-machine-link-probe.exe`
   initializes the minimal machine, disables A20 through `52:00`, then proves
   `52:02` reserves 64 KiB and returns `1088 KiB`, `52:0B` grows it and returns
   `1152 KiB` in `CX`, `52:03` frees it, and `52:05` reports `3072 KiB`.
3. No guest image was run and no trace was collected. This is a native
   lifecycle/package regression, not CLI runtime proof.

## Limits retained for the next package work

This closes the S31 package-router and admitted capability contract only. It
does not claim full historical XMS parity, a guest-memory move bridge, UMB,
INT15, or a native `#UD`/BOP runtime composition.
