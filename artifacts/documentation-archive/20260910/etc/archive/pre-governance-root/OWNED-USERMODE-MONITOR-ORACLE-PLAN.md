# Owned User-Mode Monitor Oracle Plan

Status: bounded contract-fixture plan while historical CCPU recovery remains
the primary route, 2026-08-08. This plan is not a claim that the OpenNT CCPU
profile has been recovered or authorization to build a replacement VM.

## Purpose

The historical CCPU route supplies source-derived behavior oracles, notably
the bounded selector-50 traces. The absent `InitNtCpuInfo` x86 generated ABI
is an active recovery problem, not permission to replace CCPU wholesale. This
fixture plan therefore records contained machine/transition contracts while
historical CCPU source recovery continues. It cannot become a BYOB execution
route unless a recovery disposition demonstrates materially unrecoverable CCPU
surface and `ntvdm64/core` is then selected and governed as the backend.

The first target is deliberately not NTIO boot. It is a controlled conformance
fixture for the historical trace:

```text
guest bytes: C4 C4 50 14 C4 C4 FE
expected chain: selector 50 -> service 14 -> IP semantics -> bounded stop
```

The historical fixture remains the oracle; the owned monitor may not reuse its
self-authored BOP/DEM code.

## Component Boundary

```text
owned session shell
  -> BYOB/profile validator                 (no guest mutation)
  -> contract fixture / selected core adapter
  -> recovered CCPU or governed ntvdm64/core
  -> generic transition dispatcher          (registered patterns)
  -> profile adapter                         (historical behavior oracle)
  -> future contained host capabilities
```

The session shell continues to hand PE32/PE32+ images to `CreateProcessW`.
It never starts an NTVDM system component, injects another process, changes
the registry, writes a system directory, or discovers an ambient DOS runtime.

## M0: Machine-State Contract Fixture

M0 may provide an owned, zeroed, finite physical-memory fixture and a
real-mode visible state record. Reads, writes, and test fetches are checked
against the same mapping route; there is no separate unchecked fetch aperture.
It is not a CPU execution backend.

Its first fixture state is limited to real-mode visible registers, FLAGS,
finite test RAM, and a bounded execution-budget record. A20 behavior, ROM and
provider mappings, protected mode, V86, CRx, descriptor caches, IDT/GDT,
hardware interrupts, PIC/PIT, and devices are unavailable and must cause a
typed stop if reached. A selected recovered CCPU or governed `ntvdm64/core`
backend must expose only:

- reset to an owned real-mode state;
- bounded run/step result with fault/stop reason;
- checked register snapshot/patch;
- checked physical-memory read/write; and
- instruction bytes at the physical fetch address for transition matching.

No backend callback may perform guest service work, host I/O, or a CPU-mode
transition.

## M1: Generic Transition Contract

An execution transition is a profile-registered byte pattern, not a named BOP
or DOS service API. Its registration records a maximum-length byte sequence,
physical fetch-address matching, priority/conflict rule, and a handler ID.
Patterns are registered only while the machine is configured; the registry is
frozen before reset. Prefix-overlap and duplicate-pattern resolution must be
rejected at registration rather than resolved dynamically.

On a match, the monitor performs this transaction:

1. snapshot CPU state and the matched physical bytes;
2. invoke the profile adapter with immutable snapshot data;
3. validate a constrained disposition and patch;
4. commit the complete patch atomically, or leave CPU and memory unchanged.

Allowed dispositions are only `unhandled`, `handled_resume`, `stop`, and
`fault`. A handler may patch an explicitly enumerated real-mode register set,
FLAGS mask, and the consumed IP length. It may not patch CRx, segment hidden
caches, CPU mode, mapping topology, A20 state, IRQ state, or arbitrary memory.
Every CPU mode transition remains an instruction/backend semantic, never a
transition-handler action.

An unregistered instruction follows the backend's normal instruction path; a
fault is not converted to a host service response.

The initial NT4 profile can later register the historical `C4 C4` pattern, but
that fact is profile data. The monitor exports no C4-, BOP-, selector-50-, or
Microsoft-named public interface.

## M2: Oracle-Driven Adapter Growth

The first adapter has exactly one bounded service behavior, sourced from the
existing original selector-50 oracle. Its acceptance test compares:

- matched byte length and consumed IP;
- selector/service dispatch identity;
- documented `demQueryDate` register/FLAGS result; and
- `C4 C4 FE` bounded-stop behavior.

It must not provide arbitrary selector values, filesystem access, device
responses, DOS return policy, or guest startup success. Each later service is
admitted only after an original-chain trace identifies its historical caller,
inputs, outputs, side effects, and containment requirements.

## Exit Criteria Before NTIO

Before M0/M1/M2 evidence can be used for an NTIO prefix trace:

1. the BYOB profile validator accepts or rejects before guest allocation;
2. mapping and transition registration freeze before reset;
3. all CPU/memory patch commits are atomic under failure injection;
4. the owned selector-50 result matches the historical oracle in an automated
   comparison; and
5. unknown instruction, mode transition, device I/O, interrupt, and memory
   mapping paths stop with redacted diagnostics rather than a synthetic result.

6. historical CCPU recovery remains the documented primary route, or a
   source-recovery disposition has proved its material incompleteness and the
   governed `ntvdm64/core` adoption decision has been accepted. No local
   `ntdos64` execution backend is admitted.

Only then may the bootstrap-cut checklist be applied to a real owner-supplied
NTIO image. This route does not relax any requirement for contained filesystem,
streams, clock, display, or media capabilities.
