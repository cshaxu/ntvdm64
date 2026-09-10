# M75 NT4 Executable Prefix Prerequisites

Status: source-fact inventory; its former backend conclusion is superseded by
`SOFTPC-RECONSTRUCTION-GOVERNANCE.md`, 2026-08-08.

> Governance correction: the source-derived ordering remains valid. Statements
> below that require a "governed software backend" are historical planning
> language, not the active route. The active executable dependency is the
> repaired OpenNT SoftPC/CCPU software-monitor path.

## Purpose

This is the final pre-execution inventory for the fixed NT4 EN-US prefix. It
joins M61 and M68-M74 without creating a runner or treating the historical
x86 V86 monitor as an available backend.

## Historical Ordering That Must Be Preserved Semantically

`InitialiseDosEmulation` establishes this relevant order:

1. read, adjust, and save the four-byte virtual-interrupt state at `0070:0014`;
2. initialize historical I/O, reset the machine, and initialize the single
   command-shell state;
3. read `ntio.sys` to `0070:0000`;
4. restore the virtual-interrupt word overwritten by that image copy; and
5. enter at `CS:IP = 0070:0000`.

NTIO subsequently requests NTDOS through `SVC_DEMLOADDOS`; the original DEM
uses `DI:0000` as the NTDOS destination. The original host directly streams
the file into guest memory, but a future wrapper must prevalidate and publish
the complete immutable image atomically.

The ordering is an oracle-derived semantic requirement. The historical kernel
V86 monitor is not callable on the x64 research runner, but the OpenNT
SoftPC/CCPU software-monitor path is the implementation dependency. Its
reconstruction must provide the corresponding initialized topology/lifecycle
before the entry plan is applied.

## Preflight Table

| Item | Current evidence | Status before execution |
| --- | --- | --- |
| BYOB identities for NTIO, NTDOS, COMMAND | M51/M52 and profile contract | available locally; selection remains wrapper policy |
| NTIO origin and virtual-interrupt reserve | M61 and `vint.h` | source-established: `0070:0000`, reserve `0070:0014..0017` |
| NTIO image publication | M61, M69 | blocked on checked ordinary-RAM all-or-none transaction |
| CPU entry | M61 and `nt_msscs.c` | source-established `CS:IP`; all other boot-register values remain profile evidence work |
| Initial mapping/BDA/IVT/resident bytes | M71-M74 | specific BDA and vector facts exist; complete byte plan is not yet established |
| Conventional/extended memory response | M72 | profile may provide conventional memory; zero extended memory is source-compatible for the initial query |
| A20/HMA behavior | M73 | generic A20 required; XMS/HMA policy trace-gated |
| NTDOS image publication | M61 | blocked on same atomic transaction capability |
| First marker dispatch | M60, M69 | blocked on configured decode observation |
| DEM/COMMAND profile structures | M62-M68 | source ABI recorded; wrapper capability adapters remain unimplemented |
| Input/display/clock | M74 | entry layout required; live services trace-gated |

## Hard Stop Conditions

The first future prefix trace must stop, without fabricating any guest result,
when it encounters any of the following:

- a missing M70 governed backend capability;
- an unmapped or non-ordinary-RAM image span;
- an unproved boot register, IVT/BDA byte, or required resident-code mapping;
- a profile marker with no frozen, governed dispatch registration;
- a requested XMS/HMA, live input/display/time, or media operation not present
  in the declared profile capability set; or
- an uncontained host file, directory, stream, process, or device request.

## Admission Result

Static source research now establishes the order and ownership of the first
prefix well enough to state the real execution gate: the only backend blockers
are still M70's configured decode observation and paused ordinary-RAM atomic
transaction. The remaining entries above are wrapper-profile data and
trace-evidence obligations, not justification for new PC/AT mechanisms in
core.

After backend admission, the next implementation artifact is a bounded
prefix fixture that validates all table inputs, applies a real-mode entry plan,
records its first unmet original/profile dependency, and exits. It is not a
COMMAND runner and may not synthesize BOP, SVC, or device success.
