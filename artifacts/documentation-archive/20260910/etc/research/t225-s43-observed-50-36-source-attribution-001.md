# T225 S43 — Observed 50:36 Source Attribution 001

## Question

What original OpenNT owner and state contract explain the S42 selector-blind
observation of selector `50h`, service `36h`, and copied pre-dispatch state;
and what does that observation not establish?

## Inputs

- S42 paired observation: `docs/etc/evidence/t225-s42-exception-loop-watchdog-result-001.md`.
  Both Direct and Readonly runs ended through the normal one-million-tick budget
  terminal and logged the same `50:36` record.
- Approved primary source tree: `src/opennt/`.
- Source-policy authority: `docs/etc/operations/policy/source-policy.md`.

## Procedure

This was a read-only source attribution. The reviewed source paths were:

1. `src/opennt/base/mvdm/inc/DOSSVC.INC:64-74`, which defines
   `SVC_DEMENTRYDOSAPP EQU 36H`.
2. `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm:1362-1390`, the
   `NTDOS.SYS` `$Exec` `Xfer_To_User` path.
3. `src/opennt/base/mvdm/dos/dem/demdisp.c:153,185-219`, the original DEM
   `36h -> demEntryDosApp` table entry and dispatch behavior.
4. `src/opennt/base/mvdm/dos/dem/demmisc.c:286-318`, the original
   `demEntryDosApp` provider and its stated entry/exit contract.

No executable, provider, BOP route, guest input, profile, or trace was changed
or invoked for this record.

## Observations

S42 records the following copied pre-dispatch state for `50:36`:

```text
BOP trap CS:EIP = 0041:5a70
EAX = 00000c41  EBX = 00000000  ECX = 000000ff  EDX = 00000c41
ESI = 00000100  EDI = 0000fffe  DS = 0c41  ES = 0c41  SS = 00a7
```

`msproc.asm` states immediately before `SVC SVC_DEMENTRYDOSAPP` that:

- `DS:SI` is the child entry point;
- `AX:DI` is the initial child stack;
- `DX` is the PDB pointer; and
- `BX` is the initial `AX` value.

After the SVC returns, that same path executes `DOCLI`, clears `InDos`, sets
`SS:SP` from `AX:DI`, creates a far-return frame from `DS:SI`, sets `ES` and
`DS` from `DX`, restores `AX` from `BX`, then `retf`s into the child.

The observed low words consequently match the original transfer contract:

| Original `$Exec` field | Observed copied field | Value |
| --- | --- | --- |
| Child entry `DS:SI` | `DS`, low `ESI` | `0c41:0100` |
| Initial stack `AX:DI` | low `EAX`, low `EDI` | `0c41:fffe` |
| PDB pointer `DX` | low `EDX` | `0c41` |
| Initial child `AX` | low `EBX` | `0000` |

`DOSSVC.INC` identifies service `36h` as `SVC_DEMENTRYDOSAPP`.
`demdisp.c` maps that slot to `demEntryDosApp`. Its provider describes the SVC
as made by `NTDOS.SYS,$exec` just before entering a DOS application; its normal
non-debug action obtains the PDB from `DX` and, except on the first call,
invokes the historical VDD user hook. Its debug-only branch can report state
or break, but is not an execution-continuity implementation.

## Interpretation and confidence

Confidence is high that the observed record is the original `$Exec`
pre-child-transfer notification, and that the copied register fields agree with
its source-defined contract. This resolves the identity and immediate purpose
of this specific observation without adding a special `50:36` implementation.

The logged `CS:EIP` is the BOP/SVC trap location (`0041:5a70`), not the child
entry point: the original contract assigns that point to `DS:SI`. The record is
copied before dispatch and before `$Exec` installs the child stack/registers
and performs `retf`. Therefore it does **not** prove that the child reached
`0c41:0100`, ran any instruction, entered COMMAND, returned to its parent, or
that the complete DEM/COMMAND packages are closed. The recorder's `count=128,
overflow=1` further prevents it from proving an exhaustive later service
sequence.

## Source-recovery disposition

This is an attribution-only S. `demEntryDosApp` is a historical DEM owner,
but its legacy VDD hook is bound to unavailable historical host composition.
S43 neither attempts to compose it nor proposes a replacement. Any later
provider work must first perform the mandatory four-rung recovery audit and
preserve the above ordering/failure contract.

## Follow-up

Close S43 after documentation governance verification. The next owner package
must be admitted globally from the T225 roadmap, not selected from a later
individual trace record. A future guest-continuity package may use this source
fact as one bounded prerequisite, but must establish its own caller, machine,
provider, and end-to-end acceptance evidence.
