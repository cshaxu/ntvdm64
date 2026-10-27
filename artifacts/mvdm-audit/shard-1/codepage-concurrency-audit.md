# Console code-page concurrency: shared-Console investigation

## What must be preserved

Original `src/mvdm/dos/command/cmdexec.c::cmdCreateProcess` ordinary native
launch has no `CREATE_NEW_CONSOLE`; independent processes attached to that
Console therefore use one OS input CP and one OS output CP. Original
`vdmutils/graftabl/graftabl.c` calls the public setters/getters itself.
The proof of actual current divergence and one DOS text consequence appears
in `standalone-tool-chain-audit.md`. Neither original NT4 nor original
GRAFTABL execution has been experimentally compared here.

Microsoft documents that input and output code pages belong to the Console,
not to an individual screen buffer or application:
[Console Code Pages](https://learn.microsoft.com/en-us/windows/console/console-code-pages).
It documents a process attached to at most one Console, while a Console can
have many processes: [Attaching to a Console](https://learn.microsoft.com/en-us/windows/console/attaching-to-a-console).
`CONOUT$` opens the **active** screen buffer, not an arbitrary private
buffer, and a Console can own several screen buffers:
[Console Screen Buffers](https://learn.microsoft.com/en-us/windows/console/console-screen-buffers).
These are documented API semantics, not a demonstration that changing the
current carrier architecture is safe.

## Current hook scope and what it cannot prove

`src/nthook32-dll/entry.cpp` loads its context and calls `nthook_attach`;
`create_process.cpp::nthook_attach` installs Detours on only `CreateProcessA`
and `CreateProcessW`. `ntvwm-exe/execution.c::launch_request` installs it on
the direct target before resume, and `create_process.cpp::finish` propagates
installation to applicable created children. `NATIVE_HOOK_LAUNCHER`, explicit
separate/new/detached/no-window cases, debug-created children, failure paths,
non-project processes and external Console attachers have different coverage.
There is currently no CP API interception. The frontend/execution capabilities
are removed for non-CUI or separate/no-Console children; the hook payload is
not a general authority to operate arbitrary Consoles.

Even if the four public functions `GetConsoleCP`, `GetConsoleOutputCP`,
`SetConsoleCP` and `SetConsoleOutputCP` were intercepted for every injected
process, an uninjected process attached to NTCON could change or read its CP
independently. Direct console internals or different API entry paths also need
coverage evidence. Returning an NTCON getter result in a hooked program while
its kernel Console actually has another CP does not make ordinary
`WriteConsoleA`, `ReadConsoleA`, or other APIs use the NTCON mapping. Issuing
setters in two Consoles cannot make the update atomic for uninjected readers:
there is necessarily an observable interval and either setter may fail.
Intercepting only project-owned setters is therefore a partial compatibility
measure, not proof of original immediate two-way semantics or one authority.
It is also a new potentially blocking hook call path; RPC/transport reentry,
last-error semantics, disconnect and loader-lock restrictions would require
explicit design. No such hook is proposed as an automatically authorized
replacement for original behavior.

## Architecture candidates, ordered by semantic fidelity

**A. Shared real Console, separated screen/input resources.** After NTSRV has
coordinated the correct ownership and identity, arrange for NTVWM's native
CUI carrier and NTCON to be attached to the same actual Console. Windows then
owns the sole CP state, and original native Set/Get and third-party calls see
it without polling, interception, mirrored CP or a new authority. Create
additional screen buffers for private rendering where valid; this is a
candidate architecture, not a proven drop-in patch.

The Console's active buffer, `CONOUT$`, input queue and control events are
also shared. The existing implementation explicitly requires a separate
`CREATE_NEW_CONSOLE` carrier, replaces worker-local standard endpoints and
captures its hidden `CONOUT$`; simply attaching the carrier to the frontend
would cause `CONOUT$` opens to refer to the shared active buffer. A separate
inactive buffer passed as standard output does not intercept a target that
opens `CONOUT$` or switches its active screen buffer. Input readers may steal
one another's events, and input mode, Ctrl+C handlers, geometry, title,
process attach/detach and close lifetime are also shared. The design must use
NTSRV's existing management and worker-base presentation path without letting
Run16/NTVWM/NTCON invent independent control links. Avoid claiming it is
feasible for the complete product until all affected I/O and lifecycle
semantics have been proven against real native and DOS applications. A direct
`AttachConsole` call by a component is not by itself authorization to bypass
NTSRV's coordinated identity/lifetime model.

**B. Two Consoles plus hook and handoff projection.** The previously
documented transition synchronization in `codepage-handoff-design.md` can
address the reproduced sequential DOS case. Adding CP hooks could improve
immediacy for cooperating injected target calls. It cannot provide original
universal concurrent atomic visibility: third-party mutation and unhooked
consumers still expose split authorities. Transition projection alone can also
overwrite an external change made during the native lease. Unless the exact
coverage can be proven sufficient or the user explicitly approves the
observational boundary, B is not a complete replacement for A and must not be
reported as equivalent to original shared Console behavior.

**C. A fresh common CP registry, polling or private Console internals.** These
introduce a second authoritative state, hidden OS-layout assumptions, or
unbounded/interval-dependent divergence, and cannot preserve unhooked Win32
Console API behavior. They do not satisfy the current architecture principle.
No workaround or new semantic concession is authorized by this audit.

## Verification plan before selecting A or implementing a fallback

1. A small standalone Windows fixture on an isolated physical Console: two
   processes share one Console; create an additional inactive buffer; have each
   process alternately call input/output getters and setters and verify the
   other process sees the result after completion, including invalid CP and
   last error. Also verify standard output handle and newly opened `CONOUT$`
   resolve to the expected different buffers. Compare against the current two
   Console topology. This proves only documented primitives, not the product.
2. A product-scoped prototype must preserve actual DOS -> native CUI -> DOS
   I/O, input ownership, standard handles and redirects, `CONOUT$` opened
   directly by native targets, `SetConsoleActiveScreenBuffer`, window mode,
   Ctrl+C, process tree and NTSRV-managed close/death. Run both existing-parent
   and root Console cases with an independent external process, interleaving
   bidirectional CP changes with reads/ANSI writes; verify no duplicate/lost
   keystrokes or unintended frontend window changes. Include same-Console
   and nested CUI cases, stale handles, release barriers, and error returns.
3. If A fails any required behavioral constraint, document its concrete
   failure and minimum missing OS mechanism before proposing B's exact
   observed limitation for approval. Do not equate a partial hook-coverage
   pass, or the previously reproduced sequential case, with full parity.

No architecture candidate has been implemented or experimentally evaluated in
this audit. Existing source and Microsoft documentation establish the API
constraints and show why a two-Console hook alone cannot prove full parity.

## Isolated Windows primitive witness (2026-10-26)

A standalone CUI parent created its own hidden fresh Console, wrote `A` to
its active buffer and `B` to an additional inheritable inactive buffer, set
input/output CP to 437, and wrote a `Q` key record to its own input queue.
An inherited-Console child opened `CONOUT$`, read its first cell (`A`), and
read the inherited inactive-buffer handle (`B`). It then set both code pages
to 850 and consumed the input record. Parent observed both CPs now 850 and
its input queue empty. Parent reset both to 437; a second child observed 437
and again read active `A` through `CONOUT$` and inactive `B` by the supplied
handle. Process exit code was 0.

```text
parent-before a=437 b=437
child-buffers a=1 b=65 c=1 d=66
child-pages-before a=437 b=437
child-set a=1 b=1 c=850 d=850
child-input a=1 b=1 c=1 d=81
parent-after-child a=850 b=850 c=0
child-pages-before a=437 b=437
parent-after-reset a=437 b=437
```

`a/b/c/d` are positional fields as emitted by the archived source
`console-primitive-probe.c`. The probe ran under a hidden isolated Console;
no NTSRV, product process or guest was started or modified. It was compiled
with local `gcc -O2 -Wall -Wextra`; a missing-initializer warning on the
zero-filled `STARTUPINFOA` was emitted. Source SHA-256
`4059537EFF575C25D8ABB20464EC2D9B8F48803D7AE84C1C2C4488B904A06ACD`;
executable SHA-256
`26156A7003C39FAE2EDD4B4806DE185B46846EDC109B9914127E79D9127CEBDE`.

This confirms the Windows primitives in one sequential control and demonstrates
why separate buffers do not isolate the input queue or `CONOUT$`. It does not
verify simultaneous CP operations, native independent Console APIs, keyboard
control events, or a compatible NTCON/NTVWM product architecture; scenario A
above still requires the product-scoped test matrix before it is selected.
