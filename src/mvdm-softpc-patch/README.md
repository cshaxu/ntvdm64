# MVDM SoftPC Patch Register

This is the sole production location for individually reviewed patch bodies
derived from NTVDMx64 while recovering original `mvdm-host/softpc.new`
behavior. It currently retains the source-proven x86 generated carriers and
the owner-admitted `fmstubs.c` body.

The original SoftPC source remains in `mvdm-host`. A patch needs a minimal,
registered `DIVERGENCE:` hook in its original mirror caller.

| ID | Original caller and interface | NTVDMx64 provenance | Original purpose | Divergence reason | Patch file | Mapping-manager binding | x86/x64 evidence | Removal condition |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| MVDM-SOFTPC-PATCH-001 | `softpc.new/base/ccpu386/sasCdef.c` / `PigReg_c.h`, `sas4gen.h`, `gdpvar.h` | `ntvdmpatch/patch.cmd:7-25`, `patches/common/sas4gen.patch`, and `patches/common/gdpvar.patch` (SHA-256 `2CC0F9A09D54E792460FEEE623D83E47094A42DE2A9020667F59B5A07DFAAE46`) | Supplies the generated CPU-state record and SAS/GDP vector declarations consumed by original CCPU and host-control sources | The selected OpenNT union retains no x86 `host/genPg/inc` or x86 product carrier, so the original CCPU target cannot resolve its generated include, matching vector shape, or `GDP_PTR` contract | `x86/prod/PigReg_c.h`, `x86/prod/sas4gen.h`, `x86/prod/gdpvar.h` | None: declaration carrier only; no host address conversion | T310/S2 standalone `sasCdef.c` passes on x86/x64; original `nt_cpu.c` passes on x64 with unmodified 32-bit pointer-truncation warnings | Replace if a selected byte-identical original x86 generated carrier is recovered |
| MVDM-SOFTPC-PATCH-002 | `softpc.new/base/ccpu386` reached SAS/fast-BOP hooks: `EDL_fast_bop`, `c_VirtualiseInstruction`, `c_sas_touch`, timestamp helpers | `O:\\repos.external\\ntvdmx64` commit `84a13d2e7bb1a55d11148971e5b9c8ec99f670bf`, `ntvdmpatch/patches/common/fmstubs.c`, source SHA-256 `03F45AD3124379B2462AFA5434EF479012421C45096C6CCDCE4296B151BF3025` | Supplies the NTVDMx64 patch's original fallback definitions for unresolved CCPU hooks and timing helpers | Preserve the original immediate debugger-break contract. The only active divergence replaces each x86-only inline `int 3` with the uniform MSVC `__debugbreak()` intrinsic on both host architectures. | `patches/common/fmstubs.c` (registered syntax-only divergence) | No replacement implementation is retained. A future recovery must begin from the original SoftPC caller and a separately admitted same-shaped adapter/overlay; it may not silently replace this breakpoint. | Hash provenance recorded; the registered derivative compiles/archives on both x86 and x64 as `ntvdmx64-softpc-patch-evidence.lib`, but is deliberately excluded from the candidate forced-link runtime audit. | An owner-approved complete runtime contract plus a source-shaped adapter/overlay, focused behavior test and removal of the default breakpoint disposition for that one hook. |
| MVDM-SOFTPC-PATCH-003 | `softpc.new/host/src/nt_error.c` `ErrorDialogBoxThread` passed directly to `CreateThread` | `O:\\repos.external\\ntvdmx64` commit `84a13d2e7bb1a55d11148971e5b9c8ec99f670bf`, `ntvdmpatch/patches/minnt/callconv.patch`, SHA-256 `1D3FA424A9C3337AE54E1BD5C8490BBC8FDA2F8E0A1D39E2275D7FB412823D78`, hunks 27-36 | Gives the historical error-dialog worker the Win32 `LPTHREAD_START_ROUTINE` calling convention expected by `CreateThread` | The original cdecl declaration is rejected by the current x86 SDK ABI; the supplied patch changes only the calling convention on declaration and definition | `patches/minnt/callconv.patch` (byte-identical evidence); registered two-site `WINAPI` mirror divergence | None: the thread parameter is a synchronous host-owned `ERRORDIALOGINFO`, not a guest/component identity | T310/S2 x86/x64 recompilation passes after the two-site correction | Remove only if an ABI-compatible original selected source is recovered |

| MVDM-SOFTPC-PATCH-004 | `softpc.new/base/ccpu386/c_main.c` / `EDL_fast_bop(ULONG)` | Same `ntvdmpatch/patches/common/fmstubs.c` provenance as PATCH-002 | Keeps the reached extended BOP fallback at its patch-provided debugger-break disposition | The full patch object also defines unrelated empty placeholders. Linking it whole would falsely mask distinct unresolved SoftPC interfaces. | `patches/common/fmstubs.c`, compiled with `MVDM_SOFTPC_PATCH_EDL_FAST_BOP_ONLY` | None: the immediate is a fixed guest instruction scalar, not host identity. | Formal x86/x64 Ninja links a one-symbol `ntvdmx64-softpc-edl-default.lib`; the full patch remains evidence-only. | Replace only after the original extended BOP operation obtains an admitted source-shaped provider, semantics and focused execution evidence. |

Rules:

- This is not a generic helper, adapter, or alternative machine root.
- A patch preserves original spelling, parameters, order and failure semantics.
- It reaches mechanics only through `adapter-mvdm-host-out/softpc`; it cannot
  call or include `bochs-core`, `adapter-bochs`, `session`, or `app`.
- A host pointer, handle, or VDM alias conversion uses the approved mapping
  manager lease path and is documented in the register.
