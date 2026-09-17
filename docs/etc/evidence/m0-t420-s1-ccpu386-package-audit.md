# M0 T420 S1 — CCPU386 complete-package audit and recovery design

## Question

Can the selected original `softpc.new/base/ccpu386` package be described as a
complete, finite CCPU40 package rather than a set of trace-reached source
files; which current differences are direct original composition, mandatory
host-profile seams, diagnostic residue, or a genuine correction; and what
must the next C-VID package S prove before CCPU can be certified complete?

## Frozen inputs

- Current selected mirror: `src/mvdm/softpc.new/base/ccpu386`.
- Primary comparison source: `O:\repos.external\OpenNT\base\mvdm\softpc.new\base\ccpu386`.
- Selected build manifest: `src/mvdm/softpc.new/base/ccpu386/sources` and
  `tools/build/New-T310OriginalSoftpcNinja.ps1`.
- Most recent formal product graph/map:
  `build/M0-T419/S5/exe-component-rename-x86-003/{build.ninja,source-manifest.json,ntvdm.exe.map}`.
- Comparative correction source, used only under the source-policy exception:
  `O:\repos.external\ntvdmx64\ntvdmpatch\patches\common\ccpu.patch`.
- Earlier, narrower records: [T411 C-VID vector closure](m0-t411-s2-cvidc-vector-closure.md),
  [T411 event-profile disposition](m0-t411-s4-ccpu-event-profile.md),
  [T407 FSTI recovery](m0-t407-s14-fsti-fixed-selector-recovery.md), and
  [T413 exception scope](m0-t413-s5-exception-scope.md).

The audit artifacts (`manifest-members.txt`, hashes, selected-member ledger,
numstat and include ledger) are disposable, reproducible files under
`build/M0-T420/S1/ccpu386-package-audit/`; they are not product inputs.

## Complete manifest and build selection

The original `sources` manifest has 130 C translation units.  The formal x86
generator compiles exactly 130 CCPU object nodes into `original-ccpu386.lib`:
it excludes the manifest's `ntstubs.c` and adds `localfm.c`, leaving the count
unchanged.  This is a profile selection, not a silent omission:

| Item | Disposition | Reason |
| --- | --- | --- |
| 129 ordinary manifest members | Selected directly | Original CCPU40 implementation body. |
| `ntstubs.c` | Original evidence, excluded from product profile | It defines a stub CPU/FPU/SAS carrier alongside the real `fpu.c` and `ccpusas4.c`; selecting both yields duplicate providers. |
| `localfm.c` | Selected state carrier | It supplies the historical `Gdp`, `Cpu`, and `Video` tentative state declarations, but deliberately omits `Sas`, whose unique CCPU40 owner is `ccpusas4.c`. |
| `vglob.c` (on disk but not in manifest) | Excluded from this package | It is the alternate CCPU-local GDP profile.  The selected C-VID profile owns its corresponding accessor in `base/cpu/src/evid/vglob.c`; this is an S2 interface, not a CCPU member. |

The final T419 `ntvdm.exe` map extracts 120 of the 130 archive objects.  The
ten non-extracted members are `bswap`, `c_getset`, `ccpupig`, `cmpxchg`,
`invd`, `invlpg`, `localfm`, `nop`, `wbinvd`, and `xadd`.  Static-library
extraction is demand-driven; this means those source bodies compile under the
selected profile but the established COMMAND/MEM/EDIT workloads do not yet
reference them.  It is neither evidence that they are absent nor permission to
delete them.

Source comparison has three exact cohorts among the 130 original manifest
members: 109 byte-identical files, 9 files differing only in line endings,
and 12 files with semantic textual differences.  The latter total **+298/-54
lines**.  `localfm.c` is one additional, selected non-manifest carrier with a
three-state/one-SAS ownership difference.

## Whole-package interface and lifecycle ledger

All 130 selected members are classified by their original CCPU internal
headers (`c_addr`, `c_reg`, `c_seg`, `c_stack`, `c_prot`, `c_xcptn`,
`c_main`, and instruction-specific headers).  No selected member has an
unidentified internal provider.  The finite external edges are:

| CCPU responsibility | Original outgoing owner | Current finite owner and required lifecycle proof |
| --- | --- | --- |
| CPU public/private vector, `Gdp`, `Video`, `Sas`, generated table slots, initial early-video bind and later complete bind | `softpc.new/base/cvidc` | C-VID selected original sources plus the bounded binding carrier. S2 must prove one initialized state owner, early bind, complete bind, repeat bind and teardown/reset. |
| SAS reservation/release and normal M backing | `softpc.new/host/src/sim32.c` | Selected original CPU40 `host_sas_init/term`, backed by worker-local memory/session binding. S12 owns full host lifecycle; CCPU calls only the original init/term ordering. |
| Physical alias resolution and EMS page-frame translation | NT4 kernel VDM semantic carriers | Worker-private physical-mapping seam. It is a finite address/span operation, not a CPU policy; S15/XMS and S18/DPMI must prove map/unmap/teardown. CCPU retains only the original `c_GetPhyAdd` caller. |
| Timer, reset, PIC acknowledge and BIOS interrupt transfer | `base/system`, `base/bios`, host event source | S4/S6 own producer/consumer lifecycle and device initialization. CCPU owns its event bits and delivery point only. |
| Protected DPMI software/hardware/exception interrupt hooks | `softpc.new/host/src/nt_inthk.c` and DPMI32 registration | S12/S18 own registration and rundown. CCPU calls the original-shaped hook exactly at protected interrupt/exception delivery. |
| Per-thread simulation and exception frame | `ccpu386/ntthread.c` | CCPU-owned TLS allocation, initialize-before-simulate and release-at-thread/worker teardown; S1 records the source but S12 must prove worker thread lifetime. |
| Default-off historical diagnosis | worker-private termination observer | Not an execution provider.  The CCPU calls are removable diagnostic residue once their closed investigations have no active consumer; they must not be used to justify CPU behavior. |

The actual lifecycle is therefore: worker host initialization → C-VID early
video publication → `sas_init` (original vector install, scratch allocation,
host SAS allocation) → C-VID complete publication → `c_cpu_init`/per-thread
TLS → simulate/interrupt/exception loops → source-owned CPU unwind → worker
host teardown → `sas_term`.  C-VID publication occurs in two phases, so a
CCPU-only claim cannot prove the shared state valid.  This is why S2 remains
next in the already-approved order; no reorder is needed.

## Difference disposition

| File/cohort | Current delta | Source-first disposition |
| --- | --- | --- |
| `c_xcptn.c` (+16/-6) | Five `host_exint_hook` scopes corrected with braces. | **Retain and test.** Upstream OpenNT's missing braces unconditionally call `c_cpu_continue()` even when the hook returns false. The exact brace correction appears in the approved NTVDMx64 comparative patch. S2 must not change it; later S18 must exercise handled and unhandled protected exceptions. |
| `fpu.c` (+12/-6) | Representation-safe FP copy and rounded 64-bit FIST conversion. | **Retain pending CCPU package regression.** This is a semantic correction, not an x64-width shim. S1 found no alternative original body with those semantics; a focused x87 fixture is required before any revision. |
| `c_seg.c` (+20) | Temporary protected selector `0040h` carrier before DOSX publishes the BIOS-data descriptor. | **Retain as a proven bounded recovery seam.** T407 source/runtime evidence identifies original FSTI before ordinary descriptor publication. S18 must prove final publication supersedes this early carrier and no broader selector policy exists. |
| `ccpusas4.c` (+38/-1) | Original SAS body plus physical-map probe/translate, typed declarations and default-off stores/RAM observation. | **Split.** Retain the finite mapping calls until the map owner proves an original-compatible replacement; delete the diagnostic-only observer calls when their active investigations are retired. Do not substitute the NTVDMx64 HAXM SAS body. |
| `c_main.c` (+158/-34) | Modern declarations, correct `jmp_buf` indirection, atomic event transport, protected IRQ hook/signed reject, and default-off observers. | **Split.** Keep the `setjmp` object form and signed reject protection. The atomic event transport cannot be blindly restored: host producers are concurrent. However T411's earlier claim that its HALT consume order matched original was later contradicted; S4 must re-prove reset/HALT producer-consumer ordering. Remove the default-off BOP/low-fault/PIC observations once no longer needed. |
| `c_intr.c` (+14) | Protected software-interrupt hook plus one IVT observer. | **Retain the hook conditionally, delete observer residue.** The hook is required to give the original DPMI registration owner a CCPU delivery point; it is not sourced from the OpenNT CCPU body and must be proved in S18. |
| `c_page.c`, `c_reg.c`, `popf.c`, `ntthread.c`, `sasCdef.c`, `zfrsrvd.c` | Headers, prototypes, typed null/return and varargs forms. | **Keep as minimal compiler/ABI compatibility seams until a formal x86 A/B proves their direct original spelling compiles and retains the same call ABI.** These are not autonomous algorithms. |
| 109 identical + 9 line-ending-only files | No semantic project logic. | **Direct original composition.** Normalize line endings only when a repository-wide mirror-format operation is separately admitted; no behavioral work belongs here. |
| `localfm.c` | Omits duplicate `Sas` declaration and uses C-VID layout headers. | **S2-owned composition boundary.** It must either become a single explicit state publication arrangement or be replaced by the original generated carrier; it cannot be treated as an independently complete CCPU state provider. |

This identifies a reduction cohort but does not claim it is safe to delete in
S1: the CCPU diagnostic calls in `c_intr`, `c_main`, and `ccpusas4` are
nonfunctional, default-off residue and are the first removals to measure after
the required lifecycle proofs.  The mapping, descriptor, exception, FPU, and
event deltas remain functionality until their respective owner tests pass.

## S2 implementation and verification brief

S2 is exactly `softpc.new/base/cvidc`, not a combined CCPU/C-VID task.  It
will freeze the C-VID manifest and generated inputs, then:

1. Map every C-VID definition and generated vector slot to one provider;
   reject common/tentative duplicate state as proof of ownership.
2. Preserve the original two publication points: early Video before
   `config()` and complete `Cpu/Sas/Video` publication at
   `setup_vga_globals`.  Prove initial, repeat, reset and teardown behavior.
3. Re-audit the current binding carrier against original generated artifacts.
   Delete it only if an original generated unit is available; otherwise reduce
   it to the smallest typed table assignment seam and keep it outside either
   mirror root.
4. Prove `Sas_overwrite_memory`, quick-event calibration/restart and all
   reached CCPU private/public slots do not bind back into C-VID shims.
5. Run the source-level vector contract fixture, fresh formal x86 build, and
   the mandatory direct and interactive `COMMAND`, `MEM`, `EDIT`, then
   post-EDIT `MEM` regression matrix.

S2's exit report must separately measure C-VID mirror diff, removed binding
code, retained adapter lines and any CCPU delta it enables to retire.  It may
not declare CCPU event, DPMI interrupt, host-SAS, or descriptor lifecycle
complete; those remain assigned to S4/S12/S18.

## Procedure, result, and limitations

The audit parsed the original manifest and current generator, SHA-256 compared
every selected member, reviewed every semantic `git diff --ignore-space-at-eol`,
enumerated direct include boundaries, and read the latest formal map.  The
formal generator has 130 CCPU compile nodes; the map proves the exact 120/10
archive extraction split and providers such as `c_main`, `ccpusas4`,
`host_hwint_hook`, `host_sas_init`, and C-VID vector binding.

Confidence is high for manifest selection, current textual differences,
archive extraction, and finite owner assignment.  It is deliberately not a
runtime certification: no source changed and S1 did not rerun a product
matrix.  The two material limitations are explicit: actual C-VID state
publication remains S2 work, and event reset/HALT equivalence must be
revalidated by S4 because earlier evidence records a contradicted assumption.

