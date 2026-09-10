# M0 T318 S2 P34 — Fixed-container repeat and console prerequisite audit

## Fixed observation

The immutable r30 x86 product was run once more through the same formal
non-debug, console-owning observer.  The product hash, staged media and
firmware, `-f -o --ordinary-child` arguments, real console ownership, and
eight-second limit were unchanged.

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=8000
```

This is a repeat of P30's formal result, not a new execution claim.  It does
not establish NTIO, BOP, NTDOS, DEM or COMMAND reachability; it confirms that
the CPU40 P33 audit neither changed the product nor changed the observed
failure.

## Startup console/host prerequisite cluster

The original startup order places this cluster before DOS image loading:

1. `ntvdm.c` calls `TimerInit`, `CpuEnvInit`, `nls_init`, then `host_main`.
2. `host_main` calls original `host_applInit` before `config`, CCPU init and
   `InitialiseDosEmulation`.
3. With the admitted `-f` contract, `host_applInit` in `nt_reset.c` takes the
   normal DOS route, initializes ICA locking, then calls `init_host_uis` and
   `nt_start_event_thread`.
4. `init_host_uis -> InitScreenDesc -> SetupConsoleMode` obtains standard
   handles, uses `CONIN$`/`CONOUT$` only if a standard endpoint was redirected,
   records SCS's standard handles, and establishes console modes.
5. `nt_start_event_thread` constructs its keyboard mutex, events and console
   event worker.  Its worker uses the source-shaped `GetConsoleInputWaitHandle`
   and `ReadConsoleInputExW` compatibility calls.

The formal container's real console is therefore an intentional prerequisite,
not an observation convenience: it lets the original Console-shaped code use
ordinary public console handles rather than fall into its `DisplayErrorTerm`
failure branches.  `DisplayErrorTerm` itself retains original error formatting
and dialog sequencing.  It is not reached merely because the normal console
path exists.

## Existing compatibility disposition

- `GetConsoleInputWaitHandle` returns the public waitable input endpoint,
  matching the required wait role without recreating an NT4 Console Server.
- `ReadConsoleInputExW` preserves the source call shape and maps the selected
  no-wait behavior to public console operations.
- `SetLastConsoleEventActive` is a typed session video event, not an implicit
  BOP or CPU stop.
- The full-screen/graphics-only console server calls remain outside the
  ordinary stream-console start path and must not be enabled to diagnose this
  access violation.

## Result and next static unit

No direct `DisplayErrorTerm`, return-120, private Console/CSR failure path is
shown as reached by the fixed observation.  No console mutation is admitted:
the container is deliberately already the original normal-case shape.

The next owner unit is the complete `host_applInit` initialization cluster,
not a single console API: original `DuplicateHandle`, ICA lock creation,
`init_host_uis`, and event/heartbeat worker initialization must be compared
against their modern adapter/session contracts as one pre-NTIO cluster.  A
new product is justified only if that review identifies a concrete broken
contract; otherwise the next observation remains the same fixed container.
