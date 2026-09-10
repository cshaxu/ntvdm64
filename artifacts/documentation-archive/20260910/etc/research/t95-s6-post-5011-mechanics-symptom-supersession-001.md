# T95 S6 post-`50:11` mechanics-symptom supersession 001

## Question

Do the retained x87 compatibility IRQ13, real-mode control-loss and `0xA1800`
prefetch symptoms prove that the current minimal profile needs an FPU reset
repair, PIC/IRQ13/IVT implementation, or VGA mapping before it can continue
past the first NTDOS load transaction?

## Inputs and procedure

This is a read-only comparison of successive retained source-built traces and
their source correlations:

- `t95-s6-post-5011-continuation-semantics-audit-001.md`, including the r10/
  r11 `50:11` continuation and first unmatched `BOP 12h`;
- `t95-s6-x87-last-operation-address-correlation-001.md` and
  `t95-s6-optin-x87-ntio-prefix-observation-001.md`, which describe the
  earlier diagnostic-only x87 compatibility branch;
- `t95-s6-r29-cli-root-set-trace-001.md`, and the retained r30 follow-up;
- OpenNT `msinit.asm:247-252` and `sysinit1.asm:821-831`, which establish
  `50:11 -> far jump -> goinit -> BOP 12h`; and
- the source-defined identities in `dossvc.h` and `cmdsvc.h`.

No build, execution, device option, guest payload, Bochs source or adapter
source was changed.

## Control-flow facts

1. The initial source-built image calls `50:11 SVC_DEMLOADDOS` and, on return,
   takes the encoded far jump into NTDOS `sysinit`; this transition and its
   exact `+4` resume are source/image-correlated.
2. `goinit` immediately requires `BOP 12h` for conventional-memory size.  In
   r10/r11 that marker was not served; r11 recorded it at `010d:0516` before
   the later x87/IRQ13/`0xA1800` observations.  The older mechanics symptoms
   were therefore downstream of an established unserved transition.
3. The later narrow route has source-shaped results for `12` and `15/AH=88`,
   then records subsequent guest BOP progress through `50:0D`, `50:0F`,
   `50:1B`, `50:32`, `50:3B`, `50:46`, `54:04`, and finally `54:0C` at
   `8DC8:0732` (r29/r30 evidence).  It does not stop at the prior unmatched
   `12h` location.
4. `54:0C` is a COMMAND configuration-path request whose next action is an
   ordinary DOS open.  Its reached position is source evidence that execution
   has passed the older first-prefix control-loss interval; it is not evidence
   that a DOS command or filesystem has run.

## Classification

| Candidate interpretation | Classification | Reason |
| --- | --- | --- |
| Current route requires an adapter-owned IRQ13/PIC/IVT service. | Rejected. | The later source-built flow advances past the earlier diagnostic interval without one. |
| Current route requires a new Bochs FPU reset repair. | Not established. | The old record already rejected reset-state causality; later control flow removes it as the current gate. |
| Current route requires VGA mapping at `0xA1800`. | Rejected as a current prerequisite. | The access was downstream of the unserved early control boundary and later flow reaches `54:0C` without admitting VGA. |
| The old x87 record is worthless. | Rejected. | It remains valid bounded evidence for that precursor route, but does not prescribe a repair for the newer route. |
| The current next missing capability is configuration/read-only file closure. | Established. | The later real guest stop is `54:0C`, whose source consumer immediately needs a reachable materialized CONFIG pathname. |

## Ownership consequence

If a later trace independently reaches a real x87 compatibility exception,
Bochs remains the sole owner of FPU state, CR0 branching, PIC delivery and any
mechanical diagnostic.  The adapter must not implement IRQ13 or DOS recovery.
But the retained precursor trace does not authorize adding those mechanics now.

The currently justified boundary remains the S7 read-only placement/file
projection capability described in the placement and read-handle records.
Bochs gains no device, BIOS, FPU or video feature from this conclusion.

## Follow-up

Retain the old x87 observations as diagnostic evidence.  On a future repeat,
compare it against this newer control-flow sequence before changing Bochs.
Until then, do not reopen FPU/PIC/VGA work; advance only by admitting the
complete configuration/file capability and then obtaining a new controlled
trace.
