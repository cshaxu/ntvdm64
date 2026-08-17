# T225 S24 P1 — Machine/BIOS Current-Route Ledger

## Question

Which complete historical machine/BIOS universe is relevant to the current
T225 source-built startup, which entries are actually reachable, and which
current executable route owns each reachable entry without promoting trace
order into implementation authority?

## Inputs And Procedure

- Pinned historical `refs/opennt/base/mvdm/softpc.new/base/bios/bios.c`,
  especially the conditional `BIOS[]` initializer; its existing provenance
  hash is recorded in [T96 S7](t96-s7-softpc-bios-owner-matrix-001.md).
- Original `host/src/nt_bop.c`, `bios/spckbd.asm`, `dos/v86/doskrnl/bios/spcemm.asm`,
  `base/comms/printer_.c`, and `host/src/nt_mouse.c`.
- The complete historical table classification in
  [T131](../../history/m0-t131-closure-20260812.md), [T193 S1](t193-s1-reached-machine-selector-audit-001.md),
  and [T199 S49](t199-s49-machine-bios-package-revalidation-001.md).
- Current formal sources: `bx_ntvdm_vdm_generic_ud_bridge_v1.c`,
  `bx_ntvdm_composition_runtime_v1.c`,
  `bx_ntvdm_boot_namespace_composition_v1.c`,
  `bx_ntvdm_native_bop_composition_v1.c`, and the formal T225 Ninja manifest.
- The immutable S18 reached-owner set and the S23 native observation.

The previous owner-matrix script could not be rerun because its historic
immutable export
`artifacts/analysis/opennt-bop-inventory-005-20260811-001/opennt-bop-inventory.json`
is absent from this checkout.  This is recorded as an unavailable verification
input, not silently regenerated or substituted.  The retained T131 result is
therefore the authoritative complete-table count for this P; the current-route
part of this ledger is established by direct source review.

## Historical Universe

The `BIOS[]` initializer contains 41 named selector definitions and 349
conditional table forms.  T131's pinned inventory classifies all forms as:

| Historical disposition | Forms | Meaning now |
| --- | ---: | --- |
| `illegal_bop` historical-unavailable | 227 | No product route or fabricated success. |
| OpenNT top-level collision | 25 | A three-byte selector cannot choose a SoftPC table branch over the named OpenNT owner. |
| Bochs-native candidate | 97 | Evidence for a Bochs machine domain only; not enablement or a bx-vdm pseudo-service. |

This complete universe prevents the nine reachable identities below from being
misrepresented as a new, incomplete BIOS table.

## Reachable-Identity Ledger

The union of S18's bounded source-built startup set and S23's later native
observation contains these machine/top-level identities.  `Direct/Readonly`
is deliberately shown as profile-neutral unless a host mutation capability is
actually involved.

| Identity | Original owner and contract | Current bound route | Disposition | Direct / Readonly |
| --- | --- | --- | --- | --- |
| `02` | `BIOS[02] -> unexpected_int`; reads PIC state/masks and writes BDA interrupt cause. | generic bridge -> boot composition declines -> native composition does not recognize `02` -> CPU exception path. The old `machine_composition_v2` can model it but is not the formal native route and uses callback/TLS mechanics. | **replace** in the next coherent startup-machine package with a typed source-derived `unexpected_int` transcript; do not add an isolated adapter shortcut. | identical machine behavior |
| `06` | `BIOS[06] -> illegal_op_int -> unexpected_int`; advances the saved real-mode frame before the same PIC/BDA sequence. | generic bridge -> boot composition declines -> native composition -> `opennt-host` transcript -> typed mantle RAM/PIC mechanics. | **retain** as S23's source-built typed route; consolidate its shared `unexpected_int` portion with `02` only under the next package. | identical machine behavior |
| `12` | `BIOS[12] -> memory_size`. | boot composition -> common ingress/machine facade -> declared conventional-memory profile result (`AX=027Fh`). | **retain**, but it remains declared profile data rather than a live Bochs/CMOS query. | identical machine profile |
| `15:AH=88` | The reached NTIO caller uses INT 15 extended-memory query; it is not authority to enable the table's general `cassette_io` owner. | boot composition -> common ingress/machine facade -> declared extended-memory profile result (`AX=0C00h`). Other `15` forms decline. | **retain** the bounded contract; general INT 15 remains unimplemented. | identical machine profile |
| `17` | `BIOS[17] -> printer_io`; without `PRINTER`, the original body performs no I/O and returns. | boot composition exact unavailable route -> empty `RIP+3` resume. | **retain** as explicit no-printer continuation; no host LPT/spooler route exists. | identical no-device behavior |
| `5E` | Named `MS_bop_E`: `AL=0` config-complete notification calls `UMBNotify` and `demDasdInit`; it collides with alternative BIOS rows. | boot composition -> top-level package facade -> config-complete route. | **retain in OpenNT top-level package**, not in the machine package. | profile-neutral notification |
| `5F` | Named `MS_bop_F`: `kb_setup_vectors`, monitor table publication and historical ICA restart. `spckbd.asm` supplies the BOP table and `AX=BEEF` contract. | boot composition -> common ingress/machine facade -> existing checked stream-state transaction or bounded CF/RIP continuation. | **migrate/normalize** with the startup-machine package; retain only source-proven table publication and no keyboard/PIC/firmware reimplementation in bx-vdm. | identical fixed display-mode contract |
| `66` | `spcemm.asm:InitSpcEmm` clears `BX`, issues BOP `66`, and treats `BX=0` as no EMS. | boot composition exact unavailable route -> `BX=0`, `RIP+3`. | **retain** as explicit no-EMS terminal; no LIM/EMS device is admitted. | identical no-device behavior |
| `C8` | Under historical NTVDM, `BIOS[C8] -> v_host_mouse_install1 -> host_mouse_install1 -> mouse_install1`. | boot composition checked read of the declared mouse table -> IVT INT 33 write -> `RIP+3`. | **migrate/normalize** with the startup-machine package as a headless source-derived vector mapping; it does not enable a host mouse, GUI, or Bochs device. | identical headless behavior |

`02` was observed after S23's live `06` transaction, but that observation is
only corroboration of reachability.  It is not the reason for selecting the
package: this ledger selects the package because the nine identities already
share startup-machine ownership and currently split across two compositions.

## Current Composition Boundary

The formal runtime binds both composition sessions.  The generic bridge first
asks `boot_namespace_composition`, then asks `native_bop_composition` only on
decline.  Therefore being compiled into `bx-vdm.lib` is not evidence of live
selection; the rows above name the actual first accepting route.  The legacy
`machine_composition_v2` remains a compiled historical island but has no call
from this formal bridge.  Its TLS/callback mechanics are not a valid successor
to S23's copied typed mantle boundary.

No row moves selector meaning into `bx-core` or `bx-mantle`.  The mantle
continues to own only checked mechanical execution; the adapter composes
fixed-width transactions and OpenNT-derived meaning; `opennt-host` owns the
source-derived INT 06 transcript.

## Four-Rung Recovery Result

| Rung | Result |
| --- | --- |
| Original source | The named BIOS/NTVDM owners and their ordering/failure rules are present and cited above. Direct reuse is excluded where the source depends on CCPU/SAS/host product composition. |
| Smallest source-preserving seam | S23's `opennt-host` transcript and the existing checked startup transactions are the first usable seams. They must be normalized rather than replaced by free-standing selector handlers. |
| External-code intrusion | No new Bochs intrusion is needed or admitted by this audit. Existing selector-blind RAM/PIC mechanics remain sufficient evidence only for the `02`/`06` family. |
| Newly authored behavior | None admitted. Declared memory profile values and explicit no-device continuations remain previously documented bounded product contracts, not authority for new BIOS functionality. |

## Successor Recommendation

The next implementation S must be a single **startup-machine composition
normalization** package, covering the whole reachable set above.  Its first
implementation objective is to share the original `unexpected_int` mechanics
between `02` and retained `06`, then migrate/verify the existing `5F`, `66`,
`C8`, `17`, `12`, and `15:88` routes under one audited selection surface while
leaving `5E` with its OpenNT top-level owner.  It must run one family regression
before a single native trace.  It may not enable an extra device, install a
SoftPC `BIOS[]` table, convert a collision into a selector guess, or use trace
order as its implementation backlog.
