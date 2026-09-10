# M0 T251 S2 — Redirector capability-profile source-recovery decision

## Selected profile

The selected first profile is **single-session public-Win32 Redirector**. It
uses the existing bx-vdm session and opaque handle manager; it does not
modify the host merely to install this product and it does not introduce any
Bochs semantic.

| Group | Services | Profile disposition |
| --- | --- | --- |
| Lifecycle and mode | `00/01`, `29/2A` | Included: one session-owned Redirector state machine. |
| Synchronous named pipes | `02..08`, `20/21` | Included: public Win32 named-pipe operations through opaque handle tokens and checked guest spans. |
| Asynchronous named pipes | `23/24` | Included: a session-owned completion queue; guest ANR/semaphore writes remain checked and no host callback crosses the boundary. |
| Mailslots | `09..0F` | Included: public Win32 mailslot capability plus per-PDB session ownership and termination. |
| LAN Manager / identity / assignment | `10..1F`, `22`, `2B..2E` | Included where a public API is present; otherwise return the historical load-failure-shaped `ERROR_INVALID_FUNCTION`, never a fabricated success. |
| NetBIOS, DLC and VDM window | `25..28`, `2F..31` | Explicitly deferred: these require a separate DPMI/network-device/interrupt package. The Redirector dispatcher returns its original unavailable outcome until that owner is admitted. |

## Four-rung recovery ledger

| Rung | Result |
| --- | --- |
| 1. Original source reuse | The 16-bit callers, structures and service numbering are retained directly from OpenNT. The actual VDMREDIR provider bodies, including `VrDispatch`, are absent, so no provider translation unit can be compiled. |
| 2. Smallest compatibility seam | Reuse the existing bx-vdm typed ingress, session lifecycle, checked guest-RAM actions and opaque handle manager. They replace CCPU/SAS pointer/register access only; they do not choose service semantics. |
| 3. External-code intrusion | Rejected. No Bochs change, host system change, private NT API, CCPU or CSR recreation is needed. |
| 4. Source-derived provider | Required. `src/bx-vdm/bop/opennt/redir/` will mirror the OpenNT service-family/header topology, and `bop/shim/redir_*` will contain only the named x86/x64/session/Win32 divergences. |

## Required implementation order

1. Define the selector-`57` typed ingress and the session provider ABI,
   including the historical `ERROR_INVALID_FUNCTION` unavailable result.
2. Replace the current DEM `Vr*` no-op helpers with the same provider's
   named-pipe registration/query interface; no duplicate pipe table.
3. Implement lifecycle, synchronous pipe and opaque-token group as one
   regression unit.
4. Add async completion and mailslot lifecycle as separate owner groups on
   the same session ABI.
5. Add the NetAPI/identity/assignment group only with its packed guest
   descriptor conversion and public-API failure tests.
6. Leave `25..28` and `2F..31` visibly unavailable until their DPMI/network
   machine owner is admitted; do not silently widen the profile.

## Preserved historical observable rules

- The original `MS_bop_7` failure path sets carry, returns
  `ERROR_INVALID_FUNCTION`, and advances beyond the service byte.  The typed
  ingress must express that same guest-visible result.
- `VrDispatch` reads a contiguous service code; no per-trace recognizer or
  scattered DEM/COMMAND special case is permitted.
- Open pipe, mailslot and async records remain per VDM/session and per DOS
  PDB where the original structures require it.  Host handles, event handles,
  host callback addresses and thread IDs stay host-only.

## S3 admission

S3 may now implement the session provider and the lifecycle/named-pipe owner
group.  It must move or delete the existing `Vr*` placeholder helpers only
when the new provider has equivalent explicit unavailable behavior and focused
negative/positive regressions.
