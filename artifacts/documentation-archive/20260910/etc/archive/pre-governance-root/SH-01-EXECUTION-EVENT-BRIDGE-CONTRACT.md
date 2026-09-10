# SH-01 Execution Event Bridge Contract

Status: historical-reference only; excluded from the current CCPU profile,
2026-08-06.

## Scope Correction

OpenNT contains two historical execution paths into the same `BIOS[]` table:

```text
CCPU interpreter: c_cpu_simulate -> bop(n) -> BIOS[n]()
V86 monitor:      NtVdmControl -> VDMEVENTINFO -> EventVdmBop -> BIOS[n]()
```

The CCPU path is not an event producer. In
`softpc.new/base/ccpu386/c_main.c`, its `C4 C4` decoding advances CCPU's
instruction pointer and directly invokes the historical `bop(n)` macro, which
`bios.h` defines as `(*BIOS[n])()`. It must remain direct. Inserting SH-01
between that original CCPU call and `BIOS[]` would alter the historical flow.

SH-01 would only be relevant if the kernel-V86 monitor path were ported to a
modern process. That is explicitly outside the current research executable
profile. It is not a prerequisite for CCPU, not a generic CCPU trap API, and
must not acquire implementation work while the CCPU-only path is active.

## Historical Dependency Replaced

For the V86-monitor profile only,
`base/mvdm/v86/monitor/i386/monitor.c:cpu_simulate` historically calls
`NtVdmControl(VdmStartExecution, NULL)` while in real/V86 mode. The removed
NT host service runs guest instructions and writes a `VDMEVENTINFO` result into
the VDM TIB. SH-01 replaces only that event-production operation for the
research process. It is not an emulation of the old NT system call and must
not export or patch `NtVdmControl`.

## Preserved Historical Flow

For a V86-monitor BOP exit, the normal path must remain:

```text
historical monitor cpu_simulate
  -> SH-01 run slice returns one event
  -> historical monitor applies EventInfo.InstructionSize
  -> historical EventDispatch[EventInfo.Event]
  -> historical EventVdmBop
  -> historical BIOS[EventInfo.BopNumber]()
  -> historical MS_bop_0 / DemDispatch / DEM as selected by BIOS[]
```

The bridge does not call `BIOS[]`, `MS_bop_0`, `DemDispatch`, or an individual
DEM handler. It does not inspect or consume the byte after a BOP instruction.
The V86 monitor remains the sole owner of event dispatch and the historic BOP
handler remains the sole owner of service-byte consumption and any subsequent
IP advance. The CCPU profile instead preserves its direct `bop(n)` call.

## Private Result Contract

The private C interface is deliberately event-oriented. Names and layout may
change before code exists, but all implementations must retain these semantic
limits:

```c
typedef enum sh01_event_kind {
    SH01_EVENT_BOP,
    SH01_EVENT_FAULT,
    SH01_EVENT_STOP
} sh01_event_kind;

typedef struct sh01_event {
    sh01_event_kind kind;
    unsigned char instruction_size;
    unsigned char bop_number; /* meaningful only for SH01_EVENT_BOP */
    unsigned int fault_code;  /* meaningful only for SH01_EVENT_FAULT */
} sh01_event;
```

The run slice receives the V86 monitor's private execution/session context. It
may advance the CPU only by executing guest instructions through the selected
V86 execution backend. It may not mutate guest mode, segment state, general
registers, flags, memory, or device state as a side effect of classifying an
exit. Any such change must originate in the CPU execution result and be
reflected in the historical monitor context.

`instruction_size` must be the size of the instruction that produced the
event, not the BOP selector byte or an arbitrary resume delta. For a BOP,
`bop_number` comes from the guest instruction bytes as observed by the CPU
execution boundary; SH-01 must not attach semantic meaning to its value.

## Explicit Non-Responsibilities

SH-01 must not:

- register an arbitrary instruction-pattern callback;
- recognize `C4 C4`, a BOP selector, or an SVC number in a public API;
- switch real, protected, or V86 mode outside CCPU's normal instruction
  semantics;
- inject an interrupt, emulate a device port, create IVT/BDA state, or select
  a firmware/boot profile;
- call host injection, modify `ntdll`, use registry configuration, or create
  a system-wide VDM facility.

Interrupt delivery belongs to SH-02 and filesystem/console/process behavior
belongs to their separately admitted rows. A stopped or faulted run slice is
an observable result, not permission to synthesize a recovery path.

## Admission Tests

Before any V86-monitor implementation is called a runnable bridge, its
evidence must show:

1. one bounded CCPU execution slice produces exactly one event or an explicit
   no-progress/stop result;
2. on a recorded BOP event, the historical monitor applies the reported
   instruction size exactly once;
3. `EventVdmBop` invokes exactly one historical `BIOS[bop]` entry;
4. the selected historic handler, not SH-01, obtains any following service
   byte and changes IP accordingly; and
5. a fault and a stop do not result in a BIOS or DEM call.

An isolated mock-table test may demonstrate the bridge's call-count boundary,
but it is startup-trace instrumentation only. It does not establish runtime
compatibility until the actual historical monitor and BIOS table are linked.

For the initial CCPU profile, the corresponding evidence is different: a
source-linked `c_cpu_simulate` execution must arrive at the original `bop(n)`
macro and one historical `BIOS[n]` entry. It must not pass through SH-01.

## Comparison Boundary

NTVDMx64's experimental HAXM monitor provides evidence that an execution exit
can continue through `EventVdmBop` into `BIOS[]`. Its HAXM exit transport,
instruction-specific `C4 C4` handling, and `FixNTDLL` loader patch are
explicitly excluded. They do not satisfy this contract.
