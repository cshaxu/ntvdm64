# Core To Historical Transition Bridge Gap

Status: source/interface comparison, 2026-08-07. This is evidence for a
future neutral mantle bridge. It authorizes no change to `ntvdm64` and no
historical runtime adapter.

## Question

Can the current `core_machine_undefined_instruction_transition` callback call
the original OpenNT selector-50 path directly?

```text
guest bytes -> core transition -> BIOS[50h] -> MS_bop_0 -> DemDispatch -> DEM
```

The answer is **no, not yet**. The callback is sufficient for a generic trace
stop, but not for entry into the mutable historical CCPU/SoftPC register and
SAS contract.

## Historical Contract

The original CCPU decoder in
`softpc.new/base/ccpu386/c_main.c` consumes an instruction and calls
`bop(selector)` while its own complete CPU context remains live. The original
selector-50 owner, `softpc.new/host/src/nt_bop.c:MS_bop_0`, then:

1. reads the minor service byte at `SEGOFF(getCS(), getIP())` through
   `Sim32GetVDMPointer`;
2. calls `DemDispatch(service)`;
3. lets DEM routines read/write the live register context and guest pointers;
4. performs `setIP(getIP() + 1)` after the dispatch; and
5. runs the original idle policy.

`DemDispatch` itself documents and observes all of AX, BX, CX, DX, DI, SI,
CS, IP, DS, ES, SS, SP, BP, and FLAGS. Its `demLoadDos` handler gets the NTDOS
destination from DI and obtains a mutable guest pointer through
`GetVDMAddr(getDI(), 0)`. Other live paths use `SS:SP`, `ES:DI`, and `DS:SI`.
The historical monitor owns this whole mutable state and its direct SAS mapping
for the duration of the host call.

## Current Core Surface

The current generic core transition input contains CS, EIP, seven general
registers and EFLAGS. Its response can patch those same general registers and
EFLAGS only. It intentionally does not expose DS, ES, SS, SP, segment caches,
control state, a mutable guest-memory pointer, a reentrancy rule for memory
operations during `core_machine_run`, or arbitrary instruction-pointer control.

Additionally, `core_machine.c` fixes `consumed_bytes` to the registered pattern
length after invoking the consumer. A historical selector-50 call instead
needs a defined phase where the BOP selector has been consumed, the minor
service byte remains at the current IP during `MS_bop_0`, and the original
owner alone advances IP after `DemDispatch`. The current generic callback has
no contract for that three-stage ownership.

## Consequences

The following are prohibited:

- invoking `MS_bop_0` from the current callback with a copied register subset;
- borrowing core's private CPU/RAM objects while execution is active to satisfy
  the historical `get/set*` macros or `Sim32GetVDMPointer`;
- mapping a direct historical SAS pointer onto core RAM without a provider and
  lifetime contract; or
- adding a replacement selector/SVC decoder merely to make NTIO proceed.

Any of these would either lose SS/SP/segment/memory side effects or bypass the
core's checked memory and execution ownership model.

## Required Future Bridge Shape

If a second proven consumer justifies it, the bridge must be a separate,
versioned mantle-level transaction rather than an expanded BOP API. Its minimum
contract is:

1. core stops at a documented instruction boundary and captures a complete,
   mode-tagged CPU state;
2. the bridge exposes checked guest-memory read/write/translation operations
   with no raw RAM borrow and a defined lifetime;
3. an adapter receives a full private context snapshot, invokes the historical
   consumer, and returns a complete validated state delta plus checked memory
   effects;
4. core validates and commits the delta atomically, including the precise
   IP-consumption phase; and
5. real/protected/V86 mode changes remain core CPU operations, never adapter
   permissions.

This is not a request to expose a Microsoft-named API. Generic instruction
transitions remain valuable for trace and neutral consumers. The bridge only
becomes admissible after evidence proves that an original historical consumer
can be compiled and invoked through a narrow C ABI without sharing CRT/C++
objects or raw core internals.

## Immediate Research Rule

The current core probe may continue to prove reset, entry-plan, execution, and
generic stop behavior. It may not claim historical `BIOS[50h]`, `MS_bop_0`,
DEM, or NTIO compatibility. The original selector-50 fixtures remain the
behavior oracle for that chain until a complete bridge transaction exists.
