# T199 S48 — Top-level host-service package revalidation

## Question

Can every non-machine top-level BOP retain a source-derived CLI disposition
through common ingress, including the complete notification selector contract,
without adding WOW, UI, debugger, VDD, GUI, scheduler, keyboard or video
semantics to Bochs or the adapter?

## Owner map and scope separation

`src/opennt/base/mvdm/inc/bop.h` defines all BOP instruction forms as three
bytes.  The relevant non-machine host-service selectors and current package
dispositions are:

| Selector | Original owner evidence | Current CLI disposition |
| --- | --- | --- |
| `51` | `MS_bop_1`, historical WOW dynamic composition | typed terminal stop; WOW not installed |
| `59` | `MS_bop_9`, host warning/direct-access UI | typed terminal stop; no UI |
| `5A` | `MS_bop_A`, WaitIfIdle/WakeUpNow | `RIP+3`, no register/flag mutation; no scheduler claim |
| `5B` | `MS_bop_B`, host debugger breakpoint | typed terminal stop; no debugger host |
| `5E` | `MS_bop_E`, notification | AL=0 routes retained config-complete provider; nonzero resumes as original ignored notification |
| `FE` | host unsimulate/VDM lifecycle | typed terminal stop; no host lifecycle emulation |

`5C`, `5D`, `5F` and `FD` are deliberately outside this host-service closure:
the first, second and fourth are source-mapped machine/monitor/DPMI routes and
the third is the existing machine handoff.  They remain explicit inputs to the
next bx-core/bx-mantle package, not implementation authority for `bx-vdm`.

## Repair

Review of `nt_bop.c` showed that `MS_bop_E` documents exactly one notification
code, zero.  Its other branch only diagnoses in non-production builds and
continues.  Previously `bx-vdm` handled code zero but allowed nonzero values to
fall through the selected BOP path.  The top-level facade now emits the
source-derived `RIP+3` no-op continuation for nonzero AL.  It does not read
guest memory or assign new semantics to a notification code.

## Verification

Two fresh MSVC x64 `/MT` runs passed:

1. `artifacts/build/t199-s48-top-level-facade-r1/` compiles the focused facade
   test.  It verifies terminal, idle, AL=0 delegation and nonzero `5E` no-op
   outcomes through common ingress.
2. `artifacts/build/t199-s48-top-level-package-r1/` builds the bound
   boot-namespace composition.  It verifies the top-level host-service paths,
   plus explicit controlled-stop routing of machine-owned `5C`, `5D` and `FD`
   pending their owner package.

Both use three-byte BOP decoding; a fourth observed byte is not a service byte.
No native guest trace ran.

## Limit

This closes only the source-derived top-level host-service profile
dispositions.  It does not implement WOW, UI, debugging, host waiting,
notification side effects beyond admitted config completion, or CLI process
termination.  Machine/BIOS and monitor requirements remain separately owned by
the next package.
