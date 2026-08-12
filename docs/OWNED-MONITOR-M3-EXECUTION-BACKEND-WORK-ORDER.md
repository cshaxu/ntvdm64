# Owned Monitor M3 Execution-Backend Work Order

Status: superseded historical record, 2026-08-09. Its external-core adoption
proposal is no longer part of the current research route. The governing route
is `SOFTPC-RECONSTRUCTION-ROADMAP.md`; this file is retained only to preserve
the earlier read-only comparison evidence.

## Decision

The preferred execution candidate is the independently built `ntvdm64`
software CPU/machine core, consumed only through the existing default-disabled
archive experiment. This aligns with the intended eventual boundary:

```text
core software machine -> mantle/profile adapter -> research ntvdm.exe
```

The M0/M1/M2 modules are deliberately small contract fixtures. They are not a
second permanent CPU core and do not authorize a competing machine stack. The
findings they prove must be mapped into the future core/mantle boundary before
a research runner depends on them.

WHPX remains an optional user-mode hardware-assisted probe. Its real-mode HLT,
I/O, and interrupt fixtures are useful evidence, but it is not the preferred
route because this project requires a software-emulated x64 execution path
that can remain coherent with the shared core direction. Historical CCPU/V86
also remains oracle-only because its required x86 generated monitor ABI is not
recoverable from the fixed source inputs.

## M3 Scope

1. Re-run the existing `ntdos64-core-machine-probe` against an independently
   built, read-only `ntvdm64` checkout and record exact archive/toolchain
   identity.
2. Compare its current generic transition contract to M0/M1: physical fetch
   basis, registration/freeze timing, disposition set, patch limits, atomic
   rejection, and real-mode entry constraints.
3. Write a no-source-change bridge assessment. It must distinguish a temporary
   same-toolchain archive probe from the future stable `core.dll` ABI.
4. Select one owned byte-stream fixture that reaches controlled stop through
   the core. It contains no DOS profile data, BIOS, BOP, SVC, device, or host
   capability.

## Explicit Exclusions

- Do not edit `D:\home\repos.hobby\ntvdm64`.
- Do not make its headers/archives a default `ntdos64-run` dependency.
- Do not call this a DLL ABI, a full mantle, an NTIO boot, or NTDOS support.
- Do not fall back to WHPX, a V86 route, or the frozen handwritten DOS
  prototypes if the core probe is unavailable.

## Acceptance

M3 completes only when the read-only integration probe either proves the
bounded software-machine path or records its first exact incompatibility. A
passing probe must still leave `ntdos64-run` linked from only
`src/runner/ntdos64_run.c`.

## Follow-Up

If the archive probe remains valid, the next task is a contract-delta report
for the `core -> mantle` migration, not a duplicate CPU implementation in
`ntdos64`. If it fails, retain the M0/M1/M2 fixtures as executable
specification and report the precise missing core interface to the `ntvdm64`
workstream.

## Verification Evidence

On 2026-08-08, `ntdos64` configured a new local build directory with the
read-only `ntvdm64` source directory and these independently built archives:

```text
ntvdm64 source revision: 99df886528699a0275e1b47fc2e16e3343083034
libcore-machine.a:          9151EA2F896FB4E1A24D8214B2C1277387607FD40E41F7FE562927ABD056FB90
libcore-machine-executor.a: 13A7DB1C3D061625EE0B436F5495E5BE12C85F384B9FE3D36F19D3B82AF5A0F7
libtype-facade.a:           EA2CD2E8DB901F96C35D4E6C060D6258F9A14CA0D468951FB039E66D052BFCF9
```

The local command configured
`NTDOS64_ENABLE_NTVDM64_CORE_EXPERIMENT=ON`, passed only those archive paths,
built `ntdos64-core-machine-probe`, and ran it. The probe registered an owned
undefined-instruction pattern, froze, reset, atomically applied an entry plan,
and reached `STOP` through actual software instruction execution. It printed
`ntdos64 core-machine probe: OK`. The emulator's diagnostic `#UD` line is the
expected transition precondition, not a probe failure. A subsequent normal
`ntdos64-run` build still has exactly one object:
`CMakeFiles/ntdos64-run.dir/src/runner/ntdos64_run.c.obj`.

## Contract Delta

| Concern | Read-only core evidence | M3 disposition |
| --- | --- | --- |
| Software execution, reset, run budget, stopped entry plan | `core_machine_create`, freeze, reset, `core_machine_apply_entry_plan`, and `core_machine_run` exercised by the probe | reusable execution substrate |
| Checked mapped memory and frozen topology | `core_machine_memory_*` and entry-plan preflight | reusable, richer than M0 fixture |
| Pattern length | core maximum is 15 bytes | M1 corrected from 16 to 15 and retested |
| Pattern overlap and post-freeze registration | core rejects duplicate/prefix overlap and closes at freeze | semantically aligned |
| Physical fetch identity | transition input exposes only `CS:EIP`, though CPU reads a physical address internally | core gap: add a copied physical fetch address to transition input |
| Short-tail fetch | core dispatch tries to read all 15 bytes before matching any entry | core gap: fetch only the candidate length or provide an available-byte count, so a valid short pattern at the end of RAM is observable |
| Patch authority | response replaces all exposed general registers; it has no per-register/FLAGS write masks | core gap: define explicit allowed patch masks and validate before one state commit |
| Segment/stack state | transition response exposes no `SP`, `DS`, or `ES` patch fields | defer until a trace proves need; do not invent a wrapper-side register store |
| FLAGS | core permits only arithmetic status bits, whereas M1's general contract also models `TF`, `IF`, `DF` | core/mantle policy decision required before profile needs them |
| Fault response | core needs a nonzero fault detail; invalid response outcomes become unhandled | M2 fault needs a typed core-fault mapping, not a synthetic stop |
| Capacity | core has eight transition slots; M1 fixture has 32 | defer until profile corpus measures demand |
| ABI | current proof is same-toolchain static archives | no DLL or stable external ABI claim is admitted |

The first three core gaps are blocking for a faithful direct M1/M2-to-core
bridge. They belong in the `ntvdm64` core workstream, not in an `ntdos64`
compatibility shim. No file in the external checkout was modified.
