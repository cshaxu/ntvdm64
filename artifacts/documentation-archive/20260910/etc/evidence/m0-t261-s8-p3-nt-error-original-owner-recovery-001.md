# M0 T261 S8 P3 — `nt_error.c` Original-Owner Recovery

## Question

Can the reached OpenNT `host_direct_access_error(ULONG)` owner replace the
project top-level shim while preserving its original function shape and
Abort/Ignore observable result?

## Inputs

- `refs/opennt/base/mvdm/softpc.new/host/src/nt_error.c`.
- The replaced `opennt-host/top_level/top_level_nosupport_shim` source-derived
  seam.
- Existing public-Win32 `adapter-win32/facade/opennt_error_dialog_facade`.
- BOP 59 ingress and its focused fixture.

## Procedure

1. Re-rooted the compatibility declaration and implementation as
   `opennt-host/softpc.new/host/inc/nt_error_compat.h` and
   `opennt-host/softpc.new/host/src/nt_error.c`.
2. Recovered `host_direct_access_error(ULONG)` in the original order:
   current-thread duplicate check, category record, prompt construction and
   Abort/Ignore dialog.
3. Kept only three individually registered divergences: no NT4 product TLS
   initializer, safe handling for an out-of-range guest category, and no
   private `ntvdm.exe` resource table.  The last step uses the existing public
   Win32 dialog facade; it does not change BOP 59 ingress semantics.
4. Deleted the old duplicate forwarding definition in DEM composition so all
   source callers resolve to the single re-rooted owner.
5. Generated fresh formal root `build/t261/s8-r004`, built it, and ran the
   BOP 59 fixture.

## Observations

- Fresh MSVC x64 `/MT` Ninja graph `r004` completed **467/467** edges after
  the one duplicate forwarding definition was removed.
- `t237-s5-bop-nosupport-fixture.exe` exited `0`.
- A live-source/test/manifest scan for `top_level_nosupport_shim` and
  `src/opennt-host/top_level` returned no match.
- `git diff --check` passed.

## Interpretation

The only reached `nt_error.c` direct-access owner now resides under its
original OpenNT host path.  It retains per-thread same-category suppression,
the source function name and parameter, and the two observable choices:
Ignore lets BOP 59 resume at its original next instruction; Terminate remains
a typed VDM controlled stop.  It does not terminate the host process.

The resource, WOW, console-positioning, CSRSS and other unreachable NT4
product-shell portions of the 981-line original translation unit are not
claimed as independently composed.  Their absence is explicit in the local
divergence register rather than hidden behind an unconditional Ignore path.

## Follow-up

P4 is the remaining S8 owner recovery: replace `redir_session_shim` with
re-rooted reached VDMREDIR source units and declared adapter/session seams.
