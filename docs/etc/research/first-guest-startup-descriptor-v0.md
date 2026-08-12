# First Guest-Startup Descriptor v0

## Decision

The first contained profile cannot start by treating the proven DEM `50:11`
transaction as its entry. That transaction is reached only after NTIO has
already executed guest initialization. The descriptor below separates the
source-locked startup facts from fields that must remain unavailable until
their provenance is established. It is a prerequisite for a future Bochs
engine shim and generic adapter startup application; it is not executable
profile data.

## Source-Locked Host-to-Guest Handoff

The historical x86 host path in
`base/mvdm/softpc.new/host/src/nt_msscs.c` does the following after native
reset:

```text
read four bytes of virtual-interrupt state at physical 0x714
load NTIO image at physical 0x700 (0070:0000)
restore those four bytes at physical 0x714
set CS:IP = 0070:0000
start guest CPU
```

For the fixed research artifact, the NTIO payload is exactly 33,792 bytes
(`0x8400`) with SHA-256
`CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`.
The payload itself must be selected and identity-verified by the same BYOB
session that owns NTDOS and COMMAND; no ambient system path is permitted.

The generic adapter session currently owns those bytes but does **not** yet
have a startup-preload operation. Its current pending transaction is valid
only for the later, guest-reached `50:11` NTDOS request. Reusing that operation
for initial NTIO placement would falsely claim an observed BOP transition.

## Required Descriptor Fields

| Field | Current value/status | Owner | Admission condition |
| --- | --- | --- | --- |
| Profile identity and NTIO digest | fixed research artifact only | runner + adapter session | BYOB profile locks the exact input. |
| NTIO preload | physical `0x700`, size from admitted payload | generic adapter startup transaction | complete ordinary-RAM preflight before any write. |
| Preserved virtual-interrupt state | `0x714..0x717 = 00 00 00 c0` for pinned r18 CPU5 stock-ROM/floppy profile | Bochs machine observation/profile data | re-observe if the machine profile changes; do not default it to zero. |
| Entry address | real-mode `CS:IP = 0070:0000` | generic adapter entry record | source-locked. |
| Remaining visible registers/FLAGS | preserved native post-reset baseline | Bochs machine state | historical handoff changes only CS:IP; M13 does not authorize guessed values or adapter writes. |
| Incoming IVT 10h/15h/19h/1Bh | raw `0x40..0x6f` bytes observed for pinned r18 CPU5 stock-ROM/floppy profile; compatibility unadmitted | Bochs firmware profile | externally decode against the source ledger, then admit or reject before NTIO writes anything. |
| BDA byte `0040:0096` | observed `0x10` for the pinned r18 CPU5 stock-ROM/floppy profile | Bochs firmware/profile | NTIO reads bit 4 before first `50:11`; re-observe if the machine profile changes. |
| BDA conventional-memory word `0040:0013` | r20 raw bytes `7f 02` (`0x027f`, little-endian KiB) for the pinned CPU5 stock-ROM/floppy profile | Bochs firmware/profile | provenance is recorded for later BOP `12h`; no service result-state mutation is admitted yet. |
| Optional plugin policy | `plugin_ctrl` disables unmapped/biosdev/speaker/extfpuirq/parallel/serial | adapter composition data | already proven only for r12 synthetic fixture. |
| NTDOS load | later `DI:0000` transaction after guest `C4 C4 50 11` | adapter host-service plane | separately runtime-proven, never an initial preload. |
| COMMAND and target | no guest handoff yet | guest startup descriptor + service plane | reject until PSP/environment/command contract is source-derived. |

## First Guest Prefix and Reached Boundaries

The source-built image starts at `0070:0000` with `E9 6D 03`, reaching the
NTIO `init` prefix at offset `0x370`. That prefix operates on the virtual
interrupt word at `0x714`, saves IVT `10h`, `15h`, `19h`, and `1Bh`, installs
guest-owned INT 15h/19h entries, establishes `SS:SP = 0000:0700`, and reads
BDA `0040:0096`. It then reaches the source-identified byte sequence
`C4 C4 50 11`, where `DI` names the NTDOS load segment.

For the fixed artifact, the linker-derived `dos_load_seg` is `08B0h`, so the
first NTDOS byte is requested at `0x8B00`. This is a checked artifact-layout
fact, not an adapter constant: the adapter must use live `DI` at the reached
event.

After NTDOS has initialized, NTIO's `sysinit` first reaches `C4 C4 12` for
the historical BIOS memory-size owner. It is a distinct selector from DEM
`50:11`; it does not use `DemDispatch`. The current adapter deliberately
leaves it unhandled. No synthetic AX result, BDA value, or replacement BIOS
handler is authorized by this descriptor.

## Current Execution Classification

| Segment | Evidence | Status |
| --- | --- | --- |
| Bochs reset / CPU / ordinary RAM / generic #UD transport | r12 runtime fixture | runnable mechanics only |
| BYOB resource identity and runtime installation | CLI policy test plus r12 fixture | runnable mechanics only |
| NTIO initial preload and CS:IP entry delta | source-locked shape; execution-plan implementation must preserve all other native reset CPU state | implementation and native revalidation pending |
| Bochs firmware IVT/BDA/`0x714` pre-entry bytes | r18 stock-ROM snapshot export reaches `0000:7C00` and records all declared raw bytes; admitted only for the source-bounded first NTIO prefix | first-prefix admitted only |
| `50:11` NTDOS transaction | source contract plus r12 fixture | runtime-proven only when synthetic ROM reaches it |
| BIOS `12h` memory-size call | source-static owner evidence plus r20 BDA `0040:0013` snapshot | contract closure pending; no response-state mutation admitted |
| NTDOS `sysinit` / COMMAND target launch | caller chain known in part; full host/service closure missing | not implemented |

## Consequent Implementation Order

1. Add no guest semantics to Bochs. Derive one generic adapter startup
   transaction/entry-record ABI only after it can carry all descriptor fields
   above without Bochs object pointers.
2. Create a stopped Bochs machine observation fixture for the selected native
   configuration. It must record only the field values listed above and may
   not execute NTIO or mutate guest state. **Completed by r18; retain its raw
   record as evidence.**
3. Decode and admit or reject the observed Bochs firmware profile as the source-compatible
   provider of required IVT/BDA inputs. **The r18 values are admitted only for
   the first original NTIO prefix; no handler/device compatibility follows.**
   If a later reached input is incompatible, stop rather than reconstructing
   an adapter BIOS.
4. Only then implement a single all-or-nothing NTIO preload plus the
   source-locked CS:IP entry delta through the generic startup ABI; preserve
   the native reset baseline and independently prove reset -> preload
   -> entry-boundary stop.
5. Advance the original NTIO bytes to `50:11`, then use the already proven
   common host-service plane. Address `12h` only after its full source-derived
   caller/result/failure contract and machine-value provenance are admitted.
6. Define target/COMMAND transport only after NTDOS startup has reached its
   own guest contract; the runner/shim must reject it beforehand.

This order prevents two prohibited shortcuts: using a synthetic reset ROM as
an NTIO launcher, or turning the adapter into a BIOS/BOP/DOS implementation.

## Evidence Sources

- `docs/NTIO-IMAGE-LAYOUT-PROVENANCE-CORRECTION.md`
- `docs/NTIO-ENTRY-REGISTER-BASELINE-LEDGER.md`
- `docs/NTIO-FIRST-LIVE-IVT-BDA-LEDGER.md`
- `docs/SVC-DEMLOADDOS-HISTORICAL-CONTRACT.md`
- `docs/S5-NTIO-ENTRY-INSTRUCTION-LEDGER.md`
- `docs/research/ccpu-bios-closure/S5-SYSINIT-ENTRY-STATIC-TRACE.md`
- `docs/etc/research/adapter-r12-runtime-fixture.md`
- `docs/etc/research/adapter-r18-startup-snapshot-evidence-export.md`
- `docs/etc/research/adapter-r18-first-prefix-input-admission.md`
