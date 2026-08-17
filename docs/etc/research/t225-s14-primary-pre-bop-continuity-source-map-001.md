# T225 S14 Primary Pre-BOP Execution Continuity Source Map

## Question

What can the immutable source-built NTIO image and the current machine-stage
source prove about the zero-BOP S13 result before any BOP provider or Bochs
mechanic is changed?

## Static entry and first expected boundary

The original OpenNT startup path in
`src/opennt/overlay/base/mvdm/softpc.new/host/src/nt_msscs.c` loads NTIO at
`NTIO_LOAD_SEGMENT:NTIO_LOAD_OFFSET` and then performs only `setCS` and
`setIP`.  The current typed plan retains that exact geometry:

| Contract item | Current source | Value |
| --- | --- | --- |
| NTIO payload write | `bx_ntvdm_composition_runtime_v1_prepare_startup_plan` | physical `0x700` |
| Entry delta | same | `CS:EIP = 0070:0000` |
| Preserved pre-image state | same / `bx_ntvdm_machine_stage_v1_begin` | four bytes at `0x714`, captured before and restored after NTIO write |
| Native entry application | `bx_ntvdm_machine_stage_v1_arm_real_mode_entry` | `bx_cpu.apply_real_mode_entry(CS,EIP)` after the write |

The source-built primary `NTIO.SYS` begins at that physical address with a
near jump from offset `0000` to `0370`.  Read-only 16-bit disassembly of that
immutable image shows a `C4 C4 50 11` sequence at image offset `0675`:

```
physical 0x0b75  C4 C4 50 11
```

This is the same `50:11` / `demLoadDos` boundary independently catalogued in
the retained `r20-real-ntio-next-boundary-triage` record.  It lies on the
ordinary initialization path after the reset jump, not in NTDOS or COMMAND.
If this instruction reaches the existing generic `#UD` bridge, S12's enabled
listener necessarily records it: the bridge constructs a 15-byte copied
window for every `#UD`, and the listener records any window beginning
`C4 C4`, including declined events.

## Meaning of the observed decode diagnostics

The source-built image also contains valid lock-prefixed memory operations
very early in that path, for example at image offsets `0380` and `03e1`:

```
F0 81 26 14 03 FF FD    lock and word [0x314],0xfdff
F0 81 0e 14 03 00 02    lock or  word [0x314],0x0200
```

Bochs emits `LOCK prefix unallowed` only when the decoded operation is not
lockable or lacks a memory destination (`src/bx-core/cpu/fetchdecode64.cc`).
Those messages therefore cannot, without an address/window witness, be
attributed to the two valid NTIO instructions above.

Likewise, Bochs correctly raises `#UD` for `SLDT` outside protected mode
(`src/bx-core/cpu/protect_ctrl.cc:SLDT_Ew`).  OpenNT's historical SoftPC CCPU
has its own `SLDT` implementation in
`src/opennt/base/mvdm/softpc.new/base/ccpu386/sldt.c`, returning the CCPU LDT
selector.  But the first static NTIO `SLDT` byte sequence occurs only at image
offset `4255`, long after the expected `50:11` boundary.  The current log
alone cannot establish whether a reported real-mode `SLDT` was a genuine
later CCPU-era operation, an exception/continuation path, or misdirected
control flow.

## Consequences

S13's zero BOP result through 100,000 ticks does **not** prove a missing DEM
provider, an NTDOS load failure, or a Bochs CPU feature gap.  Combined with
the entry/write ordering above, it proves a narrower unobserved frontier:
actual control flow has not been located relative to the known first
`C4 C4 50 11` byte sequence.

| Candidate | Evidence status | Owner/disposition |
| --- | --- | --- |
| BOP `50:11` provider | Not reached by S13 | Do not implement or alter. |
| NTIO payload/entry order | Source contract and current code agree | Retain; independently observe only if a generic witness contradicts it. |
| Valid early LOCK operations | Static image evidence | Do not patch CPU from an address-free log. |
| Real-mode `SLDT` | Diagnostic is real, source intent at reached CS:EIP unknown | Observe generic `#UD` first; no CCPU reintroduction. |
| PIC/IRQ13, x87, IVT, VGA/`0xA1800` | Historical downstream records, not reached by S13 | No admission. |

## Next bounded package

S15 may add an adapter-owned, fixed-capacity, default-off *generic* `#UD`
observation that copies only the existing event's CS:EIP, execution mode and
instruction window.  It must be selector-blind, must not read guest memory or
alter the existing bridge outcome, and must not add a bx-core or bx-mantle
change.  Direct and Readonly may then be re-run under the already-proven
bounded budget to identify the first actual `#UD`; only that result can select
a later owner package.

This is an evidence map, not an implementation approval.