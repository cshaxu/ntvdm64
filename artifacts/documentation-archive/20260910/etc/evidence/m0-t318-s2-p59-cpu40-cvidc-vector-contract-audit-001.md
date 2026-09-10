# M0 T318 S2 P59 — CPU40 C-VID vector-contract audit

## Question

Does the selected CPU40 CCPU/C-VID assembly leave an uninitialised CPU-frame
or a first-start vector slot that can be repaired safely from the current
fixed-container `0xC0000005` observation?

## Original startup order

The selected original `base/support/main.c` calls, in order:

1. `cpu_init()`;
2. `setup_vga_globals()`;
3. `host_start_cpu()`.

With `CPU_40_STYLE`, `CCPU` and `NTVDM` selected, original
`ccpu386/c_main.c::c_cpu_init` calls original
`ccpu386InitThreadStuff()` before `c_cpu_reset`.  Original `ntthread.c`
allocates the TLS index and installs the current thread's `ThreadSimBuf`.
Original `host_start_cpu()` then calls `cpu_simulate()`, whose CCPU provider
is `c_cpu_simulate()`.  Its `setjmp(*ccpu386SimulatePtr())` consequently has
an established main-thread buffer.  A missing CCPU TLS buffer is therefore
not a source-supported explanation of the current access violation.

The selected C-VID `ev_glue.c::setup_vga_globals` invokes the registered
`mvdm_cvidc_bind_vectors()` overlay before `host_start_cpu()`.  The overlay
binds the original CCPU `Cpu`, `Sas` and `C_Video` tables from the original
generated `c2cpusad.h` contract; it does not introduce a second CPU or a
guest-memory mapping route.

## Generated vector disposition

The generated binding has 209 named public/private slots.  183 have an
original CCPU provider under the selected CPU40/SPC386 compile profile.  The
remaining 26 slots are deliberately unavailable in that profile:

- 8 legacy interrupt/I/O table entries: `ClearHwInt`, `InitIOS`, and the six
  `DefineIn*`/`DefineOut*` entries;
- 6 486-only status entries: `SetSTATUS`, `SetAC`, `GetAC`, `GetET`, `GetNE`,
  `GetWP`;
- 12 private compiler/debugger/JIT entries, including CPU-state snapshots,
  block compilation and universal-handle constraints.

These are not silently treated as successful calls.  They remain null because
the selected original CPU40/SPC386 provider has no same-profile definition.
For example `c_reg.c` defines the AC/ET/NE/WP accessors only under `SPC486`;
the selected product is `SPC386`.  Inventing no-op replacements would change
the original unsupported-profile contract.

`accessfn.c` and `host/src/nt_aorc.c` expose generic wrapper symbols for
historical external callers, so the 26 slots are a real **future generic-vector
closure** item.  However the selected source path through `host_start_cpu →
c_cpu_simulate → ccpu(FALSE)` uses CCPU's direct generated macros, not these
generic wrapper slots.  The only direct source occurrence of
`cpu_clearHwInt()` in `nt_cpu.c` is compiled out when `CCPU` is selected.
No source-supported first-start call to an unavailable slot was found.

## Result

The original CCPU main-thread frame and the required C-VID binding are in
place before the first normal fetch.  The generic C-VID vector tail remains a
bounded, source-visible CPU40/SPC386 capability gap, but it is neither safe to
fill speculatively nor currently attributable as the fixed-container crash
root.  It transfers as a complete owner-contract cohort: before any caller is
enabled, its requested slot must receive either its original same-profile
provider or an explicitly admitted source-shaped equivalent.  No BOP route,
mapping-manager path, observer change or product repair is admitted by this
audit.
