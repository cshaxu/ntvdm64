# NTIO First-Instruction Dependency Ledger

Status: M15 static artifact/source correlation, 2026-08-08.

## Fixed Artifact

The analyzed source-built artifact is:

```text
artifacts/toolchain-runs/ntio-tools16-opennt-v1/base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS
length: 33792 bytes
SHA-256: CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937
map: same directory / ntio.map
```

The map places `BData_start` at offset `0000`, `init` at `0370`, and the
link-time entry at `0000`. The first bytes are `E9 6D 03`, a near jump from
`0000` to `0370`; this agrees with `msbio1.asm`'s `jmp init` and the map.
Thus the first live instruction from host entry `0070:0000` is the jump, and
the substantive init prefix begins at physical `0x0A70`.

## Ordered Prefix

The following evidence combines the raw bytes at offsets `0370..0480`,
`ntio.map`, and `bios/msinit.asm`. It is static only: no byte was executed.

| Cut | Artifact/source behavior | Required boundary | Status |
| --- | --- | --- | --- |
| `0000 -> 0370` | near jump to `init` | ordinary real-mode instruction execution | current generic core capability |
| `0370` | `FCLI` expansion saves flags and consults the 4-byte virtual-interrupt state at physical `0x714` | real-mode flags semantics plus ordinary checked low RAM; M5 owns the initial bytes | profile input already identified |
| `0390..03AA` | zeroes DS, copies four vectors named by `RomVectors` into NTIO data (`Old10`, `Old15`, `Old19`, `Old1B`) | normal 16-bit CPU, IVT readable ordinary RAM, string-instruction behavior | M8/M9 field set; core CPU conformance needed |
| `03AD..03C2` | overwrites IVT 15h and 19h with NTIO targets | ordinary checked low-RAM writes | guest-owned mutation, no mantle service needed |
| `03C4..03C8` | sets `SS=0000`, `SP=0700` | formal real-mode segment/stack semantics | core CPU requirement, not a host callback |
| `03C9..03EF` | `FSTI` expansion updates/observes virtual-interrupt state at `0x714` | same four-byte state plus correct flags/CLI/STI behavior | M5 state required; exact virtual-interrupt semantics still profile/CPU evidence |
| `03F0..0474` | clears selected IVT/BDA locations, derives BIOS data segment, computes NTDOS load segment | ordinary low-RAM writes and 16-bit arithmetic | generic CPU + guest-owned mutation |
| `0445..0455` | reads BDA byte physical `0x496`, tests bit 4, sets NTIO keyboard-function data if set | M9 profile byte plus ordinary RAM | profile input already identified |
| `0458..0474` | computes `DI = dos_load_seg` | 16-bit arithmetic | generic CPU |
| `0478` | exact bytes `C4 C4 50 11`, source `SVC SVC_DEMLOADDOS` | generic instruction transition plus a profile-local request descriptor | first non-CPU boundary |

No port I/O instruction occurs in this first entry prefix. The commented
COM/LPT initialization calls in `msinit.asm` are not assembled. No BIOS
interrupt is invoked before the `SVC_DEMLOADDOS` boundary.

## Important Correction: Virtual Interrupt State Is Live

The first `FCLI` is not a plain `CLI` in this build. Its emitted expansion
uses the low-memory word at `0040:0314`, physical `0x714`, which is the
historical fixed four-byte NTVDM virtual-interrupt state. The later `FSTI`
expansion reads and updates that same state. Therefore the M5 preserved bytes
are not merely a loader compatibility artifact: they are immediately consumed
by guest code before the first SVC.

This does not justify a Microsoft-named core API. The neutral requirements
are correct flags/CLI/STI semantics, checked ordinary RAM, and a profile-owned
initial four-byte state declaration. The meaning and initial source of those
bytes remain mantle/profile evidence.

## First Transition Boundary

At artifact offset `0478`, the source-built bytes are:

```text
C4 C4 50 11
```

The source macro site is `SVC SVC_DEMLOADDOS`, with `DI` carrying the computed
load segment. This is not the earlier bounded date-query oracle (`... 50 14`)
and must not be handled by it. It is the first actual NTIO-to-historical-host
request boundary in the fixed bootstrap prefix.

Generic core must only recognize a configured instruction pattern and make a
validated constrained transition available. Mantle/profile code may decode the
profile-local request only after the C1-C3 transition deltas are present. The
resulting request contract then delegates to historical `MS_bop_0` /
`DemDispatch` evidence; it may not become a new handwritten DOS service.

## Architecture Consequences

Already supported or correctly owned:

- checked low-memory reads/writes and ordinary RAM admission;
- real-mode arithmetic, strings, stack and segment behavior;
- M5 virtual-interrupt-state preload;
- M8/M9 IVT/BDA initial fields;
- M6/M7 NTDOS image planning once the request boundary is legitimately
  reached.

Still required before this prefix can execute in a governed backend:

1. M13's complete entry state must be trace-locked or otherwise explicitly
   selected.
2. Core C1-C3 must supply physical fetch input, short candidate matching, and
   validated partial transition patching.
3. A profile-local `SVC_DEMLOADDOS` request descriptor must be derived from
   the original historical dispatcher, including its result/return contract.
4. The virtual-interrupt word semantics must be tested against a bounded
   source-derived first-prefix fixture; it must not be inferred from the
   loader copy alone.

## Completion

M15 is complete. The first guest prefix has a fixed artifact identity, direct
source correlation, and an ordered boundary ledger. The next task is not to
write an SVC handler; it is to recover the historical `SVC_DEMLOADDOS`
dispatcher contract that the exact `C4 C4 50 11` request reaches.
