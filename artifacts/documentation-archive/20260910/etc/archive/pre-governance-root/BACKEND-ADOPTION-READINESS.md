# Backend Adoption Readiness

> **归档状态（2026-08-08）：** 此为跨项目能力比较，不是 `ntdos64` 的实现
> 路线、前置条件或验收门。当前路线只修复 OpenNT SoftPC/CCPU 与其已被原始
> caller 触达的现代 host seam。`ntvdm64`、Bochs、PCjs 等只读参考不得被链接、
> 复制或作为 runtime backend。冲突时以
> `SOFTPC-RECONSTRUCTION-GOVERNANCE.md` 为准。

Status: M48 read-only reconciliation, 2026-08-08. This compares the current
`ntvdm64/core` public source surface with M41-M47. It requests no source
change, introduces no dependency, and does not permit an `ntdos64` execution
backend.

## Current Core Facts

| Core surface | Current source evidence | Adapter relevance | Status |
| --- | --- | --- | --- |
| stopped-boundary real-mode entry plus up to 16 checked preloads | `core/machine/entry_plan_interface.[ch]` | accepts a later validated mantle memory/entry plan | usable foundation |
| frozen physical read/write/query with ordinary-RAM/provider route | `memory_interface.[ch]` | rejects ROM/provider/hole targets without exporting RAM | usable foundation |
| immutable ROM provider | `rom_mapping_interface.[ch]` | supports constructed firmware without a parallel ROM route | usable foundation |
| A20 setting and normal real-mode read/write helpers | `memory_interface.[ch]`, `memory.c` | core retains address semantics | usable foundation |
| CPU run/stop/reset lifecycle | `machine_interface.h` | supplies execution boundary, not DOS session policy | usable foundation |
| registered undefined-instruction pattern callback | `undefined_instruction_transition_interface.h` | copied GPR/FLAGS and finite outcome model is useful | insufficient: only actual real-mode `#UD` dispatch |
| physical one-span memory write | `memory_interface.h` | supports ordinary access but cannot prove compound publication | insufficient: no runtime transaction |

The evidence is from the current read-only source checkout, not from an
assumed DLL ABI. The current experiment consumes same-toolchain archives under
an explicitly default-disabled probe; it is not a stable external-wrapper
binary contract.

## M41-M47 Requirement Mapping

| Requirement family | Needs from backend | Existing core fit | Correct owner of remaining policy |
| --- | --- | --- | --- |
| M41 copied SCSINFO/COMSPEC reads | paused logical-address read with copied segment/mode context | physical access only; context missing | mantle validates ranges, adapter owns structures |
| M42 environment transaction | multi-span ordinary-RAM atomic commit plus finite BX/FLAGS result | entry plan only; runtime transaction missing | mantle transaction composition, adapter owns bytes |
| M43 CONFIG/AUTOEXEC paths | one ordinary-RAM receiver transaction at service trap | no generic service/decode boundary; no transaction | adapter owns profile paths/roles |
| M44 CMDINFO/UCOMBUF/EXECPATH | copied decode-point state and one multi-range atomic commit | #UD hook cannot observe an accepted service marker | adapter owns CMDINFO and target policy |
| M45 directory/ready/completion | constrained trap result and external session terminal event | `stop` outcome is a foundation; no generic trap context | mantle owns lifecycle epoch; runner owns exit display |
| M46 DEM reads | normal logical translation plus trap-scoped staging commit | physical query/write alone insufficient | DEM bridge owns token/file result |
| M47 fixtures | stable pause/transition/transaction identity | no transaction handle/epoch contract | fixture layer owns redaction and assertions |

No row supplies a reason to move DOS paths, CMDINFO, SCSINFO, BOP/SVC numbers,
file tokens, environment strings, or runner exit policy into core.

## Required Neutral Capabilities

### A. Configured Decode-Point Transition

The existing undefined-instruction callback must remain a distinct `#UD`
facility. A separate generic facility is needed only if core has an independent
consumer: configuration-time, frozen bounded pattern registration at the
normal physical fetch/decode point. The core passes copied fetch bytes and a
copied context sufficient to request normal translation. It may return only
unhandled, handled-resume, stop, or fault. Core validates byte consumption and
applies GPR/FLAGS output atomically. It may not modify mode, segment cache,
control registers, mappings, devices, IRQ state, or host objects.

This solves the marker-observation boundary for original historical code. It
does **not** authorize a BOP decoder, a selector dispatcher, or an arbitrary
instruction override API.

### B. Paused Ordinary-RAM Patch Transaction

The core needs a copied-span transaction whose input is already-translated
physical spans and copied byte buffers. Begin/commit is scoped to one paused or
stopped execution boundary. Before mutation, core validates all widened spans
through its frozen route as writable ordinary RAM. Commit is all-or-none and
invalidates on resume, reset, stop, fault, destruction, or context change.

It must reject overflow, provider/ROM/hole routes, stale handles, conflicting
overlap bytes, and injected publication failure without a partial write. It
does not expose a RAM pointer or know why bytes are being written. This is the
missing underpinning of M29/M33 and therefore M42-M46.

### C. External Adapter ABI, Later

Only after A and B are accepted and tested does it make sense to define a
versioned, fixed-width external C ABI. That ABI must pass opaque machine and
transaction handles plus copied values. It must not pass C++ objects,
exceptions, CRT allocation ownership, guest pointers, paths, handles, or DOS
records. It may initially be static/archive integration; a future `core.dll`
is packaging work, not a substitute for A or B.

## Adoption Order And Gates

1. **Consumer evidence:** an independent `ntvdm64` product/core scenario
   documents the same decode transition and an independent scenario documents
   the same patch transaction without importing Lane P headers or vocabulary.
2. **Core implementation:** the core owner implements and directly tests A
   and B with no registered-consumer regression, registration freeze/conflict
   cases, illegal patch rejection, stale-boundary rejection, and atomic
   failure injection.
3. **C ABI boundary:** a narrow experimental adapter ABI is defined only after
   those tests; the external project then consumes it without direct core
   source inclusion.
4. **Transaction fixtures:** M47 T-03 through T-08 run against that ABI using
   owned sentinel data, before any historical guest image executes.
5. **Historical prefix:** M47 R-10 may start from a fully validated entry plan
   and stop at the first unprovided original dependency.
6. **Command chain:** R-11 and R-12 proceed only after each reached DEM/host
   operation has its own source-backed bridge and fixture.

## Explicit Non-Readiness

The current core is not yet sufficient to build a Lane P runtime target:

- its undefined-instruction callback is not a general decoded-marker trap;
- its public memory operations cannot atomically publish M42/M44/M46 changes;
- it does not return copied logical segment/mode context for an external
  adapter's normal translation request;
- it has no approved external stable ABI; and
- the two required capabilities have not yet met their second-consumer gates.

Accordingly `ntdos64` must not add a CPU executor, an instruction decoder, a
GetVDMAddr-like mapping, a serial-write approximation, a parallel BOP/SVC
dispatcher, or a local source link to evade this result. The correct next work
is governance/evidence in the core project, while this project retains the
M41-M47 fixture contracts and historical source ownership records.
