# M0 T451 S4 — object consumer and handoff audit

## Scope

S4 accepts the S1--S3 shared-object boundary; it is not an authorization to
add another USER view or to reimplement original WOW32/control algorithms.
The single retained object chain remains:

```
modern USER facts -> S2/S3 page-domain copied view -> original WOW32/USER client
```

The native HWND and public USER storage remain host authority. The page domain
owns the one original-layout projection. Original WOW32 and admitted original
client bodies consume that projection; a binding may provide only the narrow
host service that modern USER cannot expose.

## Direct private-word consumer denominator

The selected provider builds 82 `src/mvdm/wow32` translation units through
`mvdm/wow32/precomp.h`. That common precomp explicitly defines
`WOW_USER_PRIVATE_REDIRECT` before including `wow_user_private_access.h`.
The source inventory finds 15 actual original units that call a Window/Class
Long API:

| Original consumer group | Units | Route |
| --- | --- | --- |
| WOW word/class lookup and dispatch | `walias.c`, `wcall32.c`, `wmdisp32.c`, `wmsg16.c`, `wumsg.c`, `wuclass.c`, `wuwind.c` | `FindPWW`/`FindPWC` and direct dispatch use the redirect's scoped PWW/PWC service. |
| Original subclass, hook, dialog and common-dialog paths | `wsubcls.c`, `wowhooks.c`, `wudlg.c`, `wcommdlg.c`, `wdde.c`, `wstruc.c`, `wow32fax.c` | Same common precomp/redirect; public indices remain native USER calls. |
| Diagnostic reader | `debug.c` | Same route, but not relied on as product acceptance. |

The redirect has one deliberate split, not two owners:

* `GWL_WOWWORDS` and the three `GWL_WOWDWORD*` slots borrow/exchange the
  retained page-domain-backed `PWW` through `wow_user_borrow_scope`.
* `GCL_WOWWORDS` and the two class words borrow the retained `PWC` class
  binding.
* `GWL_WNDPROC`/`GCL_WNDPROC` preserve native USER procedure selection and
  decode only the project's immutable guest entry. All other indices delegate
  to native USER.

This is necessary adaptation rather than an autonomous replacement: modern
USER has no OpenNT `GWL_WOWWORDS`/`GCL_WOWWORDS` slots, while the original
provider's direct readers require aliases with the original lifetime. The
selected original private-word reader/client/server objects are linked by
`New-T404S5Wow32ProviderNinja.ps1` as `rtl/getset.c`, `client/ntstubs.c`, and
`kernel/getset.c`; the adapter does not replace their selected behavior.

## Original control/client denominator

The provider's selected control cohort comprises the original EDIT, combo,
list, dialog, button, default-window and client helper bodies named in
`New-T404S5Wow32ProviderNinja.ps1`. Its only direct WindowLong occurrences in
the admitted client subset are comments or the original `NtUserSetWindowLong`
client service; it does not contain a second private PWW/PWC reader. The
control precomp instead supplies the admitted client-state validator and
original layout declarations, so an observed/foreign resource cannot be used
as local control storage.

## S4 acceptance plan

1. Rebuild the coherent x86 provider/worker and the required x64 host cohort
   once from the current tree; preserve the stable Ninja/ccache roots.
2. Run the direct-data/callback/lifetime 19-row private-desktop group from a
   desktop-capable token, retaining source/artifact hashes.
3. Run the applicable same-artifact package regressions, including immutable
   guest integrity and the established WINMINE/SOL/WRITE frontiers where their
   environment permits. A desktop/layout constraint is recorded as an
   integration boundary, never recast as a product pass.
4. If a consumer bypass or competing owner appears, trace it to the selected
   original body and repair the route as one batch before rebuilding. Do not
   add a cache, a synthetic private USER structure, or a guest change.

The direct-consumer source audit found no such bypass at admission. The
following build/acceptance group is therefore verification and handoff, not a
search for a speculative new implementation.

## Executed acceptance evidence

On 2026-10-23 the coherent cached build group completed without product source
changes.  It covered the x86 WOW32 provider and worker, and the x64 worker,
service, launcher, frontend, monitor and hook cohort.  The resulting ten
published artifacts were byte-identical to `assets/release`:
`run16.exe`, `ntsrv.exe`, `ntcon.exe`, `ntvwm.exe`, `ntmon.exe`, `ntvdm.exe`,
`WOW32.DLL`, `VDMREDIR.DLL`, `nthook32.dll`, and `nthook64.dll`.

The established private-desktop WINMINE integration regression then passed
from `build/t451-s4/winmine-r5`.  Its result records flag/counter, reset,
open-cell, `WM_CLOSE`, and normal Run16 completion.  The run used the current
standard observer built from `tests/observation/console_startup_observer.c`:

```
build/.ninja/msvc-x64-release/message-probes/console-startup-observer.exe
```

An earlier candidate-package file named `private-desktop-observer.exe` has a
different hash and is not this source-built observer.  It must not be supplied
to `verify-wow-mine-gameplay.ps1`; doing so has a different command contract
and produces no worker evidence.  This is a test-tool selection correction,
not a product fallback or behavioral exception.

The S3 direct-data/callback/lifetime private-desktop group remains the
applicable 19-row boundary proof (`failed: 0`), recorded in its closure
evidence.  The S4 run proves the selected same-artifact game frontier through
the one retained object chain; it introduces no second view or guest change.

## Historical frontier harness reconciliation

The retained `verify-wow-headless-frontiers.ps1` had not followed the present
single product layout.  Its original form directly read host images and
`WFWNET.DRV` from the package root, although the current package has one host
binary root at `<package>\\system32`; it also passed a bare guest name and a
non-normalized root to the observer.  The shared test helper already defines
`Get-PackageBinaryRoot` for exactly this compatibility boundary.  The script
now imports that helper and uses its resolved binary roots only for host-image
identity, process ownership and the launcher.  Guest media remain at the
package root and are passed by full path.  Test `TEMP`, `TMP` and `PATH` are
temporarily pinned to the isolated short stage and restored in `finally`,
matching the accepted WINMINE gate.

This reconciliation is test-only: it neither changes a product artifact nor
adds a product fallback.  The initial runs supplied a non-standard packaged
observer or a long build path and therefore ended before useful product
observation.  After the standard observer plus the short physical stage were
used, the exact window reader observed the visible immutable WINMINE
`Minesweeper` window and the visible immutable SOL `Solitaire` window, both
hosted by the one staged `ntvdm.exe` and with no `ntcon.exe`.  The historical
localized/oom-only identities are retained as alternatives; the precise
English identities are now accepted as equally specific frontiers.

The complete three-program legacy frontier script has not yet produced its
final all-three summary in this invocation: its SOL observation reached the
window frontier but the controller did not emit its final timeout report
before the harness ended.  This is recorded as a harness/liveness evidence
gap, not a failed consumer or a claim that SOL/WRITE behavior is proved.  It
does not weaken the passed dedicated WINMINE interaction regression.
