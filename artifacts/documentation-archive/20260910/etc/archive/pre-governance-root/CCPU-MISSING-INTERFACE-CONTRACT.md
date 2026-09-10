# CCPU Missing Interface Contracts

Status: behavior-classification evidence, 2026-08-07. The
`c_effective_addr` algorithm has a local contract fixture; it is not yet
admitted to the complete historical link diagnostic. The other two interfaces
remain semantically unresolved.

The fixture was rebuilt from the fixed source island and re-executed on
2026-08-07 with exit code `0`. Its coverage remains limited to controlled
real/V86, GDT, LDT, and invalid-selector translation cases; it does not prove
the presence of a historical selector provider in the live bootstrap profile.

## Scope

The current CCPU profile has three interfaces without an admitted original
source provider:

```text
IU32  c_effective_addr(IU16 segment, IU32 offset)
IU8 * c_sas_touch(IU32 address, IU32 length)
IU32  c_VirtualiseInstruction(IU32 eipInRom, IUH size,
                              IU32 linearAddrOrPort, IU32 dataIn)
```

They must not be unified as a generic instruction-hook API. They have
different evidence strength and different safe dispositions.

## `c_effective_addr`: Candidate Completion Contract

This is the only one with a source-derived semantic candidate and completed
contract fixture.

The historical non-CCPU `sim32_effective_addr` implementation establishes the
same public shape:

1. split `segment:offset`;
2. in real mode, return `(segment << 4) + offset`;
3. in protected mode, obtain the selector's descriptor-table address, read the
   descriptor, and return `descriptor.base + offset`;
4. return `0xffffffff` for a selector outside the selected descriptor table.

The CCPU tree independently provides the necessary state and primitives:
`getPE`, `getVM`, `selector_outside_table`, `read_descriptor`, descriptor
types, and cached segment-base accessors. The existing BIOS, ROM, video, EMS,
mouse, and diagnostic callers pass the returned value to SAS accessors; they
therefore treat it as a guest linear address, not a host pointer.

The prospective CCPU contract is consequently:

```text
real mode or V86:         (segment << 4) + offset
protected non-V86 mode:   selected descriptor base + offset
invalid selector table:   0xffffffff, with no host-memory access
```

The default-disabled `ntdos64-opennt-ccpu-effective-addr-fixture` now tests
this completion against a controlled monitor-ABI state, without executing
BIOS, BOP, DEM, DOS, or any guest instruction. It proves real-mode, V86,
protected-GDT, protected-LDT, invalid-selector, descriptor-limit separation,
and 32-bit linear-addition wrap results. The fixture owns only synthetic
descriptor state, so it cannot silently admit unrelated CCPU SAS, LIM, ROM,
or host-simulation dependencies. The independent Bochs source crosscheck is
recorded in
`research/softpc-source-matrix/NT4-CCPU-EFFECTIVE-ADDRESS-ORACLE-CROSSCHECK.md`.

The completion must not silently bypass CCPU's own later segment-limit,
paging, or SAS checks. It returns a linear address only; `c_GetLinAdd` remains
the owner of linear-to-host-memory translation. Passing this fixture admits
the completion only to further historical link diagnostics, never to a normal
runtime path.

The SM-2 profile audit on 2026-08-07 established an additional, narrow ABI
fact. Its original EMS provider is compiled with `CCPU` defined, and its
`xt.h` declaration is `effective_addr(word segment, word offset) -> sys_addr`.
`sys_addr` is an `IU32`; this is the 16-bit-offset subset of the CCPU slot's
`c_effective_addr(IU16, IU32) -> IU32` contract, not the distinct non-CCPU
monitor-vector profile. The completion therefore exports `effective_addr` as
a lossless zero-extension forwarder to `c_effective_addr`; the fixture proves
both entry points agree for the same real-mode address.

This does not recover the non-CCPU `CPU_30_STYLE` monitor vector or authorize
an alias outside the CCPU profile. It permits only the named CCPU ABI to enter
the SM-2 link diagnostic, where EMS, SAS, and host initialization still retain
their original owners.

## Relocation Scope Of The Two Opaque Slots

An object-level audit of all 129 objects selected by the current CCPU archive
shows that `c_sas_touch` and `c_VirtualiseInstruction` are each referenced
by exactly one object: `ccpu386/sascdef.c.obj`. That file places them in the
final two callback positions of `cSasPtrs`, the generated `SasVector`
initializer. No other selected CCPU object has an undefined relocation to
either symbol.

This narrows, but does not erase, the semantic gap:

- they are not established as a direct per-instruction dependency of the
  selected CCPU objects;
- they remain callable through the published SAS vector after
  `c_sas_init()` copies `cSasPtrs`;
- a normal runtime must not manufacture a pointer, virtualized instruction
  result, BOP/SVC result, or device action if either callback is reached.

Therefore the only admissible interim disposition is a default-disabled,
fail-closed callback boundary: a bounded experiment may link the slots only
to detect unexpected use and stop before guest-visible success. This does not
recover their historical semantics or admit them to a normal runtime. It does,
however, permit later research to distinguish a startup path that never reaches
the extension callbacks from one that depends on missing generated behavior.

## `c_sas_touch`: Opaque SAS-Vector Slot

The generated SAS headers give only its ABI: an Intel address and length enter,
and an `IU8 *` returns. `ccpu386/sascdef.c` installs it in `cSasPtrs`; no
admitted OpenNT/NT4.5/XP C caller invokes the macro. The available CCPU code
does have separate checked physical and linear translation paths, but none
proves that `sas_touch` means ordinary pointer lookup, write-intent mapping,
copy-on-write materialization, page marking, or a VDD/shared-memory action.

NTVDMx64's `int 3; return NULL` patch is evidence that its maintainer did not
have that behavior either. A candidate that simply returns `c_GetPhyAdd(addr)`
would erase the undocumented range, mapping, and side-effect contract and is
therefore prohibited.

The default-disabled CCPU/CVIDC full-link diagnostic alone includes a local
guard symbol. It performs no lookup, mapping, allocation, or write; an actual
call executes `__debugbreak` and cannot be counted as a successful access.
The guard exists only to remove this slot from the linker's unrelated
dependency noise while preserving a fail-closed trace boundary.

Current disposition: semantic behavior unresolved; any invocation is a
bounded unsupported event in instrumentation, not a success path.

## `c_VirtualiseInstruction`: Opaque SAS-Vector Slot

The ABI names an instruction location in ROM, instruction size, a linear
address-or-port operand, and input data, returning an `IU32`. Its vector
position is adjacent to `IOVirtualised`, but that adjacency is insufficient to
infer its return encoding or instruction/IP/FLAGS ownership. Unlike
`c_IOVirtualised`, whose original CCPU definition and call sites exist,
`c_VirtualiseInstruction` has no admitted C implementation or caller.

It is specifically not permission to turn arbitrary instructions, `C4 C4`,
or selector `0x50` into an external BOP handler. The historical CCPU extended
BOP path is separately owned by `EDL_fast_bop` and remains fail-closed.

The default-disabled CCPU/CVIDC full-link diagnostic alone includes a local
guard symbol. An actual call executes `__debugbreak`; it cannot advance IP,
modify registers, return a usable result, or become a generic instruction,
BOP, or DOS-service mechanism.

Current disposition: semantic behavior unresolved; any invocation is a
bounded unsupported event in instrumentation, not a generic transition or
DOS-service mechanism.

## A-Path Decision Point

The source-recovery part of Plan A is complete for the available local and
publicly indexed inputs. A may continue only on these evidence-preserving
tracks:

1. obtain an exact original generator/build artifact with profile provenance;
2. create and pass the isolated `c_effective_addr` fixture, then admit only
   that independently verified completion; or
3. observe a legal historical trace that reaches either opaque slot and gives
   a complete state/input/output contract.

2026-08-09 cross-version re-audit confirms the stop condition: NT4.5 and both
local XP source snapshots retain the same `ccpu386/sascdef.c` declarations and
the same two final `cSasPtrs` initializer entries. No source tree supplies a C
definition or a selected CCPU caller. The symbols are therefore generated-vector
completion imports only, not a currently reachable recovery boundary.

The re-audit was a focused text search over the local MVDM source scopes
`opennt-4.5/nt/private/mvdm`, `winxpscodes/Source/XPSP1/NT/base/mvdm`, and
the fixed NT4 import at `src/opennt/base/mvdm`, restricted to C, headers,
assembler, generator/build metadata, and maps. It finds only the generated
`sas4gen.h` declarations and the two `sascdef.c` vector entries. This is a
scope-limited negative result: it proves neither symbol is recoverable from
those local source inputs, but does not claim that an unexamined historical
build drop or generator can never exist.

Without one of those, implementing `c_sas_touch` or
`c_VirtualiseInstruction` would be a new CCPU design, not recovery. That is
the point at which the project must explicitly choose a self-designed software
monitor/core route rather than calling it restored historical CCPU.
