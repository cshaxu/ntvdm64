# M0 T463 S3 — lifecycle and owner acceptance

## Question and fixed boundary

Can the sealed T463 S2 package retain ordinary WINMINE, SOL and WRITE
behaviour while a WOW task closes, a shared WOW worker accepts another task,
and the original shell remains the registered launch receiver?  This record
closes that finite lifecycle question only.  It does not claim the later
cross-task message identity or full provider contract, and it changes neither
guest media nor a production image.

## Inputs and authority

The runtime input is the published `build/t463-s2/product-r3` ten-image
package, whose publication and 322144-ms Full-gate hash record is
[the S2 delivery](m0-t463-s2-sol-options-attribution.md#final-coherent-delivery).
The immutable application inputs are `WINMINE.EXE`, `SOL.EXE` and `WRITE.EXE`
from that package.  The selected original chain is unchanged:

```text
BaseSrvCheckWOW -> WM_WOWEXECSTARTAPP -> WOWEXEC registered shell
```

`src/opennt-host/base/win32/server/srvvdm.c` publishes the request;
`src/mvdm/wow32/wkman.c` owns shell-window registration; and
`src/mvdm/wow16/test/shell/wowexec.c` receives and starts the requested
application.  No project-owned lifecycle replacement was introduced.

## Reproducible acceptance

`tests/observation/verify-wow-write-lifecycle.ps1` compiles the checked-in
`wow_write_lifecycle_probe.c`, creates one owned private desktop and launches
two `run16 --wait WRITE.EXE` rounds on that same desktop.  It identifies the
reused `ntvdm.exe` by its exact staged full image path, observes the real
`MSWRITE_MENU` window, sends it `WM_CLOSE`, and requires disappearance and
successful request completion before the next round.  The current run is
`build/t463-s3/write-lifecycle-r12`:

```text
PASS WRITE round=1 close=observed exit=0
PASS WRITE round=2 close=observed exit=0
PASS WRITE normal close followed by clean restart and second normal close
```

The earlier apparent second-round failure was rejected: it launched the
second application in the correctly retained worker but observed a different
private desktop and incorrectly treated short-lived `run16` as the app
owner.  The corrected test records the actual shell/worker ownership.

`tests/observation/verify-wow-idle-workers.ps1 -SharedTasks -LifecycleOnly`
is the narrower shared-worker lifecycle witness.  Its current run,
`build/t463-s3/idle-lifecycle-r5`, proves two distinct tasks overlap in one
worker, each has exactly one completion/retirement, neither emits idle work
after retirement, and the surviving registered task continues after its peer:

```text
PASS shared worker 2404, two distinct tasks overlapped and retired independently, both requests exited normally
```

`-LifecycleOnly` deliberately does not require an unregistered peer's idle
vector to execute on another task.  That is a message-identity behaviour
reserved for the later W01/W11 work; accepting it here would weaken neither
the lifecycle assertion nor the later contract.

The pre-existing package witnesses remain applicable because no production
source or package input changed: `mine-normal-r1` passed flag/counter/reset,
open-cell, close and launcher completion; `sol-normal-r1` passed deal, stock
draw, close and launcher completion.  S2's Full gate additionally covers the
17 text routes, RPC, GUI, session, retirement and nested-window paths.

## S5 tool audit and disposition

The only S5 code is the two checked-in lifecycle probes above and their
strictness adjustment.  The WRITE probe now reports `ERROR_TIMEOUT` on either
observation deadline rather than a misleading success error, owns one desktop
for both rounds, pins its worker by exact staged image path, and terminates
only its own failed child.  The runner no longer carries unused observer
state.  `verify-wow-idle-workers.ps1` preserves the existing strict default;
the opt-in lifecycle slice is rejected unless `-SharedTasks` is present.

There is no uncommitted `tools/` change.  The separate reviewed tool/import
delivery remains `c1729f5a9`; its later tracked tool changes are already
committed, and `books/` remains untracked and excluded.  This S therefore
does not rebuild, repackage or redeploy identical ten-image binaries.  The
published S2 package remains the release authority.

## Result and handoff

T463 is complete: S1 froze the ordinary baseline, S2 repaired the
source-attributed SOL Options public-HWND boundary and published the coherent
package, and S3/S5 proved the admitted normal shared-worker lifecycle.
Unfinished extended service and full message-identity capabilities remain in
their named Queue candidates.  The next candidate begins a new acceptance
baseline and must return every newly observed missing owner to that owner's
package; it may not use this record to claim provider-wide completion.
