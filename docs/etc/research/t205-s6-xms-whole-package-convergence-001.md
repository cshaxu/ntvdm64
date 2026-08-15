# M0 T205 S6 — XMS whole-package convergence

## Question

Does one source-built XMS package session cover every callable OpenNT XMS BOP
identity with an explicit disposition, preserve the original `XMS_LASTSVC`
boundary, and avoid returning machine-gated services as raw `#UD`?

## Inputs

| Input | SHA-256 | Role |
| --- | --- | --- |
| `src/opennt/base/mvdm/xms.486/xmsdisp.c` | `24B0FA2FE32BD7A94606D424A10B0961A9911190CB2684456AF04034E04BFF01` | Original 12-entry dispatcher order. |
| `src/opennt/base/mvdm/inc/xmssvc.h` | `E2661ECF606B903E4C3053C4E6FE8197F4F4B15DDE81393FB6FD508F09BCCEBE` | `XMS_LASTSVC=0x0C` bounds the callable table. |
| `src/bx-vdm/bx_ntvdm_xms_package_session_v1.c` | Current worktree | One package-session route and typed deferred outcome. |
| `src/bx-vdm/bx_ntvdm_bop_ingress_v1.c` | Current worktree | Common copied selector/service classifier. |
| `tests/bx-vdm/bx_ntvdm_xms_package_session_v1_test.c` | Current worktree | Whole-family source-built regression. |

## Procedure

1. Reconcile the OpenNT dispatcher table with the twelve current callable
   service values and the `0x0C` table boundary.
2. Correct the common ingress classification so `52:0C` is a non-callable
   XMS sentinel rather than an unrelated unknown-service value.
3. Build the package fixture with MSVC x64 `/MT /W4 /WX` and run it from a
   fresh `artifacts/build/t205-s6-xms-session-r3` directory.

```text
cmd.exe /d /c call VsDevCmd.bat -arch=x64 -host_arch=x64
cl.exe /TC /std:c11 /W4 /WX /MT ... bx_ntvdm_xms_package_session_v1_test.c
  bx_ntvdm_xms_package_session_v1.c bx_ntvdm_xms_dpmi_plane_v1.c
  bx_ntvdm_bop_ingress_v1.c bx_ntvdm_bop_provider_registry_v1.c
  bx_ntvdm_cpu_result_v2.c bx_ntvdm_cpu_state_abi.c
  bx_ntvdm_cpu_delta_abi.c bx_ntvdm_instruction_window_abi.c
  bx_ntvdm_exception_abi.c
t205-s6-xms-session.exe
```

## Observations

The original dispatcher provides exactly `00..0B`; `xmssvc.h` gives the next
value, `0C`, the name `XMS_LASTSVC`.  The previous ingress passed `0xFF` as
the XMS sentinel, so `52:0C` was classified as an unknown service.  The common
classifier now uses `12u` for both the callable-table bound and the
non-callable XMS sentinel, without giving Bochs machine code an XMS meaning.

The fresh strict fixture built and ran successfully, printing
`T205-S6-XMS-PACKAGE-PASS`.  It verifies:

| Services | Current disposition proven |
| --- | --- |
| `00` | Source-derived A20 set/query resume. |
| `01` | Copied move record through the selector-blind mantle copy request. |
| `02..05`, `0A`, `0B` | Source-derived block/geometry/statistics resume and bounded failure results. |
| `06..08` | C3-gated UMB group produces typed `STOP`, not an unhandled fault. |
| `09` | C5-gated INT15 notification produces typed `STOP`, not an unhandled fault. |
| `0C` | Common ingress classifies it as `NOT_CALLABLE_SENTINEL`. |

## Interpretation

The current no-device profile has one complete XMS routing and disposition
surface.  This is not a claim that UMB or INT15 functionality exists: S3 and
S5 rejected their machine prerequisites, and their typed stops are intentional
non-success outcomes.  No native trace was run, because it cannot turn absent
machine ownership into a valid provider.

## Follow-up

Close T205 at the current-profile package boundary.  A future named machine
profile may reopen the C3 or C5 groups only with separately admitted native
UMA or interrupt lifecycle evidence.  It must not use this fixture as proof
of native guest execution.
