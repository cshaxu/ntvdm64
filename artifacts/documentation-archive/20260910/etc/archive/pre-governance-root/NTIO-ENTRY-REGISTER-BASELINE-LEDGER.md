# NTIO Entry Register Baseline Ledger

Status: M13 source classification, 2026-08-08.

## Scope

This ledger covers the first NT4, non-WOW, cold-start handoff into NTIO at
`0070:0000`. It distinguishes a source-derived CCPU reset candidate from a
fully witnessed pre-instruction state. It does not use the current `ntvdm64`
reset state as historical proof.

## Direct Call Order

The fixed source orders relevant operations as follows:

```text
config()
  -> c_cpu_init()
       -> c_cpu_reset()
  -> InitialiseDosEmulation()
       -> reset()
       -> load NTIO at 0070:0000
       -> restore virtual-interrupt state at 0714
       -> setCS(0070); setIP(0000)
  -> host_start_cpu()
```

`base/support/main.c` calls `cpu_init` before `InitialiseDosEmulation`.
With the CCPU build selection, `cpu_init` is `c_cpu_init`. That function calls
`c_cpu_reset`; `nt_msscs.c` then contains the only direct register setters in
the NTIO handoff block, `setCS` and `setIP`.

## CCPU Reset Candidate

`c_cpu_reset` supplies this candidate state before the NTIO host sequence:

| Field group | Candidate value | Source classification |
| --- | --- | --- |
| CS:EIP | `f000:fff0` | direct CCPU reset |
| DS, ES, SS | `0000` | direct CCPU reset |
| ESP, EBP, ESI, EDI, EAX, EBX, ECX | `00000000` | direct CCPU reset |
| EDX | `00000303` for the NT4 CCPU non-SPC486 build | direct CCPU reset |
| arithmetic/control flags | cleared by the explicit reset macro sequence | direct CCPU reset |
| bit 1 of EFLAGS | architecturally reserved/visible behavior, not an explicit CCPU source literal | core must retain its generic architectural rule |

The NTIO host handoff then changes the candidate's `CS:EIP` to `0070:0000`.
The desired eventual generic entry-plan candidate would consequently be:

```text
CS=0070  IP=0000  DS=ES=SS=0000  SP=0000
EAX=EBX=ECX=EBP=ESI=EDI=00000000  EDX=00000303
EFLAGS=00000000 before generic architectural bit-1 normalization
```

This is a candidate, not yet an admitted final profile state.

## Why It Is Not Yet Fully Locked

`InitialiseDosEmulation` calls `reset()` between `c_cpu_reset()` and the final
`setCS/setIP`. On the first call, `reset.c` defines `soft_reset = 0`, so its
direct register-restoration and `JMP_DWORD_*` branches are excluded; it sets
`soft_reset = 1` only at its end. That is strong evidence that the obvious
warm-reset register mutations do not occur on this path.

However, the cold reset still invokes device POST and host-reset functions.
This ledger has not executed a pre-first-instruction trace through their
complete selected configuration and cannot prove that no indirect callback
changes a visible general register, segment, stack pointer, or FLAGS. It also
does not yet bind the exact preprocessor configuration of the historical
CCPU build to the rebuilt guest profile.

Therefore the field classifications are:

| Entry-plan field | Current status |
| --- | --- |
| CS, IP | source-locked (`0070:0000`) |
| DS, ES, SS, SP | reset-derived candidate, trace required |
| EAX, EBX, ECX, EDX, ESI, EDI, EBP | reset-derived candidate, trace required |
| EFLAGS | reset-derived candidate plus generic bit-1 rule, trace required |

No field is allowed to inherit the `ntvdm64` cold-reset implementation merely
because it currently resembles the historical candidate.

## Required Bounded Trace

The next proof must construct the selected fixed profile until immediately
before its first guest instruction and capture only:

- all fields of `core_machine_entry_plan_state`;
- real/protected/V86 mode indicator;
- physical fetch address;
- the five M9 memory fields and four virtual-interrupt bytes at `0x714`; and
- source/profile identity plus a redacted execution-cut record identifier.

It must not execute NTIO, dispatch a BOP/SVC, emulate a device response, or
retain guest image bytes. Two independent cold starts must yield equal state
for the fixed profile. A mismatch upgrades the corresponding field to an
explicit per-profile input rather than a reset assumption.

## Completion

M13 completes the required classification: it has source-locked CS:IP and a
fully enumerated candidate for every remaining entry-plan field, while making
the unresolved proof requirement explicit. M12 remains prohibited from
applying a real core entry plan until this bounded trace locks or revises the
candidate.
