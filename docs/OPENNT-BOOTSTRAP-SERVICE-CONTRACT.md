# OpenNT Bootstrap Service Contract

## Evidence Scope

This is a source-derived contract for the fixed historical OpenNT VDM model
present in `upstream/opennt/base/mvdm`. It is not a claim that an arbitrary
user runtime is bootable. No Microsoft binary is read by, copied to, or
executed by the current research engine.

## Bootstrap Finding

`dos/dem/demmisc.c`, `demLoadDos`, documents that `NTIO.SYS` requests the host
to load `NTDOS.SYS`. The source reads the requested load segment from guest
`DI`, obtains the corresponding guest linear address, then reads `ntdos.sys`
in 16 KiB chunks into that address. The loader is therefore not a normal DOS
MZ loader invocation and cannot be replaced by the current COM/MZ product
loader. A future BYOB adapter must:

1. accept a manifest-verified runtime root selected by the user;
2. intercept the documented DOS BOP request only after the guest executes its
   trap instruction;
3. validate the destination against mapped ordinary guest RAM, with overflow
   and image-size checks before writing any bytes;
4. read only the selected `NTDOS.SYS` file and record role, length and digest
   in a redacted trace; and
5. leave all other files and host configuration untouched.

The OpenNT implementation builds a host path from its own DOS directory. The
research adapter must instead bind that lookup to the user-selected BYOB root;
no system-directory fallback is permitted.

## Exact First Bootstrap Pattern

OpenNT gives a byte-level starting point for the first handler. In
`inc/dossvc.h`, `SVC(func)` expands to `BOP BOP_DOS` followed by the service
byte. `BOP_DOS` is `50h`, and `SVC_DEMLOADDOS` is `11h`. In
`dos/v86/doskrnl/bios/msinit.asm`, initialization sets `DI` to
`dos_load_seg` and invokes `SVC SVC_DEMLOADDOS`. The first fixed-profile
bootstrap pattern is therefore exactly:

```text
C4 C4 50 11
```

Its input contract is real-mode `DI = destination segment`; the destination
linear address is `DI << 4`, and the file is the manifest-verified profile's
`NTDOS.SYS`. The handler must not generalize this one proof into support for
the other DOS minor services. `demdisp.c` confirms that minor `11h` dispatches
to `demLoadDos`; its surrounding dispatch table is historical evidence only.

The owned WHPX fixture now validates this handler *shape* with no external
runtime: it enters the exact four-byte pattern with `DI=0020h`, verifies that
register value at the rewritten trap, bounds-checks `DI << 4`, writes a small
self-owned payload at physical `0200h`, then resumes to an owned halt. This
proves register capture, checked destination calculation, private guest-memory
write, and handled-resume. `nvtdm --runtime DIR --plan-bootstrap manifest`
now provides the preceding read-only profile-planning step: it manifest-checks
the selected runtime and creates rewrite metadata from a private `NTIO.SYS`
copy. It does not open `NTDOS.SYS` for loading, execute NTIO, prevalidate a
load destination, or produce a bootstrap trace; those remain separate work.

## BOP Decoder Requirement

`inc/bop.h` fixes the historical byte form as `C4 C4 selector`, three bytes.
It assigns selectors including DOS `50h`, XMS `52h`, DPMI `53h`, command `54h`,
redirector `57h`, idle `5Ah`, and switch-to-real-mode `FDh`.

The decoder must be general rather than reserving `C4 C4` for one vendor:

- match a registered byte pattern at the physical instruction bytes actually
  fetched by the virtual CPU;
- consume exactly the registered instruction length on a handled event;
- expose a copied register snapshot and bounded guest-memory operations;
- permit only `unhandled`, `handled-resume`, `stop`, or `fault` outcomes;
- validate all output register patches, then commit them atomically; and
- keep mode changes inside the CPU backend's formal semantics.

An unregistered pattern must retain normal CPU invalid-opcode behavior. This
keeps the mechanism useful for an owned DOS engine without creating an
NTDOS-specific CPU instruction.

## Mode Transition Finding

`dpmi32/i386/dpmi386.c`, `switch_to_real_mode`, demonstrates that the DPMI
route restores a guest-supplied frame containing `DS`, `SP`, `SS`, `IP`, and
`CS`, then clears protected-mode enable. It also maintains monitor state bits
for real-mode and virtual-interrupt handling. Consequently, a bootstrap-capable
WHPX machine needs an explicit, validated mode-transition operation; it must
not let a BOP handler arbitrarily modify CR0, segment caches, or V86 state.

The first implementation gate is narrower: establish whether a `C4 C4
selector` trap captures its physical fetch address, selector, registers, and
next-IP rule without mutating guest state. On the checked WHPX host, the
initial real-mode capability probe instead observes normal #UD delivery ending
in a memory-access exit, not `WHvRunVpExitReasonException`. Thus no BYOB
profile may register a DOS-load handler through a direct #UD route. The backend
must first supply a different generic instruction-observation mechanism. The
next documented candidate is an execute-page mapping exit: map a page R/W but
not X, observe its physical RIP and bytes before execution, then decide whether
to dispatch a registered pattern. It is deliberately page-granular and needs a
separate resume design before it can execute arbitrary mixed code.

The owned fixture now proves the minimum handled-resume transaction: observe
the non-executable page, verify the three bytes, remap that page executable,
atomically set only `RIP + 3`, and reach an owned `HLT` with all other tested
registers unchanged. This one-shot transaction does **not** keep trapping later
BOPs on the same page; it is evidence for the disposition semantics, not yet a
complete execution strategy.

## Candidate Resume Mechanism: Private-RAM Rewrite

The checked backend also has a documented I/O-exit path: `whpx-io` proves
`WHvEmulatorTryIoEmulation` retires an owned `OUT imm8, AL` and resumes the
following guest instruction. A candidate for repeated BOP observation is
therefore a **private guest-RAM** rewrite, performed only after a profile
registers an exact pattern:

1. scan only the adapter's fresh guest-memory copy, never the user file;
2. record each physical offset and the exact original bytes in a per-session
   immutable rewrite table;
3. replace a registered three-byte BOP with an equal-length owned trap such as
   `OUT imm8, AL; NOP`, using a dedicated, non-forwarded port; and
4. validate the I/O exit against the rewrite table before dispatching the
   registered handler, then use the documented emulator only to retire the
   owned replacement instruction.

This makes every candidate location repeatably observable while normal code on
the same page remains executable. It does not modify the BYOB source file or
any system file, but it is still guest-image transformation and must be opt-in,
traceable, reversible on reset, and rejected when patterns overlap or a page is
not ordinary writable guest RAM.

The owned foundation now exists in `src/archived/legacy-adapters/transition_registry.c`.  It accepts
only 3--16 byte physical-memory patterns during registration, rejects duplicate
or prefix-overlapping registrations, freezes before application, delegates to
the atomic private-image rewrite table, and exposes only the four documented
dispositions.  `transition-registry-test` verifies that pre-freeze use and
post-freeze registration fail, capacity failure leaves the image unchanged,
and restore is exact.  `whpx-bop-observe` is its first consumer.  This remains
an owned fixture: there is no manifest-to-registry profile loader, runtime
file service dispatch, reset lifecycle, or NTDOS bootstrap execution yet.

The owned observation fixture now demonstrates the machine-level subchain with
no external input: it observes `C4 C4 50 11` on a no-execute private page,
rewrites only those four private-RAM bytes to `E6 E9 90 90`, verifies the
dedicated I/O exit, and consumes the original four-byte extent before its
owned halt. This is still not a profile rewrite table or a BOP handler.

## Consequence for Current Code

`owned-dos-engine` deliberately has none of these facilities. It remains a
COM/MZ real-mode engine with three owned INT 21h calls. The next executable
work is a generic trapped-instruction observation fixture plus checked mapped
memory access, followed by a manifest-gated BOP service adapter. DPMI/XMS,
`COMMAND.COM`, `HIMEM.SYS`, `DOSX.EXE`, redirector and Win16 are later service
tiers, not implicit consequences of loading `NTDOS.SYS`.
