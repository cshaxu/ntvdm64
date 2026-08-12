# OpenNT CCPU standalone backend assessment

> **归档状态（2026-08-08）：** 本文曾研究把 CCPU 单独包成新的 standalone
> backend。该方向现不在执行队列中。当前工作只允许在原始 SoftPC ownership
> chain 内、由真实 caller 证实后补全缺损函数和现代 host seam；不得据此新建
> adapter、替代 host stack 或绕开 `BIOS[] -> MS_bop_0 -> DemDispatch -> DEM`。
> 冲突时以 `SOFTPC-RECONSTRUCTION-GOVERNANCE.md` 为准。

## Scope

This note assesses only whether the OpenNT `softpc.new/base/ccpu386` source
can serve as the x86 execution backend of a non-invasive, user-mode `nvtdm`
research executable.  It does not modify OpenNT, import it into `ntvdm64`, or
claim that an output produced from this source may be distributed.

The target remains a process launched from a writable user directory.  It may
not require AppInit injection, registry changes, system-directory deployment,
a service, a kernel driver, BCD changes, or a private CSRSS interface.

## Evidence

OpenNT declares CCPU as a library:

- `base/mvdm/softpc.new/base/ccpu386/sources` sets `TARGETTYPE=LIBRARY` and
  lists the interpreter's instruction, address, paging, protected-mode,
  task-switch, exception, and register source units.
- `base/mvdm/softpc.new/base/ccpu386/c_main.c` exports `c_cpu_init`,
  `c_cpu_interrupt`, and `c_cpu_simulate`.
- `base/mvdm/softpc.new/base/inc/cpu.h` exposes those calls through the
  `cpu_init`, `cpu_interrupt`, and `cpu_simulate` aliases.

Therefore the CPU interpreter is not intrinsically tied to the old NTVDM
process loader.  It is, however, tied to the SoftPC service vocabulary.

## Required adapter services

The following are mandatory implementation obligations of a new standalone
adapter, not optional links to the historic OpenNT host sources.

1. **Memory and A20:** CCPU's `ccpusas4.c` owns the `Sas` vector and calls
   `host_sas_init` / `host_sas_term`.  The adapter must provide contiguous
   guest storage, physical and linear checked access, ROM/device dispatch,
   and deterministic 20-bit wrapping.  It must not substitute host pointers
   for guest addresses at the public monitor boundary.
2. **CPU lifecycle:** invoke only `c_cpu_init`, `c_cpu_simulate`, and
   `c_cpu_interrupt` through a narrow backend facade.  Reset, stop, and
   diagnostic state belong to that facade, rather than to a legacy UI loop.
3. **Port I/O and hardware interrupts:** `in.c`/`out.c` use CCPU I/O vector
   hooks; `c_main.c` calls `ica_intack` when accepting hardware interrupts.
   The adapter must bind a neutral port dispatcher and PIC implementation.
   It must preserve normal CPU interrupt delivery; no callback may directly
   mutate real/protected/V86 mode.
4. **Time and yielding:** CCPU invokes `host_timer_event` while simulating.
   The adapter must make a monotonic deadline/tick policy explicit, including
   when it injects IRQ0 and when the host run loop yields.
5. **BOP monitor:** BOP is a guest byte sequence consumed by the monitor,
   not a Windows API.  The adapter must detect and dispatch a bounded
   selector table, validate any register/memory patch, atomically commit it,
   and resume, stop, or fault.  Its only dispositions are `unhandled`,
   `handled-resume`, `stop`, and `fault`; it has no arbitrary mode-switch
   disposition.
6. **Diagnostics and allocation:** `host_malloc`, `host_free`, logging,
   error and debugger hooks must map to local process facilities.  They must
   neither import legacy configuration from the registry nor write global
   system state.

## Deliberately excluded legacy host code

`softpc.new/host/src` contains historic NT console, configuration, registry,
and monitor integrations.  It is evidence for behavior only.  The standalone
backend must not link `nt_cpu.c`, `nt_bop.c`, `nt_timer.c`, AppInit code, or
any HAXM/driver integration as its host implementation.

## Viability and risk

CCPU is a plausible execution engine because it contains a complete 386
interpreter and its public execution entry points are source-library shaped.
It is not yet a runnable standalone component: the historical `sources` file
assumes the old SoftPC include graph and helper symbols.

### Compile-probe result (2026-08-06)

`toolchain-probe/ccpu-backend-probe-v1` and `v2` are isolated copies.  The
OpenNT checkout is unchanged.  Their 129/130 result exposed a mismatch between
the copied `ccpu386/sasCdef.c` and the selected generated header.  That was a
probe assembly error, not a missing-source conclusion.

`toolchain-probe/ccpu-backend-probe-v3` corrects the pair without editing or
combining generated artifacts: it takes both `sasCdef.c` and `sas4gen.h` from
the same OpenNT `base/mvdm/softpc.new/base/cvidc` generation family, while
retaining the CCPU `sources` unit list.  With 32-bit MSVC, all **130/130** C
units compile.  The resulting non-executable staging archive is
`ccpu386-full.lib` (710,230 bytes, SHA-256
`A228AFB1B2E821011EB68D2B122D647C3037FE20A60074E4D5152FD20D3F364C`).

The archive still reports duplicate NPX helper definitions
(`initialise_npx` and `npx_reset`) from `fpu.obj` and `ntstubs.obj`.  Selecting
the real NPX implementation or the no-NPX stub remains an explicit CCPU
profile decision; a linker force option is not an acceptable solution.

This establishes compile-time closure of the historical CCPU unit set using a
same-source generated pair.  It does not establish a distributable component,
a runnable CPU backend, or an NTDOS bootstrap.  The next gate is the explicit
adapter contract and its remaining link-time services.

### Link-inventory result (2026-08-06)

`ccpu386-full.lib` was forced into a trivial 32-bit harness without execution.
This intentionally fails, but gives a finite inventory of the services a
standalone adapter must supply:

| Service family | Symbols observed | Adapter responsibility |
| --- | --- | --- |
| PIC / interrupt acknowledgement | `ica_intack`, `ica_hw_interrupt` | Maintain pending IRQ state, acknowledge through the PC interrupt controller policy, and request normal CCPU interrupt delivery. |
| Port I/O | `Ios_in_adapter_table`, `Ios_out_adapter_table`, `Ios_inb_function`, `Ios_inw_function`, `Ios_outb_function`, `Ios_outw_function` | Bounded 8/16-bit port routing with explicit unsupported-port failure. No direct host-device pass-through. |
| BOP and software interrupt hook | `BIOS[256]`, `EDL_fast_bop`, `host_swint_hook` | Route normal historical BOP selectors through the initialized `BIOS` table; treat EDL only as its distinct historical fast path. Validate selector and guest state, then return one finite monitor disposition. A handler cannot alter CPU mode directly. |
| Event / clock | `dispatch_q_event`, `host_timer_event` | Own scheduler deadline and IRQ injection policy. |
| SAS memory / ROM | `host_sas_init`, `host_sas_term`, `rom_init`, `read_pointers`, `LIM_b_write`, `LIM_w_write` | Allocate private guest memory and expose the generated SAS vector coherently, including RAM, ROM, device mapping, A20, and unsupported LIM behavior. |
| Exception / diagnostics | `host_exint_hook`, `host_error`, `trace_file`, `sub_io_verbose` | Local structured error/trace sink only; no historic console, registry, or system-global mechanism. |
| Cooperative control | `host_simulate` | Resume only through the run loop after a checked request. |

Unlike the v1/v2 probe, this link inventory has no unresolved `cSasPtrs`;
the matching generated pair supplies that vector.  The forced link still
reports duplicate `initialise_npx` and `npx_reset`
definitions from `fpu.obj` and `ntstubs.obj`.  The final selection between the
NPX implementation and no-NPX stub is an explicit CCPU profile decision; it
must not be suppressed with a linker force option.

This inventory did **not** expose registry, AppInit, kernel-driver,
system-directory, or private-console symbols.  That is evidence that the CCPU
interpreter can be placed behind a local adapter; it is not evidence that the
adapter or an NTDOS bootstrap is implemented.

### Lifecycle-probe result (2026-08-06)

`toolchain-probe/ccpu-backend-probe-v3` now contains an isolated, self-authored
adapter and a tiny `ccpu-init-main.c`.  The adapter is linked only to close the
process-local service names needed by the CCPU objects.  The probe calls
`c_cpu_init()`, then `c_cpu_terminate()`, and exits.  It does **not** call
`sas_init()`, map firmware, load guest bytes, invoke `c_cpu_simulate()`, or
execute an instruction.

The archive used by this probe is an explicit real-NPX profile:
`ccpu386-fpu-profile.lib` excludes the historic `ntstubs.obj` wrapper, whose
no-NPX helpers collide with `fpu.obj`.  It is not a linker-force workaround.
The staged artifacts are:

| Artifact | SHA-256 | Meaning |
| --- | --- | --- |
| `ccpu386-fpu-profile.lib` | `56803402FE61E6919BD38E647182C2D0CFA157A6E28033773FA45A7362405155` | CCPU source-unit archive with the selected NPX implementation and no historic wrapper object. |
| `ccpu-init-probe.exe` | `FA4D590191EB07A728A944881958972C86A31EA3BCC19B5D46896C07967926E9` | 32-bit process-local init/terminate probe; it exits with code 0. |

This is stronger than compile closure: the minimum CCPU lifecycle can enter and
leave a modern user-mode process without the historical NTVDM loader.  It is
still deliberately insufficient for guest execution.  In particular, the
probe's inert I/O, PIC, BOP, ROM, LIM, and read-pointer placeholders must never
be reused as a machine adapter.  A runnable backend requires their type-correct
contracts, private guest memory initialization, a bounded run loop, and an
instruction-level smoke test before it can load NTIO.

`ccpu-sas-probe.exe` extends that boundary one step, still without execution:
after `c_cpu_init()` it calls `sas_init(0x100000)`, then `sas_term()` and
`c_cpu_terminate()`.  The staged 32-bit process allocates, routes, and tears
down one MiB of private CCPU SAS memory with exit code 0 (SHA-256
`B2B67B122E8FE64BA5990FEBAB7982480D8FEA0773EEF00FEB4D534DFF4DAFF1`).  This
proves the CPU-plus-private-RAM lifecycle, but not a physical read/write
contract: no guest byte or SAS access operation is exercised, and the adapter's
read-pointer placeholder is intentionally not a valid execution implementation.

### Private-RAM and bounded-execution smoke results (2026-08-06)

Three subsequent staged probes advance the evidence without treating any inert
adapter callback as a device implementation:

| Probe | SHA-256 | Verified behavior |
| --- | --- | --- |
| `ccpu-sas-rw-probe.exe` | `3C94FE8EF952B7354A5D1AD724EFAE47F37D6304E5A2B0A23008A090642E2F3E` | Initializes one MiB SAS RAM, writes physical byte `0xA5` and word `0xBEEF` at separate ordinary-RAM addresses, reads both through `phy_r8` / `phy_r16`, then tears down. |
| `ccpu-unsimulate-probe.exe` | `DAC9F2848B7C0A4557F0B5072B06E6D6D18386E854F16BC85ECF14052A580F99` | Places the historical CCPU `C4 C4 FE` local unsimulate encoding at physical address 0, sets CS:IP to `0000:0000`, invokes `c_cpu_simulate()`, and returns through CCPU's formal NT-thread `setjmp`/`longjmp` path. |
| `ccpu-nop-unsimulate-probe.exe` | `DC56B969EB758C0B54E8D56D8966C373FB9767578E32F2261C117BA4B48DA445` | Executes ordinary `NOP` before the same bounded local exit and verifies returned IP is `0004`; this proves normal decode/advance plus the bounded return path. |

All three exit with code 0.  `C4 C4 FE` is historical CCPU control data used
only to bound the probe.  It is neither an external transition API nor an
NTDOS-specific contract.  The result proves a small private-RAM instruction
path, not a bootable machine: it still lacks complete device contracts, PIC,
timer, ROM/BIOS, a checked BOP service dispatcher, and a safe instruction
budget for non-test guest code.

### I/O and BOP callback smoke results (2026-08-06)

The staged self-authored adapter now initializes the actual CCPU routing shape:
a 64 KiB guest-port map whose entries select one of 256 callback slots.  All
slots start with deterministic process-local defaults (`IN` returns all ones;
`OUT` is ignored), so an unregistered port cannot dereference a null function
pointer or access a host device.  Three bounded guest probes then establish the
directional service boundaries:

| Probe | SHA-256 | Verified behavior |
| --- | --- | --- |
| `ccpu-out8-probe.exe` | `2BC3E274D48F2289C6D1A4D8CC6E697EC787EFA2585F992968DD87DE7B0848C2` | Executes `MOV AL,5A; MOV DX,1234; OUT DX,AL`; the registered local callback receives exactly port `0x1234` and value `0x5A`. |
| `ccpu-in8-out8-probe.exe` | `B2A8DCBEC1A3F59EDAF6DB8C04C09820FF3655E559E99F6308AD0108DE67735F` | A registered `IN AL,DX` callback supplies `0x3C`; following `OUT DX,AL` reports the same `0x3C`, proving a host callback value reaches the guest register path. |
| `ccpu-bop-dispatch-probe.exe` | `18ACCAB0293EA2ED7E8D59BBD7A936CDD04C6E63F8426C8D3821F9FBDDD2573C` | Earlier staged probe; its result is superseded below by direct control-flow inspection and a table-backed NTIO selector-`0x50` stop. |

The I/O probes exit with code 0.  The earlier `C4 C4 42` result must not be
read as proof that normal selector dispatch uses `EDL_fast_bop`: direct CCPU
control-flow inspection shows that ordinary `C4 C4 xx` selector forms call the
historical `bop(...)` macro, which dispatches through `BIOS[xx]`.  The staged
adapter now initializes every `BIOS` slot to a bounded unhandled-stop routine
and installs a dedicated `BIOS[0x50]` stop routine.  With a private copy of
the staged NTIO image at `0070:0000`, that route records selector `0x50`, the
following `SVC_DEMLOADDOS` minor byte `0x11`, and requested load segment
`DI=08AEh`; this is an archived frozen-instrumentation observation from its
own staged image, not the current fixed profile's load-segment ABI. The
current artifact/layout correction is recorded in
`NTIO-IMAGE-LAYOUT-PROVENANCE-CORRECTION.md`. The dedicated probe SHA-256 is
`F09199CFEDD45EC378B94C52034385A814489DBEEA26430E0A6B17F530C5D89C`.
This proves table routing and the narrow load-request state only, not a DEM
loader or boot.

The later `ccpu-ntio-demload-probe-v15.exe` extends this boundary in a still
bounded way.  It source-verifies and serves `50h/11h` (private full-image
NTDOS copy), BIOS BOP `12h` (profile-owned 640 KiB conventional memory),
`50h/3Bh` (non-debug), `50h/0Fh` (two profile-owned empty logical slots), and
`50h/1Bh` (checked DTA/PDB metadata capture), `50h/32h` (hard-error metadata
capture), and `54h/05h` (COMMAND state metadata capture).  It reaches
`50h/46h`, emits no DPB for its explicit no-mounted-media profile, then runs
for 65,536 instructions to a bounded stop at `0000:0086`.  Its SHA-256 is
`F647D7C847F0C6BB15630714DDAB5BC6BDD72319D8E72BC061DF44E3E01DFDA9`.
The low-memory result demands a source-derived BIOS/IVT/BDA plus interrupt and
timer policy; it must not be misreported as an all-zero-IVT fault.  This shows
guest execution has entered NTDOS initialization; it remains far short of a
standalone machine or an executable DOS session.

The subsequent `ccpu-ntio-demload-probe-v16.exe` applies the explicitly
documented, keyboard/video-independent initial state from OpenNT's early
`v86/scaffold/i386/fakeinit.c`: default IVT vectors through `1Dh`, the `INT
11h`/`12h`/`19h` overrides, BDA equipment and 640 KiB values, the reset vector,
and the `F000:FF53` IRET.  Its own pre-entry checks verify these bytes before
the guest runs.  It completes the same NTDOS load and service sequence, then
reaches the scaffold's default `C4 C4 5F CF` stub at `F000:0100`; CCPU consumes
that BOP and the adapter stops at `F000:0103`.  This is direct evidence that
the source-derived IVT participates in the runtime path.  It is not evidence
that selector `5Fh` is the required service, nor that the historical
keyboard/video implementations, an immutable ROM mapping, PIC/IRQ, timer, or
any PC/AT device are functioning.  SHA-256:
`40D64A21566757AA508E77713FD151BEDBD92231733C98EF4EBB39A07CDFD996`.

These are CPU-to-adapter plumbing proofs only:
the callbacks implement neither a PIC nor a timer, BIOS, disk, filesystem, or
NTDOS service.  In particular, BOP selector `0x42` is arbitrary test data;
the future monitor must register a bounded profile-owned selector table, check
all proposed guest-state or memory effects, and atomically commit only allowed
patches.  CCPU itself must remain the sole authority for CPU mode transitions.

### Execution-budget result and IRQ finding (2026-08-06)

`ccpu-budget-probe.exe` (SHA-256
`E3C68B8367D7C489717ACB4A26304355E189DCD80D8E8EC1D4188BA217A412FF`)
places ordinary `NOP` bytes in private RAM, sets CCPU's
`c_cpu_q_ev_set_count(8)`, and exits through a self-authored
`dispatch_q_event()` only when that counter expires.  It exits with code 0.
This provides a formal, in-process bounded-run mechanism for future probes;
it is preferable to a host timeout or process termination.  The adapter must
enable it explicitly per run and report budget exhaustion distinctly from a
guest-requested stop.

The corresponding hardware-IRQ probe is deliberately **not** a passing result.
It verifies that a pending `c_cpu_interrupt` requires a finite execution
budget, but neither pre-run nor an `OUT`-callback invocation produced an
`ica_intack` call under this staged CCPU build.  Source and object evidence
show a real compatibility hazard: `base/inc/cpu.h`, generated
`ccpu386/cpuint_c.h`, and compiled `c_main.obj` do not present one unambiguous
interrupt-type numbering contract.  The standalone adapter therefore cannot
yet claim PIC/IRQ support.  It must first select and validate one generated
CPU interrupt vocabulary, then demonstrate a complete trace of: device raises
IRQ -> CCPU accepts pending event with IF enabled -> PIC acknowledge -> IVT
fetch -> handler execution -> IRET or checked monitor return.

## Historical BOP boundary

CCPU's historical BOP recognition is intentionally narrower than a generic
instruction-trap facility.  In `ccpu386/c_main.c`, opcode `0xC4` is first
decoded as `LES`; only when its next byte is `0xC4` through `0xC7` does CCPU
recognize a BOP form.  The low two bits determine how many extra immediate
bytes are consumed.  It advances guest EIP before calling `bop(...)` or
`EDL_fast_bop(...)`.  The special immediate low byte `0xFE` calls
`c_cpu_unsimulate()`.

For ordinary selector forms whose low immediate byte is not `0xFE`, the
`bop(...)` macro dispatches to `BIOS[selector]`; `EDL_fast_bop(...)` is a
separate historical path and is not the normal `C4 C4 xx` service endpoint.
Consequently a standalone adapter must initialize all 256 `BIOS` entries to a
defined bounded failure disposition before it installs profile-owned handlers.
The selector `0x50` trace above verifies this exact route for the staged NTIO
image, but does not implement the requested service.

Consequences for the standalone adapter:

1. A registered historical BOP service receives a state in which its complete
   encoding has already been consumed.  It may only return a validated
   `handled-resume`, `stop`, or `fault` result; it cannot roll guest EIP back
   or directly alter real/protected/V86 mode.
2. `C4 C4` is a historical wire encoding, not the public adapter identity and
   not a reason to reserve one Microsoft-specific API.  Selector registration
   is profile data owned by the runtime adapter.
3. Any future generic transition facility must be implemented before CCPU
   decode (for example, over a checked private-memory rewrite/trap layer) and
   must have independently defined pattern length, conflict, atomicity, and
   CPU-state-patch rules.  It cannot be claimed merely because CCPU recognizes
   this one BOP sequence.

Passing that gate proves only source closure.  Runtime validation requires a
minimal monitor profile, a DOS launch smoke test, and a separate PE32/PE64
classifier that bypasses the guest entirely and calls the normal host process
launcher.
