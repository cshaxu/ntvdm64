# M0 T199 S24 C2 Extended-Memory Regression

## Question

Can the minimum native Bochs machine expose its already-owned RAM above 1 MiB
through a bounded, selector-blind C2 capability without importing a historical
provider or expanding the machine feature set?

## Inputs

- T199 S23 C2 admission:
  [`t199-s23-c2-extended-memory-capability-admission-001.md`](t199-s23-c2-extended-memory-capability-admission-001.md).
- Registered exception BX-MANTLE-079 in
  [`adapter-external-intrusion-exceptions.md`](adapter-external-intrusion-exceptions.md).
- Native RAM primitives in `src/bx-core/memory/misc_mem.cc`:
  `get_memory_len`, ordinary-range validation and copied read/write.
- Project-owned C2 wrapper, lifecycle integration and focused fixture.

## Procedure

1. Corrected the governance record for an uncommitted prototype before
   accepting any C2 implementation: BX-MANTLE-079 explicitly records that it
   is a remediation, not retroactive authorization.
2. Declared five fixed-width operations: query, allocate, release, resize and
   move.  Requests contain only integer handles, sizes and offsets; results
   contain only an integer handle, KiB count and physical offset.
3. Bounded the internal 32-entry table to the existing RAM aperture beginning
   at `0x100000`; allocation validates the complete ordinary writable range
   before committing a table entry.
4. Implemented resize as a no-change-on-failure operation.  A growing entry is
   copied to a separately validated free range before its table record moves.
   Move validates both allocated subranges before copying and chooses copied
   chunk direction to preserve overlapping moves.
5. Added the translation unit explicitly to the minimum-machine whole-core
   recipe and ran `Test-T199S24ExtendedMemoryBoundary.ps1` plus the existing
   minimum-machine boundary check.
6. Source-built the changed mantle files and
   `tests/bx-mantle/t199_s24_extended_memory_fixture.cc` with MSVC x64 `/MT`,
   linked them with the retained T198 S13 CPU5/Pentium-MMX x64 object closure,
   and ran the resulting native process.

## Observations

The focused 4 MiB fixture returned zero from
`artifacts/build/t199-s24-extended-memory-r1/t199-s24-extended-memory.exe`.
It observed the expected CPU reset/APIC/CPUID initialization and one native
RAM block allocation.  It proves:

- pre-initialize and post-cleanup lifecycle rejection;
- a 3 MiB reported aperture, with every allocated base at or above 1 MiB;
- non-overlapping 64 KiB allocations;
- copied allocation-to-allocation move;
- successful 64-to-128 KiB relocation with copied bytes preserved;
- unavailable oversized resize without losing the previous allocation;
- overlap-safe same-allocation move;
- subrange rejection and invalidated-handle rejection after release.

Both source-boundary scripts returned zero.  The C2 source has no adapter,
BOP, OpenNT, DOS, XMS/DPMI, selector, host-allocation or host-pointer term.
The existing whole-core recipe now names the C2 object.  A separate attempted
fresh full-core recipe r1 reached the ordinary compilation stream but was
terminated by the interactive command timeout before it could be evidence; it
is not used to support this conclusion.

## Interpretation

C2 is closed as a native machine capability, not as an XMS or DPMI provider.
It has no selector interpretation and no host capability policy.  The
T198 S13 retained object closure is used only as the CPU5 source-built baseline
for the changed-object native run; a future fresh full-source build is normal
regression work, not a missing semantic dependency.

## Follow-up

Admit C3 only after its source/ABI/failure map identifies the separate upper
memory-block requirement.  Do not connect C2 to any BOP until the complete
XMS package provider is admitted and regressed as one component.
