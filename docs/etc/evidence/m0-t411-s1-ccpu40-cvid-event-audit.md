# M0 T411 S1 — CCPU40 C-VID and event-path audit

## Question

What selected CCPU40 C-VID/event differences, overlays and authored providers
remain relative to the pinned OpenNT SoftPC tree; which are merely x86 build
carriers, which duplicate or replace original behavior, and what is the
smallest source-first restoration proposal?

## Inputs and method

The audit compared the selected mirror with
`O:\repos.external\OpenNT\base\mvdm\softpc.new`, read the formal
`New-T310OriginalSoftpcNinja.ps1` graph and the generated-binder inputs, and
searched all current C-VID, CCPU and event references. No product source,
build graph or runtime behavior changed. The prior T405 U01 ledger and T406
C-VID x86-carrier evidence were re-read; their earlier conclusions are
evidence, not a substitute for the current-file inventory.

The bounded C-VID direct-diff cohort is eight mirror files: `ev_glue.c`,
`evidfunc.h`, `evidgen.h`, `gdpvar.h`, `j_c_lang.c`, `j_c_lang.h`,
`sevid019.c`, and `sevid020.c`. Current-versus-pinned numstat is **+33/-21**.
`quick_ev.c` and `qevnt.c` are byte-identical to the pinned source; the formal
system manifest selects original `qevnt.c`, not `quick_ev.c`.

## Ledger

| ID | Current material | Original owner / selected caller | Classification and disposition |
| --- | --- | --- | --- |
| C01 | `base/cvidc/ev_glue.c`, +13/-2 | Original C-VID setup; `base/support/main.c` calls `setup_vga_globals`. | Two added binder hooks are composition changes, not a second renderer. The stdio/varargs spelling and BIOS callback ABI are build/ABI carriers. Retain pending one coherent vector-layout link test; do not restore old implicit declarations. |
| C02 | `evidfunc.h`, +13/-17 | Original generated C-VID callback declarations. | Typed four-argument mark callback declarations replace duplicate unprototyped declarations. This is a layout/signature cohort, not autonomous logic. Retain until all generated slots and selected callback targets are compile-verified together. |
| C03 | `evidgen.h`, `gdpvar.h`, `j_c_lang.[ch]`, `sevid019/020.c`, +7/-2 after C02/C01 | Original generated tables/J-code. | No provider policy. Three edits are blank/trailing-whitespace or declarations; they are at most **3 removable nonsemantic lines**, and trailing whitespace cannot be restored under the repository diff rule. Do not spend an implementation S on them. |
| C04 | `mvdm_cvidc_vector_binding.[ch]`, **86 lines** overlay; `GenerateCvidcCpuBinding.mjs`, **39 lines** | Original `c2cpusad.h` slot metadata, `cpu4gen.h` vectors, C-VID `setup_vga_globals`. | The vector assembly is a real missing generated-product carrier. It has no established composable original generated unit. It must be retained provisionally, but its per-slot table is audit material, not proof of correctness. |
| C05 | C04's `mvdm_cvidc_*jump_*` state and three generator mappings | Original `qevnt.c` calibration/restart consumers and C-VID `accessfn.c` accessors. | **Autonomous timing replacement.** It collapses separate calibration/restart queries into one mutable value initially 100. `qevnt.c` has original policy but needs provider values; no selected original CCPU backing body was found. This is the highest-priority semantic recovery row. Do not delete it until a source-shaped provider and qevnt lifecycle test exist. |
| C06 | `base/support/main.c` DIV-162/DIV-182 calls; `nt_cprgs.c`/`host_cpu.h` CCPU vector declarations | Original startup/CPU-vector ordering. | Calls establish C-VID video before config and CPU access after `cpu_init`; they are composition ordering, not substitute CPU behavior. Retain until the generated-carrier selection proves an original unit can supply the same order. |
| E01 | `ccpu386/c_main.c`, current +161/-38 | Original CCPU event bitmap consumers; timer/reset/host-I/O/PIC producers. | **Autonomous synchronization policy.** CAS/Interlocked helpers changed observation-versus-consumption order; the prior U01 audit proves a lost RESET path. The file-level numstat also includes declarations and diagnostic hooks, so it is not a valid event-policy removal count. This row is removable only as a complete event profile after producer/thread analysis. |
| E02 | `adapter-mvdm-host-out/softpc/mvdm_softpc_event_thread.c`, **30 lines**; event-thread calls in `nt_event.c` | Original event-manager shutdown and NT thread alert. | Small bounded modern wait/alert binding. It is not C-VID code and does not duplicate an original event queue; retain for a lifecycle-specific proof under U07-T/U08-W. |
| E03 | `nt_event.c`, `nt_timer.c`, `nt_thred.c`, `nt_event.h` | Original Console/event/timer/thread owners. | Their current deltas also contain Console recovery, diagnostics and termination seams. They are not all CCPU C-VID diffs. Only their selected event producer/consumer and shutdown edges enter T411; Console ownership remains closed T410. |
| X01 | `mvdm-host-overlay/.../localfm.c` and `sas_overwrite_memory.c` | Original global carrier and `ntstubs.c:sas_overwrite_memory`. | Adjacent C-VID composition support, but not a C-VID/event duplicate. The first avoids a second Sas global; the second is a direct forwarding facade. Retain outside this S1's proposed deletion count. |

## Findings and removal accounting

There are **eight direct C-VID mirror-diff files**, **two C-VID binder overlay
files**, **one generator**, one CCPU event-policy body, and one bounded
event-thread adapter in the selected cohort. They are not 13 independent
algorithms.

- Confirmed immediately removable behavior: **0 lines**. The 3 nonsemantic
  lines are not worthwhile and two would violate whitespace governance if
  restored verbatim.
- Conditional autonomous-replacement target: roughly **20 source/generator
  lines** for C05's shared timing state and mappings, only after a replacement
  source-shaped provider proves distinct calibration and restart semantics.
- E01 has **no honest line-removal estimate yet**. Its surrounding file's
  +161/-38 includes unrelated declarations and diagnostic hooks; current
  worker threads make a blind restoration unsafe, and the original plain
  bitmap is not automatically thread-safe in standalone composition.
- Binder footprint: **125 lines** (86 overlay + 39 generator) is necessary
  provisionally; no original generated binding translation unit has yet been
  found. It is therefore not counted as removable.

The only current numeric removal target is therefore **about 20 conditional
source/generator lines** from C05, not the entire 125-line binder and not the
gross C-VID diff count. The ideal of zero autonomous policy remains valid,
but only C05 and E01 are currently proved policy replacements. All other rows
are source-shaped ABI or composition carriers pending a complete slot/profile
proof.

## S2 restoration proposal

1. **Slot and generation closure.** Reproduce the `c2cpusad.h` public/private
   slot list, resolve every null/overridden slot against original CCPU
   definitions, and prove exactly one `Cpu`, `Sas` and `Video` owner. Build a
   focused x86 link fixture that executes original `setup_global_data_ptr`,
   `setup_vga_globals` and the selected C-VID write/overwrite path. No timing
   change in this group.
2. **Original quick-event provider closure.** Trace the original CCPU owner
   for the three `Get/SetJump*` slots, including `qevnt.c` initialization,
   recalibration, reset and overflow behavior. Prefer composing that body or
   a smallest original-shaped facade; delete the local shared-100 provider
   only when calibration and restart are independently observed.
3. **Complete CCPU event profile.** Audit every producer and consumption site
   of the event bitmap, then restore original observation/acknowledgment
   ordering while retaining only the minimum proven atomic transport required
   by worker threads. Test RESET, PIC delivery/acknowledgment, SIGALRM/SAD,
   wake-before-wait, callback reentry and teardown. This group owns E01 and
   must not alter instruction algorithms.
4. **Integration and reduction review.** Run the formal x86 CCPU40 build,
   C-VID video-memory/EMS regression, and ordinary COMMAND/EDIT smoke only
   after groups 1--3 pass. Recalculate actual removed lines; do not stage an
   EXE until it demonstrates an admitted improvement.

## Confidence and limitation

High confidence: C05 and E01 are local policy replacements, and C04 is a
missing generated-carrier workaround rather than a renderer. Medium
confidence: the exact original provider capable of replacing C05 has not yet
been located. Therefore S1 recommends the four groups above and stops before
any semantic change.
