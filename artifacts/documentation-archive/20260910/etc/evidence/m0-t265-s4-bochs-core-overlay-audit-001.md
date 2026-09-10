# M0 T265 S4 Bochs-core overlay audit

## Method

Compared every live `src/bochs-core/**/*.{c,cc,h}` file with the pinned
Bochs 2.6 tree at `O:/repos.external/bochs-2.6-compat/bochs-2.6` using
`git diff --no-index --numstat`.  This is a source-layout audit; it makes no
guest-runtime claim.

## Disposition ledger

### Exact or lexical / true-subset boundaries retained in core

- `gui/paramtree.cc`: `+5/-2`; `BX-BUILD-002` token-spelling only.
- `cpu/i387.h`: `+7/-4`; include-root spelling only (`BX-BUILD-002`).
- `logio.cc`: `+0/-1`; newline-only identity preservation.
- `pc_system.cc`: `+3/-31`; whole unreachable product-shell `Reset` and
  `exit` methods are a true subset (`BX-MACH-023`).
- `iodev/pic.cc`: `+19/-3`; registered true-subset product shell omissions
  and the one-call private overlay boundary (`BX-MACH-024..026`).
- `iodev/pic.h`: `+4/-0`; declaration/friend seam only (`BX-MACH-024`).
- `iodev/keyboard.h`: `+3/-0`; `fini()` declaration only (`BX-MACH-027`).
- `iodev/keyboard_bridge.{cc,h}`: no upstream filename; they are the narrow
  core-owned private-overlay call boundary and have no external consumer.

### Requires body migration to `bochs-core-overlay`

- `cpu/exception.cc`: formerly `+189/-1`; **migrated**. The mirror now has
  one selector-blind private-overlay call under `BX-UD-001` / `BX-UD-002`.
  `bochs-core-overlay/cpu/exception_hook.cc` contains copied state/result/
  resume and first-fault observation bodies; `opaque_callback.cc` owns the
  private registration state.
- `memory/misc_mem.cc`: `+256/-1`; SIM-free memory initialization and checked
  backing-RAM helpers are a substantial new body.  Retain only minimal
  private-access seam(s); migrate initialization/copy logic under
  `BX-MEM-024` / `BX-CORE-DIV-006`.
- `memory/memory.h`: `+37/-0`; declarations are allowed only if their large
  implementations leave core.  Audit alongside `misc_mem.cc`.
- `cpu/init.cc`: `+25/-0`; real-mode profile/entry helper body requires
  private overlay under `BX-EXEC-016`.
- `cpu/cpu.h`: `+69/-0`; profile state and implementation members require a
  smallest boundary review with `init.cc`.
- Observers: `cpu/access.cc`, `access32.cc`, `cpu/cpu.cc`, `instr.h`,
  `soft_int.cc`, `ctrl_xfer16.cc`, `ctrl_xfer32.cc`, and `memory/memory.cc`
  carry default-off observation bodies (`BX-CORE-DIV-002..004`) and must be
  reduced to core call points plus private overlay implementations.

### Small profile predicates pending group migration

- `cpu/icache.cc`, `io.cc`, `stack.cc`, `string.cc`: each is small by line
  count but participates in the same real/V86 compatibility profile as
  `access*.cc`, `ctrl_xfer*.cc`, `exception.cc`, `init.cc`, and `cpu.h`.
  S4 treats them as one group; none is individually dismissed as harmless.

## Required order

1. **Completed for #UD and first-fault observation.** `exception.cc` now
   retains one selector-blind member call and no adapter include. The private
   overlay owns copied-record construction, validation and CPU mechanical
   result application. The slot ABI is `int (*)(void *context, const void
   *event, unsigned event_bytes, void *outcome, unsigned outcome_bytes)`:
   core sees no event fields, selector, provider or adapter header.
   `adapter-bochs` exposes registration; `app` composes the BOP route and
   optional first-fault observer behind the opaque callback. `adapter-bop`
   remains the typed BOP ingress provider. The first-fault record has the
   same treatment, so it no longer forms a hidden core-to-adapter dependency.
   Compile evidence: focused formal Ninja object closure on 2026-08-24.
2. Move SIM-free memory body behind narrow private memory access seams.
3. Move real-mode profile/entry body as one CPU compatibility group, including
   the small predicates listed above.
4. Re-measure every file, update README exceptions and prove overlay privacy
   with focused formal builds.

The #UD and first-fault bodies have now moved. Memory, real-mode compatibility
and the remaining observation groups remain active S4 work; this record is not
a closure claim.
## Closure result

All four admitted S4 body groups are now private overlay implementations:
`#UD`/first-fault, SIM-free checked RAM, real/V86 profile entry, and the
segment/IRET/software-interrupt/instruction-history/physical-write observers.
The adopted core retains only Bochs member declarations, default-off call sites
or one selector-blind opaque callback boundary. It includes no `adapter-*`,
OpenNT, app or session header.

Formal Ninja compiled the changed `cpu.h` consumer, `memory.cc`, physical-write
overlay and app composition objects on 2026-08-24, then produced refreshed
`bochs-core.lib` and `app.lib`. Focused physical-write, checked-RAM,
RAM-preflight and opaque-BOP-ingress fixtures exit zero. The physical fixture
uses the public `adapter-bochs` opaque callback; it does not import the private
overlay. The generic-context fixture's pre-existing diagnostic exit 2 is not
used as passing evidence.

Final dependency scans found no external include in `bochs-core`, no external
consumer of `bochs-core-overlay`, and no direct Bochs type/header/global import
in `adapter-softpc`. S4 is source/build/fixture closed; it makes no continuous
guest-runtime claim.