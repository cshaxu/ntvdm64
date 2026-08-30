# M0 T310 S8 — Original SoftPC machine composition plan

## Objective

Recover one selected original SoftPC/CCPU machine profile in original startup
order. This is one integrated machine package, not a sequence of trace-driven
device patches. The profile starts from the closed S5 CCPU interval and S7
physical binding, then ends only after original creation, reset, firmware
initialization, a bounded execution interval, typed stop and teardown work in
the current Win32/x86 recovery profile. The original CCPU40 configuration is
the sole selected machine profile; x64 compatibility is deliberately deferred
until SoftPC/MVDM execution is connected.

## Selected x86 CCPU40 profile

T310 selects the original `CPU_40_STYLE + CCPU + C_VID` configuration and its
CCPU SAS-vector contract. A CPU30/MONITOR machine is a different historical
product path and is explicitly outside the product and build plan. The
selected CCPU40 preprocessing may retain `CPU_30_STYLE` where the original
CCPU headers use it as a compatibility carrier; that is not an independent
CPU30 build, runtime row, or acceptance obligation. In particular,
`ccpusas4.c` requires the CPU40 generated `SasVector` contract and may not be
compiled as a CPU30-only CCPU body.

## Ordered S owner-contract packets

Every S closes exactly one coverage-ledger cluster. It is atomic: original
definition, declaration, table/initializer, selected callers, prior repair,
selected x86 CCPU40 diagnostics and locally meaningful behavior are reread
together. x64 compatibility is outside this recovery program: it is neither
an acceptance gate nor a worklist for an active S. A
cluster cannot close merely because its selected objects compile or its
warnings have a static disposition: it must also have focused functional
verification of the recovered contract. Where the selected original profile
intentionally retains an unavailable path, this is an exact negative test of
the original failure direction rather than a fabricated positive result.
Later packets may consume a completed contract but may not reopen it with a
local workaround. The sequence is dependency order, not warning-count order.

1. S8 `SPC-CCPU-EXECUTOR-DISPATCH` (closed)
2. S9 `SPC-CCPU-ACCESS-TABLE` (closed)
3. S10 `SPC-CCPU-SAS-MONITOR-VECTOR` (closed; historical vector name, not the MONITOR product route)
4. S11 `SPC-CCPU-SAS-MONITOR-VECTOR-EFFECTIVE-ADDRESS` (closed)
5. S12 `SPC-CCPU-EXTENDED-BOP-DEFAULT` (closed)
6. S13 `SPC-SAS-MEMORY-CONTRACT` (closed)
7. S14 `SPC-MEMORY-MAPPING-BINDINGS` (closed)
8. S15 `MVDM-XMS.486-OWNER-PACKAGE` (active)
9. S16 `SPC-BIOS-FIRMWARE-BINDINGS`
10. S17 `SPC-SYSTEM-CONTROLLER-CALLBACKS`
11. S18 `SPC-HOST-SYSTEM-INTERRUPT-BINDINGS`
12. S19 `SPC-INPUT-CONTROLLER-CALLBACKS`
13. S20 `SPC-HOST-INPUT-BINDINGS`
14. S21 `SPC-STORAGE-CONTROLLER-CALLBACKS`
15. S22 `SPC-HOST-STORAGE-BINDINGS`
16. S23 `SPC-COMMS-CONTROLLER-CALLBACKS`
17. S24 `SPC-HOST-COMMS-BINDINGS`
18. S25 `SPC-CVIDC-GENERATED-DISPATCH`
19. S26 `SPC-VIDEO-CONTROLLER-DISPATCH`
20. S27 `SPC-HOST-VIDEO-BINDINGS`
21. S28 `SPC-HOST-PLATFORM-BINDINGS`
22. S29 `SPC-HOST-CONFIGURATION-BINDINGS`
23. S30 `SPC-HOST-VDD-BINDING`
24. S31 `SPC-HOST-BOP-BINDING`
25. S32 `SPC-OTHER-MACHINE-SOURCE`
26. S33 `SPC-SUPPORT-CONTROLLER-CALLBACKS`
27. S34 `MVDM-SUPPORT-LIBRARY`
28. S35 `MVDM-DOS-OWNER-PACKAGE`
29. S36 `MVDM-SIM32-OWNER-PACKAGE`
30. S37 `MVDM-DPMI32-OWNER-PACKAGE`
31. S38 `MVDM-VDMREDIR-OWNER-PACKAGE`
32. S39 `MVDM-WOW32-OWNER-PACKAGE`
33. S40 `MVDM-VDD-OWNER-PACKAGE`
34. S41 `MVDM-BDE-OWNER-PACKAGE`
35. S42 `MVDM-DBG-OWNER-PACKAGE`
36. S43 `MVDM-VDMDBG-OWNER-PACKAGE`
37. S44 `MVDM-VDMEXTS-OWNER-PACKAGE`
38. S45 `MVDM-FAX-OWNER-PACKAGE`
39. S46 `MVDM-IEUVDDEX-OWNER-PACKAGE`
40. S47 `SPC-DEBUG-UNSELECTED`
41. S48 `SPC-HUNTER-UNSELECTED`
42. S49 `P4-GLOBAL-BASELINE`: final all-cluster reread and selected-host
    `create -> reset -> firmware/machine initialization -> bounded execution
    -> typed controlled stop -> teardown` verification.

### Deferred integrated behavior

An owner packet normally supplies focused behavior evidence for its recovered
contract. Where the exact original caller is unreachable until a later packet
in this fixed dependency sequence supplies firmware, a controller or the
machine lifecycle, the earlier packet may close its source package only when
it records: every declaration/definition/caller disposition, a selected x86
CCPU40 formal build, the precise later owner and the fact that no substitute
machine/fixture was accepted as runtime proof. The deferred behavior becomes a
mandatory S49 integration condition; it is never silently reclassified as
success or discarded.

## Selected-profile source-contract audit rule

Each packet audits the complete callable contract selected by the original
Win32/x86 CCPU40 source list: function-pointer declarations, table
initializers, callback casts, pointer/integer conversions and compiler ABI/type
diagnostics reached by that owner. Source is authoritative: every candidate
requires its declaration, initializer/assignment, all selected call forms and
original owner context before it receives a disposition. Compiler diagnostics
are a coverage aid and cross-check, never proof that an unreported source
contract is safe. The record must capture source identity, original
owner/package, selected CCPU40 profile, actual x86 diagnostic where present,
pointer/address class, and one mirror/overlay/adapter/unavailable disposition.
Common contracts are handled as complete repair clusters, so a build hit never
selects the next implementation packet. A selected source recovery may retain
the original basic video/IOS port behavior and selected disk/floppy read path,
including its original unavailable/media failure direction, only after its
owning cluster is reviewed. Before any
   `cvidc` video path is enabled, the complete generated C-video ABI includes:
   `cvidc/evidgen.h` vector slots, every `vglfunc.c` initializer, every
   generated-table initializer in `evidfunc.c`, and every reached
   `base/video` handler-table or callback assignment must agree with the actual
   `S_*` callable signature in the selected CCPU40 profile. The original generated
   `S_*` bodies take four generic host-word arguments, while the vector tables
   declare typed zero- and one-argument interfaces. It remains a source ABI
   contract requiring a selected-profile disposition.
   Recovery must be generated, private `mvdm-host-overlay` carriers derived
   from the original generator output—not hand-written per-slot wrappers—and
   they must preserve the original selected vector/table order.  The CVIDC
   table carrier and the independently generated/base-video callback families
   are separately audited but are one P4 ABI closure. Their current generated
   carriers are provisional audit inputs, not a P4 completion claim, until the
   whole-tree baseline establishes their common clusters and dispositions.
   Actual guest addresses continue through the existing mapping-manager
   physical binding; internal SoftPC pointers remain native-width private
   values and are not tokenized. No display entry is enabled merely because
   the static original package archives.

   **P4 execution cadence.** The baseline is not a queue of individual
   compiler diagnostics.  The generated source/call index and compiler ledger
   are first reduced into owner-contract clusters.  A cluster is reviewed as
   one unit: original declaration(s), definition(s), table/initializer forms,
   selected caller forms, selected x86 CCPU40 diagnostic evidence and its runtime owner
   are read together. The execution order is maintained in
   `m0-t310-s8-p4-owner-contract-execution-order-001.md`: startup-critical
   CCPU, memory/system/firmware, storage and input are considered before
   optional video, communications, debugger and unselected product surfaces.
   A warning may only cause a mirror correction, overlay or adapter change
   after its entire contract cluster has that evidence. The full-tree ledger is
   a shared cross-cluster planning index, not a blocking prerequisite for each
   S: an active S reads and closes only its own owner-contract rows. Source-form
   records remain compiler-visible and leave the current x86 worklist by a
   documented cluster disposition, never by suppression. x64-only repair
   work is not admitted here.
## T310 S8 closure boundary

S8 closes only when `SPC-CCPU-EXECUTOR-DISPATCH` has a source-shaped
disposition and focused x86 CCPU40 evidence. S9--S49 then execute in order. T310 closes only
after S49 completes the all-42-cluster reread and the selected original SoftPC
`create -> reset -> firmware/machine initialization -> bounded execution ->
typed controlled stop -> teardown` profile on Win32/x86 for CCPU40. Neither
S8 nor T310 removes the remaining Bochs production route or claims
whole-program SoftPC capability/mirror purity.

Those formerly numbered S9 and S10 scopes are intentionally separate queued T
packages after the active packet: first production-route removal, then the
`i386` semantic audit, then complete SoftPC capability and mirror-purity
acceptance. Their independent proposals and ordering are authoritative in the
package-recovery program and `QUEUE.md`.

## Recovery constraints

- Directly compose the original translation unit first. A modern public API
  may bind a historical dependency but cannot replace an available controller
  algorithm.
- **S8 external-boundary rule.** For an executor call that crosses into a
  later owner packet, S8 closes the caller-side contract only: identify the
  original declaration and call form, preserve its ABI and failure direction,
  and give it one `binding-only`, `adapter-backed`, or
  `original-unavailable` disposition sufficient for the selected CCPU archive,
  forced link, and focused executor observation. S8 does not implement the
  crossed owner's business semantics, controller behavior, or host API
  capability. Those remain the acceptance responsibility of their named later
  owner packet. A focused S8 fixture may use only an explicitly admitted
  boundary result; it may not rely on an unregistered empty stub or fabricated
  success.
- A source-bearing native pointer, handle or 32-bit identity crossing uses the
  existing session mapping manager. This remains mandatory on x86. Short-lived
  guest leases cannot be physical backing or asynchronous device state.
- No package selects a Bochs, MONITOR, kernel-VDM or `src.old` route.
- Any newly authored behavior belongs in its named adapter and must preserve
  the original function shape, ordering and failure direction. Mirror changes
  carry `DIVERGENCE:` and their README register entry.

## P1 acceptance boundary

P1 is complete only once the complete source-selected reset/firmware/media
group has a direct composition attempt and an owner/disposition for every
outbound form. It may establish an explicit original unavailable result for
missing media, but it may not claim a runnable device profile until P2--P5.

## T310 final closure boundary

S8 closes only `SPC-CCPU-EXECUTOR-DISPATCH`.
T310 closes only after the sequential S8--S49 packets complete. Formal MSVC
`/MT` Win32/x86 Ninja graph for CCPU40 must prove the selected SoftPC
`create -> reset -> firmware/machine initialization -> bounded execution ->
typed controlled stop -> teardown` path. Bochs production-route removal and
full SoftPC capability/mirror-purity acceptance remain explicit subsequent T
packages, not implicit T310 exit conditions.
