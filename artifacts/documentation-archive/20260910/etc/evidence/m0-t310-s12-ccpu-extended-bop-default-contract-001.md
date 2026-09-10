# M0 T310 S12 — CCPU extended-BOP default owner-contract review

## Question

Can the selected original CCPU extended-BOP default retain its documented
debug-break failure direction on both x86 and x64 without pretending that the
unrecovered opcode has a BOP service implementation?

## Original contract and selected path

The selected `softpc.new/base/ccpu386/c_main.c` CCPU decode path declares
`EDL_fast_bop(ULONG immed)` and reaches it only from the default arm of the
extended-BOP decode: `0xfe` first performs the original `c_cpu_unsimulate()`
exit; every other extended-BOP immediate is forwarded to `EDL_fast_bop`.

No selected OpenNT MVDM body supplies that default provider.  The imported
NTVDMx64 patch is the registered source record for it.  Its original meaning
is not a successful BOP dispatch: it breaks into the debugger so an unexpected
operation is visible.

## Disposition

The formal selected CCPU graph links a one-symbol build of the registered
`mvdm-softpc-patch/patches/common/fmstubs.c` source under
`MVDM_SOFTPC_PATCH_EDL_FAST_BOP_ONLY`.  The provider retains the original
`VOID EDL_fast_bop(ULONG)` spelling and immediately invokes `__debugbreak()`.
MSVC emits the same breakpoint behavior on x86 and x64, so there is no
architecture-specific inline assembly or changed return semantics.

The prior fixture-local no-op definition was removed.  The focused fixture
now invokes a non-exit extended-BOP immediate under a narrowly scoped SEH
filter which accepts only `EXCEPTION_BREAKPOINT`; it thereby proves the
selected provider's negative failure contract and then resumes the fixture's
ordinary original CCPU/SAS bounded execution.  This is observation of the
debug-break contract, not BOP semantic recovery.

Actual BOP selector/service routing, the DPMI fast-BOP operations, and every
other owner-specific extended operation remain in their named owner packages.
S12 neither introduces an adapter BOP dispatch nor selects MONITOR, kernel
VDM, Bochs or a raw-pointer route.

## x86/x64 evidence

Fresh formal MSVC `/MT` Ninja graphs built the selected CCPU fixture with the
one-symbol patch object on both host architectures.  The direct default call
raised and was caught as the expected breakpoint; both executions subsequently
reported `sas-init`, `cpu-init`, `access-init`, `returned-start` and
`returned-recursive` with exit code zero.  The generated x86 graph rebuilt
the patch and fixture rather than relying on the former test seam.  Existing
unrelated original-source diagnostics remain visible for their own owner
clusters.
