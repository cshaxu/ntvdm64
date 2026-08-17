# T225 S2 P1 DEM Child/PDB Lifecycle Source, ABI, and Failure Map

## Question

What may the CLI composition recover for the original DEM child/PDB lifecycle
without taking over DOS process teardown, VDD semantics, or a future profile's
private resource lifecycle?

## Inputs

- Active packet: M0 T225 S2 in `docs/STATUS.md`.
- Original OpenNT:
  - `src/opennt/base/mvdm/dos/dem/demmisc.c:demEntryDosApp`;
  - `src/opennt/base/mvdm/dos/dem/demsrch.c:demTerminatePDB`;
  - `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msctrlc.asm:reset_environment`.
- Current adapter:
  - `bx_ntvdm_vdd_create_user_notify_service.c`;
  - `bx_ntvdm_dem_session_lifecycle_provider_v1.c`;
  - `bx_ntvdm_dem_file_session_v1.c`;
  - `bx_ntvdm_dem_namespace_partition_v1.c`;
  - `bx_ntvdm_dem_package_session_v1.c`.

## Original Contract

| BOP | Original owner | Input | Original host action | Guest-visible result |
| --- | --- | --- | --- | --- |
| `50:36` | `demmisc.c:demEntryDosApp` | DS:SI entry, AX:DI stack, DX PDB | On a non-first call, invokes `VDDCreateUserHook(PDB)`; otherwise debug-only observation | Void return |
| `50:3C` | `demsrch.c:demTerminatePDB` | BX terminating PDB | On a non-first call, invokes `VDDTerminateUserHook`; calls `HostTerminatePDB`; frees the PDB's FFIND list when present | Void return |

The guest call order is controlling. `msctrlc.asm:reset_environment` sends
`SVC_PDBTERMINATE` while the child PDB is current, then guest DOS frees MCB
ownership, closes guest JFN/SFT/FCB state, restores the parent PDB and stack,
and returns. Neither BOP chooses a parent target or implements DOS teardown.

## Current Resource Ownership and Disposition

| Profile/resource | Current owner form | `50:3C` disposition for S2 | Reason |
| --- | --- | --- | --- |
| Direct opaque file token | `DIRECT_WIN32_HANDLE`, copied PDB owner | Release all matching private Win32 handles, invalidate their tokens | The direct opener records the bounded current PDB; leaving those handles after guest teardown is a real host-private leak. |
| Readonly startup image token | `READONLY_NAMESPACE`, owner zero | No PDB release; retain startup namespace lifecycle | It is a session-wide declared startup capability, not child-owned state. Closing it at an arbitrary child PDB would alter the admitted startup contract. |
| Search state | boot namespace's per-PDB search sessions | Release matching search slots | This is the direct source-shaped `demTerminatePDB` responsibility already present. |
| Overlay file token | private Overlay file backend | Explicit deferred lifecycle hook; no Direct fallback | Overlay remains outside this Direct/Readonly closure. Existing material is retained, not broadened. |
| Virtual file token | private virtual-root backend | Explicit deferred lifecycle hook; no Direct fallback | Virtual remains default-off/future package work. |

## ABI and Failure Contract

Both services are exact real-mode `C4 C4 50 xx` ingress records. They resume
at fault RIP + 4 with no fabricated general-register or flag result.
`50:36` must not read guest memory or enact VDD behavior in the no-VDD CLI
profile; it consumes no profile-private resource. `50:3C` copies only the
low BX PDB word and must preserve the original notification's void shape.

A stale Direct token after a successful `50:3C` must be rejected by its
ordinary typed invalid-handle path. A missing search list or no matching Direct
token is a successful no-op. A malformed BOP, invalid ingress, or mechanical/
typed provider failure is not converted into a guest DOS teardown or a
fabricated success.

## Recovery Ledger

| Rung | S2 decision |
| --- | --- |
| Original source | Retain DEM's two notification contracts, especially void result and guest-owned teardown order. |
| Smallest adapter/shim | One selector-blind-internal DEM lifecycle provider receives the copied PDB and coordinates only search cleanup plus already-owned Direct token cleanup. |
| External-code intrusion | Rejected: no Bochs, VDD, device, or guest DOS change. |
| New behavior | Rejected: no adapter EXEC/PSP/MCB/JFN model, host child, or parent-return substitute. |

## Implementation Decision

Migrate the current standalone `50:36` no-VDD recognizer and the current
`50:3C` search-only provider into one DEM child/PDB lifecycle package route
inside `bx_ntvdm_dem_package_session_v1`. It may use the existing typed
whole-provider file session only when that session exists; it may never infer
an Overlay or Virtual cleanup policy from Direct.

The package must reject attempts to route either service through a generic
fallback. It does not claim that the original VDD hooks are available: their
no-VDD profile disposition is an explicit source-shaped omission.

## Required Regression Matrix

1. `50:36`: exact instruction, real mode, +4 void resume; altered service or
   non-real-mode input remains unhandled; no VDD side effect.
2. `50:3C` Direct: two PDB-owned Direct tokens are invalidated, a different
   PDB token remains live, and search state for the target PDB is released.
3. `50:3C` Readonly: the declared startup token retains its session scope and
   the result is still void.
4. A stale token returns the established invalid-handle result after the
   notification; no profile falls through to another backend.
5. The whole-provider Direct/Readonly file-chain fixture remains green under
   MSVC x64 `/MT`. Overlay/Virtual are asserted deferred, not exercised as
   completed lifecycle providers.

## P2 Implementation and Regression Checkpoint

The lifecycle provider now owns both selectors before the package facade: it
returns the no-VDD void form for 50:36, and for 50:3C it releases the existing
search slots plus only PDB-owned Direct file-session handles. The standalone
adapter runtime no longer keeps a parallel VDD-create recognizer; it invokes
the same lifecycle provider before its generic DEM unavailable provider.

A fresh MSVC x64 /MT dem-lifecycle fixture compiled, linked, and directly ran
with exit code 0. A broader dem-package fixture compiled and linked but ran
with exit code 195 at its pre-existing 50:41 media-ID assertion. The same
fixture was rebuilt and run from detached pre-change commit d798e509; it also
returned 195. Thus that failure is baseline evidence for the media package,
not a T225/S2 regression and not a reason to weaken lifecycle tests.

This checkpoint does not close S2. It still needs one focused package-session
fixture that proves the newly wired 50:3C call invalidates a Direct token while
retaining a Readonly startup token's session scope. Overlay and Virtual remain
explicitly deferred.

## P3 Closure Evidence

A fresh source-built whole-provider fixture in build/M0-T225-S2/003 compiled,
linked, and ran under MSVC x64 /MT with manifest exit codes 0, 0, and 0.
It drives ordinary DEM package-session ingress. In Direct mode it opens a
PDB-owned host-view token, invokes 50:3C with BX=1234h, receives the void
resume at RIP+4 with no register/flag delta, and then observes the established
invalid-handle result on seek. In Readonly mode it opens the declared
COMMAND.COM startup token, invokes the same 50:3C void notification, then
successfully seeks and closes that token. The test thereby proves that the
lifecycle route has no Direct fallback for Readonly session capability.

A separate fresh dem-lifecycle source closure in build/M0-T225-S2/004 linked
and directly ran with exit code 0. It covers the exact no-VDD lifecycle ABI.
The broader dem-package 50:41 failure remains the recorded pre-change
baseline and is not relied on for this closure.

S2 is closed: the recovered package is limited to the original host
notification boundary. It neither claims EXEC, PSP/MCB/JFN teardown, VDD
recovery, Overlay/Virtual lifecycle completion, nor COMMAND parent return.
