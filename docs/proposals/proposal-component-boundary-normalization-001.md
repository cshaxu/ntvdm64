# Three-program component normalization and adapter retirement

## Status and objective

M0 T418 is owner-readmitted with this revised proposal. It first completes the
already-admitted archive-cleanup proof, then performs a **no-behaviour-change**
physical reorganization so that project-owned executable components are legible
by process:

```text
src/run16-exe/   → run16.exe
src/basesrv-exe/ → basesrv.exe
src/ntvdm-exe/   → ntvdm.exe
```

The objective is to retire broad, free-standing `adapter-*`, `session` and
obsolete local broker ownership where they conceal a single executable's
private implementation. The two original source mirrors remain unchanged in
principle: `src/mvdm` and `src/opennt-host` retain their strict no-new-file rule
and own only original source or minimal owner-local divergences.

## Final component model

| Component | Owns | Must not own |
| --- | --- | --- |
| `run16` | public CLI `main.c`; discovery; original classifier invocation; BaseSrv startup/connection; suspended worker creation/registration; parent completion wait | BaseSrv DOS/WOW records, worker machine/session state, Console presentation, a new command parser |
| `basesrv` | thin `main.c`; RPC endpoint/authentication/rundown; stream/receipt transfer; Console membership; broker liveness; transport assembly around `opennt-host` `srvvdm.c` | original DOS/WOW record selection, queue policy, or a replacement of BaseSrv semantics |
| `ntvdm` | original entry behind the thinnest process-local pre-init; worker lifecycle; guest-memory lease; thread binding; termination cleanup; SoftPC/CCPU and worker-local Console presentation | cross-process broker policy, launcher creation, global reusable session management |
| `product-abi` / `product-package` | deliberately tiny shared product-version identity and package/media layout contract | generic utilities, state, process policy or a generic `common` component |

The BaseSrv RPC protocol contract is owned by `basesrv`; generated client
artifacts are build outputs. Product identity remains in the shared fixed-width
`product-abi` contract. Native handles, worker session pointers and guest
pointers never enter that boundary.

## Cross-executable code rule

"Shared" is a linkage fact, not a reason to create a fourth generic runtime
component.  Each such source item has one semantic owner and may be linked into
more than one executable only through that owner's narrow static library:

| Kind | Final owner and use |
| --- | --- |
| Original OpenNT/MVDM algorithm | Its existing file in `mvdm` or `opennt-host`; each consumer links the selected original-owner library. |
| BaseSrv protocol, client stubs and copied wire records | `basesrv`; `run16` and `ntvdm` may link its versioned client library, but cannot own a second protocol implementation. |
| Process-local Win32 operation | The one executable whose process owns the HANDLE, Console, thread or lifetime: `run16`, `basesrv`, or `ntvdm`. |
| Same-shaped unavailable historical host ABI needed by several owners | `opennt-abi/host-compat`, only when its README names the original declaration, finite public-Win32/NTDLL binding and consumers. It is a host-ABI compatibility island, not a `common` utility layer. |
| Product version and package/media constants | Stateless `product-abi` and `product-package` only. |

Thus no new `win32api`, `common`, or generic compatibility root is admitted.
Duplicated wrappers are removed or moved to the owner above; a function that
only happens to call Win32 does not become shared merely for build convenience.

## Required source placement

- Move a binding out of `adapter-*` only when exactly one executable owns all
  callers and its lifetime. Preserve source-facing names, ABI and failures in
  a named subdirectory of that executable component.
- `session` is not a reusable cross-product service. Its worker-local
  lifecycle, lease, bind, termination and presentation state move into
  `ntvdm`; `session.lib` becomes a private `ntvdm.exe` link group.
- BaseClient bindings divide by caller: run16 launcher bindings and worker
  bindings go to their owning process; BaseSrv server bindings and transport
  facilities go to `basesrv`.
- Worker-only SoftPC, CCPU, monitor, VDD, Redirector, WOW, debugger and Console
  bindings enter `ntvdm` under named subfamilies, never either original mirror.
- The obsolete local `broker.lib` plane is removed first. Live authenticated
  transport remains in `basesrv`, never as a second neutral broker component.

## Sequential S packets

### S1 — legacy broker archive proof and removal

Freeze all legacy broker source/export/link/test edges. Build a fresh x86
archive-free graph, inspect product and fixture maps, and run regressions. Only
then delete the six proved-unused local-state files, obsolete tests and every
build edge. Retain live RPC transport.

### S2 — product ABI/package extraction and run16 ownership

Create only the named `product-abi`/`product-package` surface needed by all
programs, then move run16-only entry and BaseClient-launch bindings to
`src/run16`. Preserve discovery, version rejection, suspended-worker rollback
and parent wait behaviour.

### S3 — basesrv ownership reorganization

Move service entry, protocol contract, authenticated transport and BaseSrv
bindings to `src/basesrv`; retain `srvvdm.c` as original record owner under
`opennt-host`. Prove connection/rundown, stream/receipt, Console membership,
protocol mismatch and broker-death behaviour without changing DOS/WOW policy.

### S4 — ntvdm worker-local reorganization

Move `session` and all worker-only adapter families to `src/ntvdm`, making the
original entry's standalone pre-init explicit and private. Preserve lease,
CCPU, Console, native-child teardown, Redirector, VDD, WOW and debugger
boundaries without turning them into mirror code.

### S5 — three-program build, shared-boundary and runtime closure

Regenerate the formal x86 graph using only final component roots. Prove no live
`adapter-*`/`session`/legacy-broker source or archive edge remains, then run
version checks, broker death/pending command, COMMAND/MEM, nested COMMAND,
EDIT keyboard/mouse and EDIT-exit→MEM. Publish only a tested package to
`O:\winnt`; prove the cross-executable rule by source/include/link sweep;
report mirror diff, adapter deletion, relocation and retained boundary counts
separately.

### S6 — non-mirror footprint recovery audit

Reopen only because S1 deleted the dead local broker plane while S2--S5 mostly
relocated live code. Inventory every selected body below `run16`, `basesrv`,
`ntvdm`, `opennt-abi/host-compat`, `product-abi` and `product-package`.
For each, record its physical-line count, callers/build targets, original
owner/source-first recovery ladder and exactly one disposition: delete,
restore original owner, retain as finite binding, or defer with a named
unavailable boundary. Do not make a source or behavior change in this audit.
The resulting grouped proposal must distinguish actual removable lines from
relocation and state the focused/runtime proof required for each deletion or
restoration group.

### S7 — proven dead-code retirement

Apply only source-proven S6 retirements. Remove the no-caller/no-map duplicate
image classifier, the unselected duplicate `HostIdleNoActivity` provider and
the unreferenced unavailable Redirector-thread stub. Remove formal product
archive inputs only when the final worker/DLL maps prove that they contribute
no provider; preserve a still-useful historical fixture as test-only material,
not a product dependency. The mixed termination/observation source is
explicitly out of this S: first split its real standalone termination binding
from its mirror observation calls in a separately evidenced package. Rebuild
all x86 products and fixtures, re-run the established three-program matrix,
then deploy the tested package to `O:\\winnt`.

## Stop conditions and acceptance

Any move that changes a public wire layout, original ordering, state owner,
teardown, standard-handle lifetime, lease epoch, Console behaviour or original
DOS/WOW record policy stops that S for separate design review. A directory move
alone is never acceptance.

Closure requires all S evidence, fresh MSVC Win32/x86 `/MT` links for three
products and fixtures, a clean production source/build sweep, and the runtime
matrix above. The final report distinguishes true deletion from relocation and
names every retained modern boundary with its unavailable original dependency.
