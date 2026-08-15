# T95 S6 CPU-core compile-object ledger

## Question, fixed inputs, and procedure

This record answers only which original Bochs CPU source families must be
accounted for before an object/link target can be admitted for the accepted
CPU3, 32-bit, FPU-off projected configuration.  It begins with the
syntax-003 candidate `cpu/fetchdecode.cc`, `cpu/ia_opcodes.h`, and the imported
CPU build metadata; no project file is a build input and no compiler, linker,
archiver, configuration, or source copy was invoked.

The fixed configuration facts come from
`tools/build/t95-s6-msvc-config-projection-manifest-v2.json`: `BX_CPU_LEVEL=3`,
`BX_SUPPORT_X86_64=0`, `BX_SUPPORT_FPU=0`, `BX_SUPPORT_SMP=0`,
`BX_SUPPORT_APIC=0`, `BX_CONFIGURE_MSRS=0`, `BX_SUPPORT_VMX=0`,
`BX_SUPPORT_SVM=0`, `BX_SUPPORT_AVX=0`, and `BX_WITH_WX=0`.

Read-only searches performed:

```powershell
rg -n "bx_define_opcode|BX_IA_FPU_ESC|BX_SUPPORT_(FPU|X86_64|AVX|3DNOW)" refs/bochs/cpu/ia_opcodes.h refs/bochs/cpu/fetchdecode.cc
rg -n "init_FetchDecodeTables|BxOpcodesTable|BxError|FPU_ESC" refs/bochs/cpu/fetchdecode.cc refs/bochs/cpu/init.cc refs/bochs/cpu/fpu_emu.cc
Get-Content refs/bochs/cpu/Makefile | Select-Object -First 170
Get-Content refs/bochs/vs2008/cpu.vcproj | Select-String -Pattern 'RelativePath="..\\cpu\\.*.cc"'
rg -n "BX_CPU_C::(initialize|reset|cpu_loop)|DEV_cmos_get_reg|DEV_vga_mem_(read|write)|DEV_dma_raise_hlda" refs/bochs/cpu
```

`ia_opcodes.h` is not generated at this point: it is a checked-in static macro
list, included at `fetchdecode.cc:169` to initialize `BxOpcodesTable` and again
at `fetchdecode.cc:2025+` for names/features.  No generation input or generator
is needed to explain the map, and none has been inferred.  `cpu/cpudb/*` is a
separate generated/static CPU-description family; `init.cc` only reaches it at
`BX_CPU_LEVEL >= 4`, so it is rejected by the fixed CPU3 condition.

## Classification ledger

The historical `cpu/Makefile` puts the following source objects in `OBJS` for
the 32-bit CPU archive.  That is source/build-metadata evidence of the native
family, not a permitted archive invocation or a proof that every resulting
object survives a future link.

| Family / imported objects | Classification | Exact reason / edge |
| --- | --- | --- |
| Table and initialization: `init`, `fetchdecode`, `icache`, `resolver`, `access`, `access32` | mandatory native CPU core | `BX_CPU_C::initialize()` unconditionally calls `init_FetchDecodeTables`; table entries use decode/address helpers and member-function targets. |
| Scalar arithmetic/data: `arith8`, `arith16`, `arith32`, `mult8`, `mult16`, `mult32`, `logical8`, `logical16`, `logical32`, `shift8`, `shift16`, `shift32`, `bit`, `bit16`, `bit32`, `bcd`, `data_xfer8`, `data_xfer16`, `data_xfer32`, `load` | mandatory native CPU core | Unconditional `ia_opcodes.h` entries name these target methods or their `LOAD_*` memory forms; replacing or omitting them would change native instruction semantics. |
| Control/stack/segmentation: `ctrl_xfer16`, `ctrl_xfer32`, `ctrl_xfer_pro`, `call_far`, `jmp_far`, `ret_far`, `iret`, `stack`, `stack16`, `stack32`, `segment_ctrl`, `segment_ctrl_pro`, `protect_ctrl`, `tasking`, `vm8086`, `soft_int`, `flag_ctrl`, `flag_ctrl_pro` | mandatory native CPU core | The static opcode table contains control-transfer, protected-mode, stack, interrupt, and flag handler pointers. CPU3 does not authorize a hand-selected instruction subset. |
| CPU state/exception/memory boundary: `exception`, `crregs`, `paging`, `smm`, `io`, `string`, `proc_ctrl`, `debugstuff`, `crc32` | mandatory native CPU core | Table targets and reset/initialization support cross these units. Feature bits may redirect at runtime, but `BxOpcodesTable` has already taken the target addresses. |
| FPU-off and extension-handler compatibility: `fpu_emu`, `mmx`, `sse`, `sse_move`, `sse_pfp`, `sse_rcp`, `sse_string`, `xsave`, `aes` | mandatory native CPU core | This is the non-obvious closure: `fetchdecode.cc:405-413,941-959` selects `BX_IA_FPU_ESC` when FPU is off, and `ia_opcodes.h:702` names `FPU_ESC`; `fpu_emu.cc:42-53` defines that FPU-off handler.  The static opcode map also names MMX/SSE-family members even where CPU-level bodies reduce to original fallback behavior.  Runtime feature masking in `init_FetchDecodeTables` happens after the member addresses are emitted. |
| CPU loop/event: `cpu`, `event` | default-but-not-required for reset-to-bounded-stop | The composition deliberately calls `initialize()` then `reset(HARDWARE)` and does not call `cpu_loop()`. `cpu.cc` owns the loop and `event.cc` owns wait/event behavior. They become unresolved only if an object-level link retains their uncalled members. |
| CPUID/MSR/APIC: `generic_cpuid`, `msr`, `apic` | explicitly rejected due to fixed CPU3/no-MSR/no-APIC configuration | `init.cc` guards CPUID construction at `BX_CPU_LEVEL >= 4`, configurable MSRs at `BX_CONFIGURE_MSRS`, and the projection forces CPU3/APIC/MSR off. These are not a substitute instruction implementation. |
| x86-64 archive family: `access64`, `arith64`, `ctrl_xfer64`, `data_xfer64`, `fetchdecode64`, `logical64`, `mult64`, `shift64`, `bit64`, `stack64`, `avx`, `avx_pfp`, `avx_fma`, `avx2`, `gather`, `bmi64`, `tbm32`, `tbm64`, `xop` | explicitly rejected due to `BX_SUPPORT_X86_64=0` | These are `OBJS64` in the imported CPU Makefile; `fetchdecode64.cc` itself is guarded by `BX_SUPPORT_X86_64`. |
| Virtualization: `svm`, `vmx`, `vmcs`, `vmexit`, `vmfunc` | explicitly rejected due to fixed `BX_SUPPORT_SVM=0` and `BX_SUPPORT_VMX=0` | The sources are feature-guarded, while `init.cc` only calls `init_VMCS()` under `BX_SUPPORT_VMX`. |
| 3DNow: `3dnow` | explicitly rejected due to `BX_SUPPORT_3DNOW=0` | `ia_opcodes.h:791-815` encloses its specific targets in that macro. |
| Per-object survival within the mandatory families | unresolved | The source proves static target ownership, but not whether the future MSVC object model can isolate uncalled members. This record intentionally does not collapse native source families into a link recipe. |

## Static-map reconciliation and post-core edges

The resolution is therefore a native CPU **family**, not a generated opcode
map or an adapter seam.  The checked-in macro map takes addresses for every
included handler before `init_FetchDecodeTables()` uses feature bits to replace
unsupported opcodes with `BxError`.  A CPU3/FPU-off observation still needs the
original `FPU_ESC` fallback and cannot omit semantics merely because the CPU
loop is outside this fixture.

After the CPU family is accounted for, source-level non-CPU edges remain:

| CPU edge | Imported owner | Status |
| --- | --- | --- |
| `exception.cc` triple-fault path -> `DEV_cmos_get_reg` | iodev/CMOS | unresolved/rejected until retention is proved; the admitted minimal SIM path selects the original shutdown branch. |
| `io.cc` port instructions -> `bx_pc_system.inp/outp`; `event.cc` -> DMA/timer facilities | `pc_system.cc`, iodev | default-not-required without CPU loop; no source-only object-retention proof. |
| `access*`, `paging`, `icache`, handler memory forms -> `bx_mem` physical-memory methods | `memory/memory.cc`, `memory/misc_mem.cc` | required external memory family, already subject to no-SIM helper boundary; exact retained set unresolved. |
| logging/SIM diagnostics | `logio.cc`, `gui/siminterface.cc`, `gui/paramtree.cc` | required for the admitted composition setup, but UI/debug members remain unproven at object retention. |

## Result and limits

The missing CPU closure is now narrowed but not solved: the complete original
32-bit `OBJS` family is the only source-backed native candidate for the static
handler map, with the named CPU3 configuration rejections above.  It is not a
compile, archive, link, or executable recipe.  The remaining decision is
whether a future closure package may establish the CPU-family object model and
its non-CPU retention edges without reducing or reimplementing instruction
semantics.  No adapter/OpenNT behavior, device feature, source, configuration,
or exception registration changed in this audit.
