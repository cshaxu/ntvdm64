# M0 T199 S12 Redirector Source, ABI, And Failure Matrix

## Question

What complete `BOP_REDIR` (`57h`) package is defined by the retained OpenNT
sources, and what disposition can the non-invasive CLI profile truthfully
provide before a Redirector capability exists?

## Inputs

- `src/opennt/base/mvdm/inc/rdrsvc.h`: contiguous service definitions
  `00h` through `31h`, with `MAX_REDIR_SVC` equal to `SVC_NETBIOSCHECK`.
- `src/opennt/base/mvdm/dpmi/dxnetbio.asm`: a guest caller of
  `FBOP BOP_REDIR, SVC_NETBIOSCHECK`.
- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c`: `MS_bop_7`, its
  `VDMREDIR` lazy-load path, and its unavailable result.
- Current `bx-vdm` inventory, ingress, catalog, registry, and system-plane
  sources plus their regression tests.

## Procedure

1. Read the contiguous original definitions rather than inferring membership
   from a historical trace.
2. Compare the definitions with the inventory exporter and every current
   selector/service bound.
3. Read the original top-level unavailable branch and identify its host-bound
   dependencies without attempting to link it.

## Complete Package Map

| Service range | Original responsibility | CLI disposition basis |
| --- | --- | --- |
| `00-01`, `0F` | Redirector lifecycle and termination | Requires `VDMREDIR`; explicit unavailable. |
| `02-08` | Named-pipe information, state, I/O, transaction, call and wait | Requires historical host named-pipe implementation; explicit unavailable. |
| `09-0E` | Mailslot delete/info/create/peek/read/write | Requires historical host mailslot implementation; explicit unavailable. |
| `10-1A`, `22`, `2E` | Network transaction, server, use, workstation, message, DC and service APIs | Requires NT network APIs and Redirector composition; explicit unavailable. |
| `1B-21` | Redirected identity and handle information | Requires Redirector session/host state; explicit unavailable. |
| `23-28`, `2F-31` | Async pipe, NetBIOS/DLC, VDM window and interrupt/PM NetBIOS operations | Requires async, VDD/window and NetBIOS/DLC composition; explicit unavailable. |
| `29-2D` | Return/set mode and assignment/macro compatibility | Requires Redirector state; explicit unavailable. |

The package has **50 callable identities**, `00h` through `31h`. `57:31`
(`SVC_NETBIOSCHECK`) was absent from the earlier parser expression and all
three current numeric bounds. This record supersedes only the Redirector
counts in the earlier global audit and T96 inventory-derived statements:
there are 195 named service definitions, two non-callable sentinels, and 193
callable host-family services.

## ABI And Failure Contract

The source form is a four-byte family BOP: `C4 C4 57 service`. `MS_bop_7`
reads the service byte through the old CCPU/SAS environment, lazily loads
`VDMREDIR`, and on load failure sets `CF=1` and `AX=ERROR_INVALID_FUNCTION`.
It documents that DOS execution otherwise continues without network, DLC,
NetBIOS, named-pipe, or mailslot support.

`MS_bop_7` itself is not a link candidate: it mixes guest-state primitives,
dynamic DLL loading, Win32 handles and historical dispatcher entry points.
The retained tree does not contain the referenced Redirector dispatcher body
(`vrdisp.c`); this is a source-closure limit, not permission to invent
per-service behavior.

The existing `bx_ntvdm_redir_unavailable_provider_v1` is an older,
disconnected three-byte-result artifact. It cannot prove the current common
four-byte ingress/resume contract and must not be reused blindly. The next
package implementation must provide one common `bx-vdm` provider for all 50
members, preserving the source-derived unavailable flags/result and using the
current typed result ABI.

## Observations And Interpretation

The corrected inventory exporter yields 50 Redirector entries and ends with
`SVC_NETBIOSCHECK:49`. Ingress, catalog, and system-plane classification now
accept `31h`; no Redirector capability, core/mantle behavior, or trace was
added. The appropriate package disposition is therefore one complete,
explicit-unavailable CLI provider, not a set of service-specific shims.

## Follow-up

Admit T199 S13 for that one whole-package provider, a 50-member positive and
out-of-range negative regression, and no native trace. Only after S13 is
accepted may the Redirector package receive its one native integration trace.
